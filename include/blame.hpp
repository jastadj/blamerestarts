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
#include "level.hpp"

class Blame
{
private:

    // singleton
    Blame();
    ~Blame();
    static Blame *m_Instance;

    bool init();
    bool m_DebugMode;

    // assets
    SpriteSheet *m_TilesSS;
    ParticleManager *m_ParticleManager;
    sf::Font m_Font;
    SpriteSheet *m_UISS;
    std::vector< sf::Sprite* > m_UI_Health_Sprites;
    sf::Sprite *m_UI_Repair_Sprite;


    // game data
    Player *m_Player;
    time_t m_Seed;
    std::vector<GameOBJ*> m_GameObjects;
    std::vector<Level*> m_Levels;
    Level *m_CurrentLevel;


    // render window
    unsigned int m_ScreenWidth;
    unsigned int m_ScreenHeight;
    sf::RenderWindow *m_Screen;
    sf::View m_View;
    double m_DeltaTime;

    // main loop
    void newGame();
    void mainLoop();

    // draw
    void drawLevel();
    void drawRect(sf::FloatRect trect);
    void drawHUD();

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

    // tile factory
    Tile *createTile(int tilenum);

    // all game objects must be registered and destroyed through these methods
    bool registerGameOBJ(GameOBJ *tobj);
    bool destroyGameOBJ(GameOBJ *tobj);

    Tile *getMapCollision(GameOBJ *tobj);
    std::vector<GameOBJ*> getObjectCollisions(GameOBJ *tobj);

    ParticleManager *getParticleManager() { return m_ParticleManager;}
    // if in debug mode, draw this text to the screen
    sf::Text *dbg_txt;
};
#endif // CLASS_BLAME
