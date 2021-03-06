#ifndef CLASS_LEVEL
#define CLASS_LEVEL

#include <vector>
#include <string>
#include <SFML\Graphics.hpp>

#include "teleporter.hpp"

#define TILE_SIZE 32

// forward dec
class Blame;

struct Tile
{
    bool blocked;

    sf::Sprite *sprite;
    int id;
    sf::FloatRect boundingbox;

    int tiledamage;

    Tile(): blocked(true), tiledamage(0)
        {
        }
};

class Level
{
private:

    Blame *m_BlameCallback;

    std::vector< std::vector<Tile*> > m_Map;
    std::vector<GameOBJ*> m_ObjectsToInit;

    unsigned int m_Width;
    unsigned int m_Height;

    sf::Vector2f m_TeleporterStartPos;
    sf::Vector2f m_TeleporterEndPos;

    // spawn lists
    std::vector<sf::Vector2f> m_SpawnRepairItems;
    std::vector<sf::Vector2f> m_SpawnDrones;

    void genLevel();

public:
    Level(unsigned int width, unsigned int height);
    Level(std::string levelfile);
    ~Level();

    sf::Vector2i getDims();

    Tile *getTile(int x, int y);
    bool setTile(int x, int y, int tilenum);

    sf::Vector2f getStartingPosition();
    void startLevel();
    void endLevel();

};
#endif // CLASS_LEVEL
