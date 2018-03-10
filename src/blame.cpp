#include "blame.hpp"

Blame *Blame::m_Instance = NULL;

Blame::Blame()
{
    std::cout << "Blame" << std::endl;

    m_Screen = NULL;
    m_ParticleManager = NULL;
    m_Player = NULL;

    m_ScreenWidth = 800;
    m_ScreenHeight = 600;

    m_DeltaTime = 0;

    m_Instance = this;

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

    m_Screen->setFramerateLimit(60); // 60fps limit


    // create particle manager
    m_ParticleManager = new ParticleManager;

    // start new game
    newGame();
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

    // init player
    m_Player = new Player();
    if(!registerGameOBJ(m_Player)) {std::cout << "Error registering player game object.\n"; exit(2);}

    // clear any existing particles
    m_ParticleManager->clear();

    // start main loop
    mainLoop();
}

void Blame::mainLoop()
{
    bool quit = false;

    //sf::Sprite *mysprite = sstest->createSprite();

    /*
    ParticleEmitter p1(m_ParticleManager, PEMIT_CUSTOM, sf::Vector2f( 400, 300) );
    p1.m_custom_min_life = 500;
    p1.m_custom_max_life = 1000;
    //p1.m_custom_color = sf::Color(236,191,44,255);
    p1.m_custom_accel = sf::Vector2f(0, 0.05);
    */

    // test player position
    m_Player->m_Position = sf::Vector2f(32,32);

    // zoom in view
    m_View.zoom(0.25);

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
        // testing sprite
        sf::RectangleShape myshape(sf::Vector2f(64,64));
        m_Screen->draw(myshape);
        for(int i = int(m_GameObjects.size()-1); i >= 0; i--)
        {
            m_GameObjects[i]->draw(m_Screen);
        }
        m_ParticleManager->draw(m_Screen);


        // render screen
        m_Screen->display();

        m_DeltaTime = frameclock.getElapsedTime().asMilliseconds();
        frameclock.restart();

    }

}
