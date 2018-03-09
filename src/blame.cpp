#include "blame.hpp"

Blame *Blame::m_Instance = NULL;

Blame::Blame()
{
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

    // init assets
    sstest = new SpriteSheet(".\\Data\\Art\\testimage.jpg");
    if(!sstest->initialized()) {std::cout << "Error initializing assets.\n"; exit(1);}

    // start main loop
    mainLoop();
}


void Blame::mainLoop()
{
    bool quit = false;

    sf::Sprite *mysprite = sstest->createSprite();

    // main game loop
    while(!quit)
    {
        m_Screen->clear();

        sf::Event event;

        // handle all input events
        while(m_Screen->pollEvent(event))
        {
            if(event.type == sf::Event::Closed) quit = true;
            else if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::Escape) quit = true;
            }
        }

        // update stuff

        // draw stuff
        m_Screen->draw(*mysprite);

        // render screen
        m_Screen->display();

    }

}
