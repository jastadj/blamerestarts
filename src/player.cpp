#include "player.hpp"
#include "blame.hpp"
#include "bullet.hpp"

#include <sstream> // debug text

SpriteSheet *Player::m_ChassisSS = new SpriteSheet(".\\data\\art\\chassis.png", 3, 1);
SpriteSheet *Player::m_TurretSS = new SpriteSheet(".\\data\\art\\turret.png", 1, 5);
SpriteSheet *Player::m_WheelSS = new SpriteSheet(".\\data\\art\\wheel.png");

Player::Player(sf::Vector2f tpos)
{
    // create sprites
    if(m_ChassisSS->initialized())
    {
        // sprite 0
        m_Sprites.push_back(m_ChassisSS->createSprite(0));
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

    if(m_TurretSS->initialized())
    {
        // turret right - sprite 6
        m_Sprites.push_back(m_TurretSS->createSprite(3));
        // turret forward - sprite 7
        m_Sprites.push_back(m_TurretSS->createSprite(4));
    }

    // create sprites
    if(m_ChassisSS->initialized())
    {
        // sprite 8 - looking up right
        m_Sprites.push_back(m_ChassisSS->createSprite(1));

        // sprite 9 - looking up left
        m_Sprites.push_back(m_ChassisSS->createSprite(2));
    }

    m_SpriteState = FACING_RIGHT;

    // physics
    m_DriveSpeed = 0.4f;
    m_TurretPosition = 1.f;
    m_TurretSpeed = 0.01f;
    m_Drive = 0; // -1 left, 1 right, 0 stopped
    m_TerminalVel = sf::Vector2f(3, 5);
    m_VelCutoff = sf::Vector2f(0.3, 0.5);
    m_MaxAccel = sf::Vector2f(1.0, 1.0);
    m_Friction = 0.05;
    m_Gravity = 0.008;
    m_OnGround = false;
    m_JumpForce = 4.0f;
    m_LookInYAxis = 0;
    m_CrouchAmount = sf::Vector2f(0,-7);

    // health
    m_MaxHealth = 5;
    m_CurrentHealth = m_MaxHealth;
    m_DamageInvincibleTime_ms = 2000;
    m_DamageInvincibleBlink_ms = 200;
    m_TimeDamageTaken = -m_DamageInvincibleTime_ms - 1;

    // repair
    m_Repairs = 1;
    m_IsRepairing = false;
    m_RepairMaxTime = 1000;
    m_RepairStartTime = 0;

    // configure repair emitter
    m_Emitter_Repair = new ParticleEmitter(m_BlameCallback->getParticleManager(), PEMIT_CUSTOM);
    m_Emitter_Repair->m_custom_accel = sf::Vector2f(0,0);
    m_Emitter_Repair->m_custom_color = sf::Color(0,128,0,128);
    m_Emitter_Repair->m_custom_min_life = 500;
    m_Emitter_Repair->m_custom_max_life = 1500;

    // configure other repair emitter
    m_Emitter_Repair2 = new ParticleEmitter(m_BlameCallback->getParticleManager(), PEMIT_CUSTOM);
    m_Emitter_Repair2->m_custom_accel = m_Emitter_Repair->m_custom_accel;
    m_Emitter_Repair2->m_custom_color = sf::Color(0,200,0,128);
    m_Emitter_Repair2->m_custom_min_life = 500;
    m_Emitter_Repair2->m_custom_max_life = 1500;
    m_Emitter_Repair2->m_custom_texture_index = 1;

    // randomize starting wheel rotation
    m_LeftWheelRot = rand()%360;
    m_RightWheelRot = rand()%360;

    m_Position = tpos;

    // init bounding box
    m_BoundingBox.width = 44;
    m_BoundingBox.height = 32;
    m_BoundingBoxOffset = sf::Vector2f(-24, -18);
    m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;
    m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;

}

Player::~Player()
{

}

void Player::shoot()
{
    // if turret is not in place, do not fire
    if(m_TurretPosition > 0.f && m_TurretPosition < 1.f) return;

    Bullet *newbullet;

    // if turret is looking upwards
    if(m_LookInYAxis == 1) newbullet = new Bullet(m_BarrelExit, sf::Vector2f(0, -1));
    // if turret is facing to the right
    else if(m_TurretPosition == 1) newbullet = new Bullet(m_BarrelExit, sf::Vector2f( 1, 0 ) );
    // else if turret is facing to the left
    else newbullet = new Bullet(m_BarrelExit, sf::Vector2f( -1, 0 ) );

}

void Player::jump()
{
    if(m_OnGround)
    {
        //m_Accel.y -= m_JumpForce;
        m_Vel.y -= m_JumpForce;
    }
}

void Player::lookInY(int ydir)
{
    int oldlook = m_LookInYAxis;

    m_LookInYAxis = ydir;

    if(m_LookInYAxis < -1) m_LookInYAxis = -1;
    else if(m_LookInYAxis > 1) m_LookInYAxis = 1;

}

bool Player::takeDamage(int dmg)
{
    // if taking damage during "invincible" time, ignore
    if( (m_TimeAlive.getElapsedTime().asMilliseconds() - m_TimeDamageTaken) < m_DamageInvincibleTime_ms) return false;

    // capture time of damage taken
    m_TimeDamageTaken = m_TimeAlive.getElapsedTime().asMilliseconds();

    // take damage
    m_CurrentHealth -= dmg;

    return true;

}

void Player::update()
{
    int32_t dt = m_BlameCallback->getDeltaTime();
    Tile *coltile = NULL;
    //m_Position.x = floor(m_Position.x);
    //m_Position.y = floor(m_Position.y);
    //m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;
    //m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;

    sf::Vector2f startpos = m_Position;
    //sf::Vector2f startvel = m_Vel;

    // UPDATE X AXIS POS/VEL/ACCEL
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
            // if looking up, just look left, no transition
            if(m_LookInYAxis == 1)
            {
                m_SpriteState = FACING_LEFT;
                m_TurretPosition = 0.f;
            }
            else if(m_SpriteState != FACING_LEFT) m_SpriteState = TURNING_LEFT;
        }
        else
        {
            // if looking up, just look right, no transition
            if(m_LookInYAxis == 1)
            {
                m_SpriteState = FACING_RIGHT;
                m_TurretPosition = 1.f;
            }
            else if(m_SpriteState != FACING_RIGHT) m_SpriteState = TURNING_RIGHT;
        }
    }
    else m_Accel.x = 0;


    // clip acceleration
    if(m_Accel.x > m_MaxAccel.x) m_Accel.x = m_MaxAccel.x;
    else if(m_Accel.x < -m_MaxAccel.x) m_Accel.x = -m_MaxAccel.x;


    // update velocity from acceleration in x-axis
    m_Vel.x += m_Accel.x;

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


    // check x-axis map collision and respond
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
            m_Position.x -= (m_BoundingBox.left + m_BoundingBox.width) - coltile->boundingbox.left + 0.01;
            m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;
        }
        // moving leftward
        else if(m_Vel.x < 0)
        {
            // push out to just touch the surface
            m_Position.x += (coltile->boundingbox.left + coltile->boundingbox.width) - m_BoundingBox.left;
            m_BoundingBox.left = m_Position.x + m_BoundingBoxOffset.x;
        }
    }

    // test
    m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;
    // UPDATE Y-AXIS POS/VEL/ACCEL
    // add gravitational acceleration
    m_Accel.y += m_Gravity;

    // clip max acceleration in y-axis
    if(m_Accel.y > m_MaxAccel.y) m_Accel.y = m_MaxAccel.y;
    else if(m_Accel.y < -m_MaxAccel.y) m_Accel.y = -m_MaxAccel.y;

    // update velocity from acceleration in y-axis
    m_Vel.y += m_Accel.y;
    if(m_Vel.y > m_TerminalVel.y) m_Vel.y = m_TerminalVel.y;
    else if(m_Vel.y < -m_TerminalVel.y) m_Vel.y = -m_TerminalVel.y;

    // check y-axis map collision and respond
    m_Position.y += m_Vel.y;
    m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;
    coltile = m_BlameCallback->getMapCollision(this);
    if(coltile)
    {
        // if moving downward
        if(m_Vel.y > 0)
        {
            // push out to just touch the surface
            // note : for some reason, the exact values were still detecting ocllision when pushing away
            m_Position.y -= (m_BoundingBox.top + m_BoundingBox.height) - coltile->boundingbox.top + 0.001;
            //m_Position.y = startpos.y;
            m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;
            m_Accel.y = 0;
            m_Vel.y = 0;
            m_OnGround = true;
        }
        else if(m_Vel.y < 0)
        {
            m_Position.y += (coltile->boundingbox.top + coltile->boundingbox.height) - m_BoundingBox.top;
            m_BoundingBox.top = m_Position.y + m_BoundingBoxOffset.y;
        }
    }
    else m_OnGround = false;



    // debug info
    std::stringstream dbgss;
    dbgss << "pos:" << m_Position.x << "," << m_Position.y << ",\nvel:" << m_Vel.x << "," << m_Vel.y << "\n";
    dbgss << "accel:" << m_Accel.x << "," << m_Accel.y << "\n";
    dbgss << "onground:" << m_OnGround << std::endl;
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
        // if looking up
        if(m_LookInYAxis == 1)
        {
            m_Sprites[TURRET_UP_RIGHT]->setPosition( m_Position - sf::Vector2f(34, 43) );
            m_BarrelExit = m_Position - sf::Vector2f(7,41);
        }
        else if(m_LookInYAxis == -1)
        {
            m_Sprites[TURRET_RIGHT]->setPosition( m_Position - sf::Vector2f(32, 44) - m_CrouchAmount );
            m_BarrelExit = m_Position - sf::Vector2f(-32,12) - m_CrouchAmount;
        }
        else
        {
            m_Sprites[TURRET_RIGHT]->setPosition( m_Position - sf::Vector2f(32, 44) );
            m_BarrelExit = m_Position - sf::Vector2f(-32,12);
        }

    }
    else if(m_SpriteState == FACING_LEFT)
    {
        // if looking up
        if(m_LookInYAxis == 1)
        {
            m_Sprites[TURRET_UP_LEFT]->setPosition(m_Position - sf::Vector2f(34, 43) );
            m_BarrelExit = m_Position - sf::Vector2f(-4,41);
        }
        else if(m_LookInYAxis == -1)
        {
            m_Sprites[TURRET_LEFT]->setPosition(m_Position - sf::Vector2f(34, 44) - m_CrouchAmount );
            m_BarrelExit = m_Position - sf::Vector2f(32,12) - m_CrouchAmount;
        }
        else
        {
            m_Sprites[TURRET_LEFT]->setPosition(m_Position - sf::Vector2f(34, 44) );
            m_BarrelExit = m_Position - sf::Vector2f(32,12);
        }

    }

    //if(m_TurretPosition > 0 && m_TurretPosition < 1.f)
    if(m_LookInYAxis == -1) m_Sprites[TURRET_FORWARD]->setPosition( m_Position - sf::Vector2f(32, 44) - m_CrouchAmount );
    else m_Sprites[TURRET_FORWARD]->setPosition( m_Position - sf::Vector2f(32, 44) );

    if(m_LookInYAxis == -1) m_Sprites[CHASSIS]->setPosition( m_Position - sf::Vector2f(34, 43) - m_CrouchAmount );
    else m_Sprites[CHASSIS]->setPosition( m_Position - sf::Vector2f(34, 43) );
    m_Sprites[CHASSIS_UP_LEFT]->setPosition( m_Position - sf::Vector2f(34, 43) );
    m_Sprites[CHASSIS_UP_RIGHT]->setPosition( m_Position - sf::Vector2f(34, 43) );

    // update wheel positions
    // if moving upwards, push wheels down
    if(m_Vel.y < 0)
    {
        // if looking upwards, bring wheels together
        if(m_LookInYAxis == 1)
        {
            m_Sprites[WHEEL_LEFT]->setPosition(m_Position - sf::Vector2f(12, -8));
            m_Sprites[WHEEL_RIGHT]->setPosition(m_Position - sf::Vector2f(-9, -8) );
        }
        else
        {
            m_Sprites[WHEEL_LEFT]->setPosition(m_Position - sf::Vector2f(18, -8));
            m_Sprites[WHEEL_RIGHT]->setPosition(m_Position - sf::Vector2f(-15, -8) );
        }

    }
    // else not moving up
    else
    {
        // if looking up, bring wheels in
        if(m_LookInYAxis == 1)
        {
            m_Sprites[WHEEL_LEFT]->setPosition(m_Position - sf::Vector2f(12, -5));
            m_Sprites[WHEEL_RIGHT]->setPosition(m_Position - sf::Vector2f(-9, -5) );
        }
        else
        {
            m_Sprites[WHEEL_LEFT]->setPosition(m_Position - sf::Vector2f(18, -5));
            m_Sprites[WHEEL_RIGHT]->setPosition(m_Position - sf::Vector2f(-15, -5) );
        }

    }

    // set wheel rotation
    if(m_LeftWheelRot >=360.f) m_LeftWheelRot = m_LeftWheelRot - 360.f;
    else if(m_LeftWheelRot < 0) m_LeftWheelRot = m_LeftWheelRot + 360.f;
    if(m_RightWheelRot >= 360.f) m_RightWheelRot = m_RightWheelRot - 360.f;
    else if(m_RightWheelRot < 0) m_RightWheelRot = m_RightWheelRot + 360.f;

    m_Sprites[WHEEL_LEFT]->setRotation(m_LeftWheelRot);
    m_Sprites[WHEEL_RIGHT]->setRotation(m_RightWheelRot);

    // set repair emitter to player position
    m_Emitter_Repair->setPosition(m_Position);
    m_Emitter_Repair2->setPosition(m_Position);

    // if currently repairing, add repair particles
    if(m_IsRepairing)
    {
        if(rand()%2)
            m_Emitter_Repair->createParticle( sf::Vector2f(rand()%64-32, 0), sf::Vector2f(0, -0.02 * dt) );
        else m_Emitter_Repair2->createParticle( sf::Vector2f(rand()%64-32, 0), sf::Vector2f(0, -0.02 * dt) );

        if(m_TimeAlive.getElapsedTime().asMilliseconds() - m_RepairStartTime > m_RepairMaxTime) m_IsRepairing = false;
    }
}

void Player::draw(sf::RenderTarget *tscreen)
{
    if(!tscreen) return;

    bool dodraw = true;

    // if blinking from damage taken
    if(m_TimeAlive.getElapsedTime().asMilliseconds() - m_TimeDamageTaken < m_DamageInvincibleTime_ms)
    {
        if( sin( (3.14159 / m_DamageInvincibleBlink_ms) * (m_TimeAlive.getElapsedTime().asMilliseconds() - m_TimeDamageTaken) ) >= 0)
            dodraw = false;
    }

    if(dodraw)
    {
        // if looking straight, draw normal chassis
        if(m_LookInYAxis <= 0) tscreen->draw(*m_Sprites[CHASSIS]);

        // if facing right
        if(m_SpriteState == FACING_RIGHT)
        {
            // if facing right and looking up
            if(m_LookInYAxis == 1)
            {
                // draw chassis and turret
                tscreen->draw(*m_Sprites[CHASSIS_UP_RIGHT]);
                tscreen->draw(*m_Sprites[TURRET_UP_RIGHT]);
            }
            else tscreen->draw(*m_Sprites[TURRET_RIGHT]);
        }
        // if facing left
        else if(m_SpriteState == FACING_LEFT)
        {
            // if facing left and looking up
            if(m_LookInYAxis == 1)
            {
                // draw chassis and turret
                tscreen->draw(*m_Sprites[CHASSIS_UP_LEFT]);
                tscreen->draw(*m_Sprites[TURRET_UP_LEFT]);
            }
            else tscreen->draw(*m_Sprites[TURRET_LEFT]);
        }
        else if(m_SpriteState == TURNING_LEFT || m_SpriteState == TURNING_RIGHT) tscreen->draw(*m_Sprites[TURRET_FORWARD]);
        tscreen->draw(*m_Sprites[WHEEL_LEFT]);
        tscreen->draw(*m_Sprites[WHEEL_RIGHT]);
    }

}

bool Player::doRepair()
{
    if(m_Repairs <= 0 || m_IsRepairing ) return false;

    m_Repairs--;

    m_IsRepairing = true;
    m_RepairStartTime = m_TimeAlive.getElapsedTime().asMilliseconds();

    m_CurrentHealth = m_MaxHealth;
}
