#ifndef CLASS_DRONE
#define CLASS_DRONE

#include "enemy.hpp"

class Drone:public Enemy
{
private:

public:
    Drone();
    ~Drone();

    void update();
    void draw(sf::RenderTarget *tscreen);

};
#endif // CLASS_DRONE
