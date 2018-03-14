#include "bullet.hpp"
#include "blame.hpp"

SpriteSheet *Bullet::m_BulletSS = new SpriteSheet(".\\data\\art\\bullet.png");

Bullet::Bullet(sf::Vector2f tpos, sf::Vector2f tdir)
{
    // create sprite and center origin
    m_Sprites.push_back(m_BulletSS->createSprite());
    sf::FloatRect bsize = m_Sprites.back()->getLocalBounds();

    m_Sprites.back()->setOrigin(bsize.width/2, bsize.height/2);

    m_BulletSpeed = 0.3;
    m_MaxAge = 600;

    // initial properties
    m_Vel.x = tdir.x * m_BulletSpeed;
    m_Vel.y = tdir.y * m_BulletSpeed;
    m_Position = tpos;

    m_Sprites.back()->setPosition(m_Position);

    m_BoundingBox = bsize;
    m_BoundingBoxOffset = sf::Vector2f(-m_BoundingBox.width/2, -m_BoundingBox.height/2);

    // update bounding box position
    m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;
    m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;

}

Bullet::~Bullet()
{

}

void Bullet::update()
{
    std::vector< sf::Vector2i > mapcol;

    // if alive too long, destroy self
    if(m_TimeAlive.getElapsedTime().asMilliseconds() > m_MaxAge)
    {
        m_BlameCallback->destroyGameOBJ(this);
        return;
    }

    // update position from velocity
    int32_t dt = m_BlameCallback->getDeltaTime();
    m_Position += sf::Vector2f(m_Vel.x * dt, m_Vel.y * dt);

    // check map collisions
    if(m_BlameCallback->getMapCollision(this))
    {
        /*
        // debug
        for(int i = 0; i < mapcol.size(); i++)
        {
            std::cout << "col tile : " << mapcol[i].x << "," << mapcol[i].y << std::endl;
        }

        // if collided while moving left
        if(m_Vel.x < 0)
        {
            for(int i = 0; i < int(mapcol.size()); i++)
            {
                if( mapcol[i].x * TILE_SIZE + TILE_SIZE > m_BoundingBox.left)
                {
                    m_Position.x += (mapcol[i].x * TILE_SIZE + TILE_SIZE) - m_BoundingBox.left;
                    m_Vel.x = 0;
                    m_Accel.x = 0;
                    break;
                }
            }
        }
        // if collided while moving right
        else if(m_Vel.x > 0)
        {
            for(int i = 0; i < int(mapcol.size()); i++)
            {
                if( mapcol[i].x * TILE_SIZE < m_BoundingBox.left + m_BoundingBox.width)
                {
                    m_Position.x -= m_BoundingBox.left + m_BoundingBox.width - (mapcol[i].x * TILE_SIZE);
                    m_Vel.x = 0;
                    m_Accel.x = 0;
                    break;
                }
            }
        }

        // re-update bounding box
        m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;
        m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;
        */

        m_BlameCallback->destroyGameOBJ(this);
        return;
    }

    // update sprite position
    m_Sprites[0]->setPosition( m_Position);

    // update bounding box position
    m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;
    m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;
}

void Bullet::draw(sf::RenderTarget *tscreen)
{
    tscreen->draw(*m_Sprites[0]);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
Bullet_Drone::Bullet_Drone(sf::Vector2f tpos, sf::Vector2f tdir):Bullet(tpos,tdir)
{

}

Bullet_Drone::~Bullet_Drone()
{

}
