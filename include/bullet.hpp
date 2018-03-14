#ifndef CLASS_BULLET
#define CLASS_BULLET

#include "gameobj.hpp"
#include "spritesheet.hpp"

class Bullet: public GameOBJ
{
protected:

    static SpriteSheet *m_BulletSS;

    int m_MaxAge;


public:
    Bullet(sf::Vector2f tpos, sf::Vector2f tdir);
    virtual ~Bullet();

    void update();
    void draw(sf::RenderTarget *tscreen);

    float m_BulletSpeed;

};

class Bullet_Drone:public Bullet
{
private:

public:
    Bullet_Drone(sf::Vector2f tpos, sf::Vector2f tdir);
    ~Bullet_Drone();
};


#endif // CLASS_BULLET
