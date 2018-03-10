#include "player.hpp"
#include "blame.hpp"
#include "bullet.hpp"

SpriteSheet *Player::m_ChassisSS = new SpriteSheet(".\\data\\art\\chassis.png");
SpriteSheet *Player::m_TurretSS = new SpriteSheet(".\\data\\art\\turret.png", 1, 3);
SpriteSheet *Player::m_WheelSS = new SpriteSheet(".\\data\\art\\wheel.png");

Player::Player()
{
    // create sprites
    if(m_ChassisSS->initialized())
    {
        // sprite 0
        m_Sprites.push_back(m_ChassisSS->createSprite());
    }

    if(m_TurretSS->initialized())
    {
        // turret right - sprite 1
        m_Sprites.push_back(m_TurretSS->createSprite(0));
        // turret forward - sprite 2
        m_Sprites.push_back(m_TurretSS->createSprite(1));
        // turret left - sprite 3
        m_Sprites.push_back(m_TurretSS->createSprite(2));
    }

    if(m_WheelSS->initialized())
    {
        // left wheel sprite 4
        m_Sprites.push_back(m_WheelSS->createSprite());
        // center left wheel origin
        m_Sprites.back()->setOrigin(sf::Vector2f(7.5,7.5));
        // right wheel sprite 5
        m_Sprites.push_back(m_WheelSS->createSprite());
        // center right wheel origin
        m_Sprites.back()->setOrigin(sf::Vector2f(7.5,7.5));
    }

    m_SpriteState = FACING_RIGHT;

    // init properties
    m_DriveSpeed = 0.3f;
    m_TurretPosition = 1.f;
    m_TurretSpeed = 0.01f;
    m_Drive = 0; // -1 left, 1 right, 0 stopped

    // randomize starting wheel rotation
    m_LeftWheelRot = rand()%360;
    m_RightWheelRot = rand()%360;

}

Player::~Player()
{

}

void Player::shoot()
{
    // if turret is not in place, do not fire
    if(m_TurretPosition > 0.f && m_TurretPosition < 1.f) return;

    Bullet *newbullet = new Bullet();

    if(m_TurretPosition == 1) newbullet->m_Vel = sf::Vector2f( newbullet->m_BulletSpeed, 0 );
    else newbullet->m_Vel = sf::Vector2f(-newbullet->m_BulletSpeed, 0);
    newbullet->m_Position = m_BarrelExit;

    if(!m_BlameCallback->registerGameOBJ(newbullet))
    {
        std::cout << "Error in player shoot, unable to register bullet.\n";
        delete newbullet;
    }

}

void Player::update()
{
    int32_t dt = m_BlameCallback->getDeltaTime();

    // driving?
    if(m_Drive != 0)
    {
        m_LeftWheelRot += m_DriveSpeed * dt * m_Drive;
        m_RightWheelRot += m_DriveSpeed * dt * m_Drive;

        // calc driving velocity
        m_Vel = sf::Vector2f(0.66 * m_DriveSpeed * dt * m_Drive, 0);

        // if driving left
        if(m_Drive < 0)
        {
            if(m_SpriteState != FACING_LEFT) m_SpriteState = TURNING_LEFT;
        }
        else if(m_SpriteState != FACING_RIGHT) m_SpriteState = TURNING_RIGHT;
    }
    else m_Vel = sf::Vector2f(0,0);

    // update position
    m_Position += m_Vel;

    // update turret turning
    if(m_SpriteState == TURNING_RIGHT)
    {
        if(m_TurretPosition >= 1.0)
        {
            m_SpriteState = FACING_RIGHT;
            m_TurretPosition = 1.0f;
        }
        else m_TurretPosition += dt * m_TurretSpeed;
    }
    else if(m_SpriteState == TURNING_LEFT)
    {
        if(m_TurretPosition <= 0.0)
        {
            m_SpriteState = FACING_LEFT;
            m_TurretPosition = 0.0f;
        }
        else m_TurretPosition -= dt * m_TurretSpeed;
    }

    // update sprite offsets relative to player position and barrel exit points
    if(m_SpriteState == FACING_RIGHT)
    {
        m_Sprites[TURRET_RIGHT]->setPosition( m_Position - sf::Vector2f(32, 44) );
        m_BarrelExit = m_Position - sf::Vector2f(-32,12);
    }
    else if(m_SpriteState == FACING_LEFT)
    {
        m_Sprites[TURRET_LEFT]->setPosition(m_Position - sf::Vector2f(34, 44) );
        m_BarrelExit = m_Position - sf::Vector2f(32,12);
    }
    if(m_TurretPosition > 0 && m_TurretPosition < 1.f)
        m_Sprites[TURRET_FORWARD]->setPosition( m_Position - sf::Vector2f(32, 44) );

    m_Sprites[CHASSIS]->setPosition( m_Position - sf::Vector2f(10, 6) );
    m_Sprites[WHEEL_LEFT]->setPosition(m_Position - sf::Vector2f(18, -5));
    m_Sprites[WHEEL_RIGHT]->setPosition(m_Position - sf::Vector2f(-15, -5) );

    // set wheel rotation
    if(m_LeftWheelRot >=360.f) m_LeftWheelRot = m_LeftWheelRot - 360.f;
    else if(m_LeftWheelRot < 0) m_LeftWheelRot = m_LeftWheelRot + 360.f;
    if(m_RightWheelRot >= 360.f) m_RightWheelRot = m_RightWheelRot - 360.f;
    else if(m_RightWheelRot < 0) m_RightWheelRot = m_RightWheelRot + 360.f;

    m_Sprites[WHEEL_LEFT]->setRotation(m_LeftWheelRot);
    m_Sprites[WHEEL_RIGHT]->setRotation(m_RightWheelRot);

}

void Player::draw(sf::RenderTarget *tscreen)
{
    if(!tscreen) return;

    tscreen->draw(*m_Sprites[CHASSIS]);
    if(m_SpriteState == FACING_RIGHT) tscreen->draw(*m_Sprites[TURRET_RIGHT]);
    else if(m_SpriteState == FACING_LEFT) tscreen->draw(*m_Sprites[TURRET_LEFT]);
    else if(m_SpriteState == TURNING_LEFT || m_SpriteState == TURNING_RIGHT) tscreen->draw(*m_Sprites[TURRET_FORWARD]);
    tscreen->draw(*m_Sprites[WHEEL_LEFT]);
    tscreen->draw(*m_Sprites[WHEEL_RIGHT]);
}
