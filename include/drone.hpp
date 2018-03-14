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
    bool m_MovingRight;

public:
    Drone(sf::Vector2f spos);
    ~Drone();

    void update();
    void draw(sf::RenderTarget *tscreen);

};
#endif // CLASS_DRONE
