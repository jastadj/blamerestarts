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
    m_UISS = NULL;

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
    const int tilesw = 4;
    const int tilesh = 1;
    m_TilesSS = new SpriteSheet(".\\data\\art\\tiles.png", tilesw, tilesh);

    // init ui sprite sheet
    m_UISS = new SpriteSheet(".\\data\\art\\uigfx.png", 3, 1);
    m_UI_Health_Sprites.push_back( m_UISS->createSprite(0));
    m_UI_Health_Sprites.push_back( m_UISS->createSprite(1));
    m_UI_Repair_Sprite = m_UISS->createSprite(2);


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

    if(tilenum >0 && tilenum <= m_TilesSS->getCount())
    {
        newtile->sprite = m_TilesSS->createSprite(tilenum-1);
        newtile->id = tilenum;

        // spikes
        if(tilenum == 2 || tilenum == 3)
        {
            newtile->tiledamage = 1;
            newtile->blocked = false;
        }
        // lava
        else if(tilenum == 4)
        {
            newtile->tiledamage = 2;
            newtile->blocked = false;
        }
    }
    else
    {
        newtile->sprite = new sf::Sprite();
        newtile->blocked = false;
        newtile->id = 0;

        if(tilenum != 0) std::cout << "Error creating tile, tilenum " << tilenum << " is not valid.\n";
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
    m_Player = new Player( sf::Vector2f(100,32 * 29) );
    //if(!registerGameOBJ(m_Player)) {std::cout << "Error registering player game object.\n"; exit(2);}

    // clear any existing particles
    m_ParticleManager->clear();

    // create levels
    m_Levels.push_back(new Level(".\\data\\levels\\level_01.txt"));
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
                else if(event.key.code == sf::Keyboard::Space) m_Player->jump();
                else if(event.key.code == sf::Keyboard::R) m_Player->doRepair();
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
        drawHUD();
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

void Blame::drawHUD()
{
    sf::Vector2f healthoffset;
    sf::Vector2f repairoffset;

    int pmaxh = m_Player->getMaxHealth();
    int pcurh = m_Player->getHealth();

    int pcurr = m_Player->getRepairs();

    for(int i = 0; i < pmaxh; i++)
    {
        if(i < pcurh)
        {
            m_UI_Health_Sprites[1]->setPosition( sf::Vector2f(i*32, 0) + healthoffset);
            m_Screen->draw( *m_UI_Health_Sprites[1]);
        }
        else
        {
            m_UI_Health_Sprites[0]->setPosition( sf::Vector2f(i*32, 0) + healthoffset);
            m_Screen->draw( *m_UI_Health_Sprites[0]);
        }
    }

    repairoffset = healthoffset + sf::Vector2f(pmaxh*32, 0) + sf::Vector2f(32,0);

    for(int i = 0; i < pcurr; i++)
    {
        if(i < pcurr)
        {
            m_UI_Repair_Sprite->setPosition( sf::Vector2f(i*32,0) + repairoffset);
            m_Screen->draw( *m_UI_Repair_Sprite);
        }
    }





}

Tile *Blame::getMapCollision(GameOBJ *tobj)
{
    if(!tobj)
    {
        std::cout << "Error in getMapCollision, target object is null!\n";
        return NULL;
    }

    // map array "tile number" range for top left and bottom right
    // these are used to scan each occupied tile by the game object
    sf::Vector2i p1( int(floor(tobj->m_BoundingBox.left/TILE_SIZE)),
                     int(floor(tobj->m_BoundingBox.top/TILE_SIZE)) );
    sf::Vector2i p2( int(floor( (tobj->m_BoundingBox.left + tobj->m_BoundingBox.width)/TILE_SIZE ) ),
                     int(floor( (tobj->m_BoundingBox.top + tobj->m_BoundingBox.height)/TILE_SIZE) ) );


    // go through each occupied tile by the game obj and determine if its a "blocked" tile
    for(int i = p1.y; i <= p2.y; i++)
    {
        for(int n = p1.x; n <= p2.x; n++)
        {
            // the one of the occupied tiles from the current level
            Tile *ttile = m_CurrentLevel->getTile(n,i);

            // successfully retrieved map tile
            if(ttile)
            {
                // tile is blocked (can't pass through)
                if(ttile->blocked)
                {
                    // return this tile
                    return ttile;
                }
                // else if player is touching a damaging tile
                else if(ttile->tiledamage > 0 && tobj == m_Player && ttile->boundingbox.intersects(m_Player->m_BoundingBox) )
                {
                    m_Player->takeDamage( ttile->tiledamage );
                }
            }

        }
    }

    // all tiles non blocked
    return NULL;
}
