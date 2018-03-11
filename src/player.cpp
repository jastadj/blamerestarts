#include "player.hpp"
#include "blame.hpp"
#include "bullet.hpp"

#include <sstream> // debug text

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
    m_TerminalVel = sf::Vector2f(3, 3);
    m_VelCutoff = sf::Vector2f(0.3, 0.3);
    m_MaxAccel = sf::Vector2f(1.0, 1.0);
    m_Friction = 0.05;
    m_Gravity = 0.3;

    // randomize starting wheel rotation
    m_LeftWheelRot = rand()%360;
    m_RightWheelRot = rand()%360;

    // init bounding box
    m_BoundingBox.width = 44;
    m_BoundingBox.height = 32;
    m_BoundingBoxOffset = sf::Vector2f(-24, -18);


}

Player::~Player()
{

}

void Player::shoot()
{
    // if turret is not in place, do not fire
    if(m_TurretPosition > 0.f && m_TurretPosition < 1.f) return;

    Bullet *newbullet;

    if(m_TurretPosition == 1) newbullet = new Bullet(m_BarrelExit, sf::Vector2f( 1, 0 ) );
    else newbullet = new Bullet(m_BarrelExit, sf::Vector2f( -1, 0 ) );

}

void Player::update()
{
    int32_t dt = m_BlameCallback->getDeltaTime();
    std::vector< sf::Vector2i > mapcol;

    // if driving, add acceleration
    if(m_Drive != 0)
    {
        m_LeftWheelRot += m_DriveSpeed * dt * m_Drive;
        m_RightWheelRot += m_DriveSpeed * dt * m_Drive;

        // calc acceleration
        m_Accel.x = 0.66 * m_DriveSpeed * dt * m_Drive;

        // if driving left
        if(m_Drive < 0)
        {
            if(m_SpriteState != FACING_LEFT) m_SpriteState = TURNING_LEFT;
        }
        else if(m_SpriteState != FACING_RIGHT) m_SpriteState = TURNING_RIGHT;
    }
    else m_Accel.x = 0;

    // add gravitational acceleration
    m_Accel.y += m_Gravity;

    // clip acceleration
    if(m_Accel.x > m_MaxAccel.x) m_Accel.x = m_MaxAccel.x;
    else if(m_Accel.x < -m_MaxAccel.x) m_Accel.x = -m_MaxAccel.x;

    // update velocity from acceleration
    m_Vel += m_Accel;

    // if not applying drive, add deceleration / friction
    if(m_Drive == 0)
    {
        // if moving
        if(m_Vel.x != 0.f)
        {
            // decelerate velocity
            m_Vel.x *= m_Friction * dt;

            // if velocity is close enough to 0, kill it
            if(m_Vel.x > 0.f)
            {
                if(m_Vel.x < m_VelCutoff.x) m_Vel.x = 0;
            }
            else if(m_Vel.x > -m_VelCutoff.x) m_Vel.x = 0;
        }

    }

    // clip velocity at terminal vel
    if(m_Vel.x > m_TerminalVel.x) m_Vel.x = m_TerminalVel.x;
    else if(m_Vel.x < -m_TerminalVel.x) m_Vel.x = -m_TerminalVel.x;
    if(m_Vel.y > m_TerminalVel.y) m_Vel.y = m_TerminalVel.y;
    else if(m_Vel.y < -m_TerminalVel.y) m_Vel.y = -m_TerminalVel.y;

    // update position from velocity
    m_Position += m_Vel;

    // update bounding box position
    m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;
    m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;

    // check map collision
    mapcol = m_BlameCallback->getMapCollision(this);
    if(!mapcol.empty())
    {

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
    }

    // debug info
    std::stringstream dbgss;
    dbgss << "pos:" << m_Position.x << "," << m_Position.y << ",\nvel:" << m_Vel.x << "," << m_Vel.y << "\n";
    dbgss << "accel:" << m_Accel.x << "," << m_Accel.y << std::endl;
    m_BlameCallback->dbg_txt->setString(dbgss.str());

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
