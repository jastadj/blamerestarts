#include "blame.hpp"

Blame *Blame::m_Instance = NULL;

Blame::Blame()
{
    m_Screen = NULL;
    m_ParticleManager = NULL;

    std::cout << "Blame" << std::endl;

    init();

}

Blame::~Blame()
{

}

bool Blame::init()
{
    // init render window
    m_Screen = new sf::RenderWindow(sf::VideoMode(800,600,32), "Blame Restarts");

    if(!m_Screen) {std::cout << "Error creating render window.\n"; exit(1);}

    m_Screen->setFramerateLimit(60); // 60fps limit

    // init assets
    sstest = new SpriteSheet(".\\Data\\Art\\testimage.jpg");
    if(!sstest->initialized()) {std::cout << "Error initializing assets.\n"; exit(1);}

    m_ParticleManager = new ParticleManager;

    // start main loop
    mainLoop();
}


void Blame::mainLoop()
{
    bool quit = false;

    sf::Sprite *mysprite = sstest->createSprite();

        ParticleEmitter p1(m_ParticleManager, PEMIT_CUSTOM, sf::Vector2f( 400, 300) );
        p1.m_custom_min_life = 500;
        p1.m_custom_max_life = 1000;
        //p1.m_custom_color = sf::Color(236,191,44,255);
        p1.m_custom_accel = sf::Vector2f(0, 0.05);

    // main game loop
    while(!quit)
    {
        // clear screen
        m_Screen->clear();

        // event que
        sf::Event event;

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
                    p1.createParticle(sf::Vector2f(0,0), sf::Vector2f(1,-0.7));
                }
            }
        }






        // update stuff
        m_ParticleManager->update();

        // draw stuff
        m_Screen->draw(*mysprite);
        m_ParticleManager->draw(m_Screen);

        // render screen
        m_Screen->display();

    }

}
