#include "blame.hpp"

Blame *Blame::m_Instance = NULL;

Blame::Blame()
{
    std::cout << "Blame" << std::endl;

    m_Screen = NULL;
    m_ParticleManager = NULL;
    m_Player = NULL;
    m_TilesSS = NULL;
    m_CurrentLevel = NULL;
    dbg_txt = NULL;

    // default screem dimensions
    m_ScreenWidth = 800;
    m_ScreenHeight = 600;

    // init vars
    m_DeltaTime = 0;
    m_Instance = this;
    m_DebugMode = false;

    init();

}

Blame::~Blame()
{

}

bool Blame::init()
{
    // init render window
    m_Screen = new sf::RenderWindow(sf::VideoMode(m_ScreenWidth, m_ScreenHeight,32), "Blame Restarts");
    m_View = sf::View(sf::Vector2f(m_ScreenWidth/2, m_ScreenHeight/2), sf::Vector2f(m_ScreenWidth, m_ScreenHeight) );

    if(!m_Screen) {std::cout << "Error creating render window.\n"; exit(1);}

    // render settings
    m_Screen->setFramerateLimit(60); // 60fps limit
    m_Screen->setVerticalSyncEnabled(true);

    // init tile sprite sheet
    const int tilesw = 1;
    const int tilesh = 1;
    m_TilesSS = new SpriteSheet(".\\data\\art\\tiles.png", tilesw, tilesh);

    // create fonts and debug text
    m_Font.loadFromFile(".\\data\\font\\droidsansmono.ttf");
    dbg_txt = new sf::Text("test", m_Font, 12);

    // create particle manager
    m_ParticleManager = new ParticleManager;

    // start new game
    newGame();

    return true;
}

Tile *Blame::createTile(int tilenum)
{
    Tile *newtile = new Tile;

    switch(tilenum)
    {
    case 1:
        newtile->sprite = m_TilesSS->createSprite(tilenum-1);
        newtile->id = tilenum;
        break;
    default:
        std::cout << "Error creating tile, tilenum " << tilenum << " is not valid.\n";
    case 0:
        newtile->sprite = new sf::Sprite();
        newtile->blocked = false;
        newtile->id = 0;
        break;
    }

    return newtile;
}

bool Blame::registerGameOBJ(GameOBJ *tobj)
{
    if(!tobj) return false;

    // check that game object isn't already registered...
    for(int i = 0; i < int(m_GameObjects.size()); i++)
    {
        if(m_GameObjects[i] == tobj) return false;
    }

    m_GameObjects.push_back(tobj);

    return true;
}

bool Blame::destroyGameOBJ(GameOBJ *tobj)
{
    if(!tobj) return false;

    // check that game object exists
    for(int i = 0; i < int(m_GameObjects.size()); i++)
    {
        if(m_GameObjects[i] == tobj)
        {
            delete m_GameObjects[i];
            m_GameObjects.erase(m_GameObjects.begin() + i);
            return true;
        }
    }

    // object not found
    return false;
}

void Blame::newGame()
{
    //  init random seed
    srand( time(NULL));

    // delete all current game objects
    for(int i = 0; i < int(m_GameObjects.size()); i++)
        delete m_GameObjects[i];
    m_GameObjects.clear();

    // delete level
    for(int i = 0; i < int(m_Levels.size()); i++)
        delete m_Levels[i];
    m_Levels.clear();

    // init player
    m_Player = new Player();
    //if(!registerGameOBJ(m_Player)) {std::cout << "Error registering player game object.\n"; exit(2);}

    // clear any existing particles
    m_ParticleManager->clear();

    // create levels
    m_Levels.push_back(new Level(32,32));
    m_CurrentLevel = m_Levels.back();

    // start main loop
    mainLoop();
}

void Blame::mainLoop()
{
    bool quit = false;

    /*
    ParticleEmitter p1(m_ParticleManager, PEMIT_CUSTOM, sf::Vector2f( 400, 300) );
    p1.m_custom_min_life = 500;
    p1.m_custom_max_life = 1000;
    //p1.m_custom_color = sf::Color(236,191,44,255);
    p1.m_custom_accel = sf::Vector2f(0, 0.05);
    */

    // test player position
    m_Player->m_Position = sf::Vector2f(100,100);

    // zoom in view
    m_View.zoom(0.5);

    sf::Clock frameclock;

    // main game loop
    while(!quit)
    {
        // clear screen
        m_Screen->clear();

        // set view
        //m_Screen->setView(m_View);


        // event que
        sf::Event event;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) m_Player->m_Drive = 1;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) m_Player->m_Drive = -1;
        else m_Player->m_Drive = 0;


        // handle all input events
        while(m_Screen->pollEvent(event))
        {
            // if window closed
            if(event.type == sf::Event::Closed) quit = true;
            // if a key was pressed
            else if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::Escape) quit = true;
                else if(event.key.code == sf::Keyboard::F1) m_DebugMode = !m_DebugMode;
            }
            // if mouse button pressed
            else if(event.type == sf::Event::MouseButtonPressed)
            {
                // if left mouse button pressed
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    // particle test
                    //p1.createParticle(sf::Vector2f(0,0), sf::Vector2f(1,-0.7));
                    m_Player->shoot();
                    std::cout << "test\n";
                }
            }
        }






        // update stuff
        m_ParticleManager->update();
        for(int i = int(m_GameObjects.size()-1); i >= 0; i--)
        {
            m_GameObjects[i]->update();
        }

        // update view
        m_View.setCenter(m_Player->m_Position);
        m_Screen->setView(m_View);

        // draw stuff
        drawLevel();

        // draw all game objects
        for(int i = int(m_GameObjects.size()-1); i >= 0; i--)
        {
            m_GameObjects[i]->draw(m_Screen);
        }
        // draw particles
        m_ParticleManager->draw(m_Screen);

        // draw rects (debug)
        if(m_DebugMode)
        {
            for(int i = int(m_GameObjects.size()-1); i >= 0; i--)
            {
                drawRect( m_GameObjects[i]->m_BoundingBox);
            }
        }

        // draw hud
        m_Screen->setView( m_Screen->getDefaultView());
        if(m_DebugMode) m_Screen->draw(*dbg_txt);

        // render screen
        m_Screen->display();

        m_DeltaTime = frameclock.getElapsedTime().asMilliseconds();
        frameclock.restart();

    }

}

void Blame::drawLevel()
{
    sf::Vector2i dims = m_CurrentLevel->getDims();

    for(int i = 0; i < dims.y; i++)
    {
        for(int n = 0; n < dims.x; n++)
        {
            Tile *ttile = m_CurrentLevel->getTile(n,i);

            // if tile id is not 0 (empty), draw tile
            if(ttile->id != 0)
            {
                m_Screen->draw( *m_CurrentLevel->getTile(n,i)->sprite );
                if(m_DebugMode)
                {
                    drawRect( m_CurrentLevel->getTile(n,i)->boundingbox);
                }
            }

        }
    }
}

void Blame::drawRect(sf::FloatRect trect)
{
    sf::RectangleShape rshape(sf::Vector2f(trect.width-2, trect.height-2));
    rshape.setFillColor(sf::Color::Transparent);
    rshape.setOutlineThickness(1.f);
    rshape.setOutlineColor(sf::Color::Red);
    rshape.setPosition(sf::Vector2f(trect.left+1, trect.top+1) );

    m_Screen->draw(rshape);

}

Tile *Blame::getMapCollision(GameOBJ *tobj)
{
    std::vector< sf::Vector2i > col;

    if(!tobj)
    {
        std::cout << "Error in getMapCollision, target object is null!\n";
        return NULL;
    }

    sf::Vector2i p1( int(floor(tobj->m_BoundingBox.left/TILE_SIZE)),
                     int(floor(tobj->m_BoundingBox.top/TILE_SIZE)) );
    sf::Vector2i p2( int(floor( (tobj->m_BoundingBox.left + tobj->m_BoundingBox.width)/TILE_SIZE ) ),
                     int(floor( (tobj->m_BoundingBox.top + tobj->m_BoundingBox.height)/TILE_SIZE) ) );


    for(int i = p1.y; i <= p2.y; i++)
    {
        for(int n = p1.x; n <= p2.x; n++)
        {
            Tile *ttile = m_CurrentLevel->getTile(n,i);

            if(ttile)
            {
                if(ttile->blocked)
                {
                    std::cout << "Colliding!\n";
                    return ttile;
                }
            }

        }
    }

    return NULL;
}
