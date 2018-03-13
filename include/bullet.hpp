#ifndef CLASS_BULLET
#define CLASS_BULLET

#include "gameobj.hpp"
#include "spritesheet.hpp"

class Bullet: public GameOBJ
{
private:

    static SpriteSheet *m_BulletSS;

    int m_MaxAge;


public:
    Bullet(sf::Vector2f tpos, sf::Vector2f tdir);
    ~Bullet();

    void update();
    void draw(sf::RenderTarget *tscreen);

    float m_BulletSpeed;

};
#endif // CLASS_BULLET
