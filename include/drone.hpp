#ifndef CLASS_DRONE
#define CLASS_DRONE

#include "enemy.hpp"

#include "spritesheet.hpp"

class Drone:public Enemy
{
private:

    static SpriteSheet *m_DroneSS;

    sf::Vector2f m_Vel;
    float m_Speed;
    float m_BulletSpeed;
    bool m_MovingRight;

    int m_FireCooldown;
    sf::Clock m_FireClock;
    float m_FireRange;
    void shoot();
    float getRangeToPlayer();

public:
    Drone(sf::Vector2f spos);
    ~Drone();

    void update();
    void draw(sf::RenderTarget *tscreen);

};
#endif // CLASS_DRONE
