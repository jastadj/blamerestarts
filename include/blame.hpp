#ifndef CLASS_BLAME
#define CLASS_BLAME

#include <cstdlib>
#include <iostream>

#include <SFML\Graphics.hpp>
#include "spritesheet.hpp"

class Blame
{
private:

    // singleton
    Blame();
    ~Blame();
    static Blame *m_Instance;

    bool init();

    // assets
    SpriteSheet *sstest;

    // render window
    sf::RenderWindow *m_Screen;

    // main loop
    void mainLoop();

public:
    // get singleton
    static Blame *getInstance()
    {
        if(!m_Instance)
        {
            m_Instance = new Blame();
        }

        return m_Instance;
    }
};
#endif // CLASS_BLAME
