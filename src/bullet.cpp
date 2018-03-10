#include "bullet.hpp"
#include "blame.hpp"

SpriteSheet *Bullet::m_BulletSS = new SpriteSheet(".\\data\\art\\bullet.png");

Bullet::Bullet()
{
    // create sprite and center origin
    m_Sprites.push_back(m_BulletSS->createSprite());
    sf::FloatRect bsize = m_Sprites.back()->getLocalBounds();

    m_Sprites.back()->setOrigin(bsize.width/2, bsize.height/2);

    m_BulletSpeed = 0.2;

}

Bullet::~Bullet()
{

}

void Bullet::update()
{
    if(m_TimeAlive.getElapsedTime().asMilliseconds() > 500)
    {
        m_BlameCallback->destroyGameOBJ(this);
        return;
    }

    int32_t dt = m_BlameCallback->getDeltaTime();

    m_Position += sf::Vector2f(m_Vel.x * dt, m_Vel.y * dt);

    m_Sprites[0]->setPosition( m_Position);
}

void Bullet::draw(sf::RenderTarget *tscreen)
{
    tscreen->draw(*m_Sprites[0]);
}
