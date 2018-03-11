#ifndef CLASS_GAMEOBJ
#define CLASS_GAMEOBJ

#include <vector>
#include <SFML\Graphics.hpp>

// forward dec
class Blame;

// base class for all game objects
class GameOBJ
{
protected:

    Blame *m_BlameCallback;

    // sprite
    std::vector<sf::Sprite*> m_Sprites;
    int m_SpriteState;
    sf::Vector2f m_BoundingBoxOffset;

public:
    GameOBJ();
    virtual ~GameOBJ();

    // properties
    sf::Vector2f m_Position;
    sf::Vector2f m_Vel;
    sf::Vector2f m_Accel;
    sf::FloatRect m_BoundingBox;

    virtual void update()=0;
    virtual void draw(sf::RenderTarget *tscreen);
};
#endif // CLASS_GAMEOBJ
