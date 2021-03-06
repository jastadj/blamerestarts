#include "drone.hpp"
#include "blame.hpp"
#include "bullet.hpp"

#include "tools.hpp"

SpriteSheet *Drone::m_DroneSS = new SpriteSheet(".\\data\\art\\drone.png",1,1);

Drone::Drone(sf::Vector2f spos)
{
    // init starting position
    m_Position = spos;

    m_Sprites.push_back(m_DroneSS->createSprite(0));
    //m_Sprites[0]->setOrigin(sf::Vector2f(16,16));
    m_BoundingBox.width = 20;
    m_BoundingBox.height = 20;
    m_BoundingBoxOffset.x = 6;
    m_BoundingBoxOffset.y = 6;

    m_Speed = 0.08;
    m_BulletSpeed = 1.0;
    m_MovingRight = rand()%2;
    m_FireCooldown = 500;
    m_FireRange = 1000;

    m_CurHealth = 4;
}

Drone::~Drone()
{

}

float Drone::getRangeToPlayer()
{
    // get vector to player
    sf::Vector2f vtop(m_BlameCallback->getPlayerPosition() - m_Position);

    // return magnitude of vector (range)
    return getMagnitude(vtop);
}

void Drone::shoot()
{
    // get vector to player
    sf::Vector2f tvel = normalizeVector(m_BlameCallback->getPlayerPosition() - m_Position);
    tvel = tvel * m_BulletSpeed;

    // fire at player
    Bullet_Drone *newbullet = new Bullet_Drone(m_Position + sf::Vector2f(16,16), tvel);


}


void Drone::update()
{
    int32_t dt = m_BlameCallback->getDeltaTime();
    Tile *coltile = NULL;
    std::vector<GameOBJ*> ocol;

    // update velocity
    m_Vel.x = m_Speed * dt * ((m_MovingRight)?1:-1);

    // update x position from velocity
    m_Position.x += m_Vel.x;
    m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;
    coltile = m_BlameCallback->getMapCollision(this);
    if(coltile)
    {
        // if moving rightward
        if(m_Vel.x > 0)
        {
            // push out to just touch the surface
            // note : for some reason, the exact values were still detecting ocllision when pushing away
            // so adding a 0.1 offset seems to have corrected this
            m_Position.x -= (m_BoundingBox.left + m_BoundingBox.width) - coltile->boundingbox.left +0.1;
            m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;

            // reverse direction
            m_MovingRight = !m_MovingRight;
        }
        // moving leftward
        else if(m_Vel.x < 0)
        {
            // push out to just touch the surface
            m_Position.x += (coltile->boundingbox.left + coltile->boundingbox.width) - m_BoundingBox.left;
            m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;

            // reverse direction
            m_MovingRight = !m_MovingRight;

        }

    }

    // check object collisions
    ocol = m_BlameCallback->getObjectCollisions(this);

    for(int i = 0; i < int(ocol.size()); i++)
    {
        Bullet *bul = dynamic_cast<Bullet*>(ocol[i]);
        Bullet_Drone *bul_drone = dynamic_cast<Bullet_Drone*>(ocol[i]);

        // ignore drone bullets
        if(bul_drone) continue;
        // collided with bullet
        else if(bul)
        {
            // take damage from bullet
            m_CurHealth--;

            // destroy bullet
            m_BlameCallback->destroyGameOBJ(bul);

            // destroy this drone if health <= 0
            if(m_CurHealth <= 0)
            {
                m_BlameCallback->destroyGameOBJ(this);
                return;
            }
        }
    }

    // update sprites and bounding box
    m_Sprites[0]->setPosition(m_Position);
    m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;
    m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;

    // if able to fire, fire at player
    if(m_FireClock.getElapsedTime().asMilliseconds() > double(m_FireCooldown) )
    {
        // if player is in range, shoot
        if(getRangeToPlayer() < m_FireRange )
        {
            shoot();
            m_FireClock.restart();
        }

    }
}

void Drone::draw(sf::RenderTarget *tscreen)
{
    tscreen->draw(*m_Sprites[0]);
}
