#ifndef CLASS_BLAME
#define CLASS_BLAME

#include <cstdlib>
#include <iostream>
#include <math.h>
#include <time.h>

#include <SFML\Graphics.hpp>
#include "spritesheet.hpp"
#include "particle.hpp"
#include "player.hpp"

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
    ParticleManager *m_ParticleManager;


    // game data
    Player *m_Player;
    time_t m_Seed;
    std::vector<GameOBJ*> m_GameObjects;


    // render window
    unsigned int m_ScreenWidth;
    unsigned int m_ScreenHeight;
    sf::RenderWindow *m_Screen;
    sf::View m_View;
    double m_DeltaTime;

    // main loop
    void newGame();
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

    time_t getSeed() { return m_Seed;}
    int32_t getDeltaTime() { return m_DeltaTime;}

    bool registerGameOBJ(GameOBJ *tobj);
    bool destroyGameOBJ(GameOBJ *tobj);
};
#endif // CLASS_BLAME
