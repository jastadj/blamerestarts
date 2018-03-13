#ifndef CLASS_PLAYER
#define CLASS_PLAYER

#include "gameobj.hpp"
#include "spritesheet.hpp"
#include "particle.hpp"

class Player: public GameOBJ
{
private:

    // sprite sheets for player sprite components
    static SpriteSheet *m_ChassisSS;
    static SpriteSheet *m_TurretSS;
    static SpriteSheet *m_WheelSS;

    enum PLAYERSPRITES{CHASSIS, TURRET_RIGHT, TURRET_FORWARD, TURRET_LEFT, WHEEL_LEFT, WHEEL_RIGHT, TURRET_UP_RIGHT,
                        TURRET_UP_LEFT};
    enum PLAYERSTATES
    {
        FACING_RIGHT, FACING_RIGHT_UP, TURNING_RIGHT, FACING_LEFT, FACING_LEFT_UP, TURNING_LEFT
    };

    // spawn position for bullets
    sf::Vector2f m_BarrelExit;

    // player properties
    float m_LeftWheelRot;
    float m_RightWheelRot;
    float m_DriveSpeed;
    float m_TurretPosition;
    float m_TurretSpeed;
    sf::Vector2f m_TerminalVel;
    sf::Vector2f m_MaxAccel;
    sf::Vector2f m_VelCutoff;
    float m_Gravity;
    float m_Friction;
    bool m_OnGround;
    float m_JumpForce;

    int m_DamageInvincibleTime_ms;
    int m_DamageInvincibleBlink_ms;
    int32_t m_TimeDamageTaken;
    int m_CurrentHealth;
    int m_MaxHealth;

    int m_Repairs;
    int32_t m_RepairStartTime;
    int m_RepairMaxTime;
    bool m_IsRepairing;
    ParticleEmitter *m_Emitter_Repair;
    ParticleEmitter *m_Emitter_Repair2;

public:
    Player(sf::Vector2f tpos);
    ~Player();

    void update();
    void draw(sf::RenderTarget *tscreen);

    int m_Drive; // -1 = left, 0 = stopped, 1 = right
    void shoot();
    void jump();

    int getHealth() { return m_CurrentHealth;}
    int getMaxHealth() { return m_MaxHealth;}
    bool takeDamage(int dmg);
    int getRepairs() { return m_Repairs;}
    bool doRepair();
};
#endif // CLASS_PLAYER
