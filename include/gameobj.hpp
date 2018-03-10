#ifndef CLASS_GAMEOBJ
#define CLASS_GAMEOBJ

#include <SFML\Graphics.hpp>
#include "spritesheet.hpp"

// base class for all game objects

class GameOBJ
{
protected:

    SpriteSheet *m_SpriteSheet;

public:
    GameOBJ();
    virtual ~GameOBJ();

    virtual void update();
    void draw(sf::RenderTarget *tscreen);
};
#endif // CLASS_GAMEOBJ
