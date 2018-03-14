#ifndef CLASS_ENEMY
#define CLASS_ENEMY

#include "gameobj.hpp"

class Enemy:public GameOBJ
{
protected:

public:
    Enemy();
    ~Enemy();

    virtual void update() = 0;
    virtual void draw(sf::RenderTarget *tscreen)=0;

};
#endif // CLASS_ENEMY
