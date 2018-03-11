#ifndef CLASS_PLAYER
#define CLASS_PLAYER

#include "gameobj.hpp"
#include "spritesheet.hpp"

class Player: public GameOBJ
{
private:

    // sprite sheets for player sprite components
    static SpriteSheet *m_ChassisSS;
    static SpriteSheet *m_TurretSS;
    static SpriteSheet *m_WheelSS;

    enum PLAYERSPRITES{CHASSIS, TURRET_RIGHT, TURRET_FORWARD, TURRET_LEFT, WHEEL_LEFT, WHEEL_RIGHT};
    enum PLAYERSTATES
    {
        FACING_RIGHT, TURNING_RIGHT, FACING_LEFT, TURNING_LEFT
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

public:
    Player();
    ~Player();

    void update();
    void draw(sf::RenderTarget *tscreen);

    int m_Drive; // -1 = left, 0 = stopped, 1 = right
    void shoot();
};
#endif // CLASS_PLAYER
