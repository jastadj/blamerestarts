#ifndef CLASS_BULLET
#define CLASS_BULLET

#include "gameobj.hpp"
#include "spritesheet.hpp"

class Bullet: public GameOBJ
{
private:

    static SpriteSheet *m_BulletSS;

    sf::Clock m_TimeAlive;


public:
    Bullet();
    ~Bullet();

    void update();
    void draw(sf::RenderTarget *tscreen);

    float m_BulletSpeed;

};
#endif // CLASS_BULLET
