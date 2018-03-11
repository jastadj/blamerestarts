#ifndef CLASS_LEVEL
#define CLASS_LEVEL

#include <vector>
#include <SFML\Graphics.hpp>

#define TILE_SIZE 32

// forward dec
class Blame;

struct Tile
{
    bool blocked;

    sf::Sprite *sprite;
    int id;
    sf::FloatRect boundingbox;

    Tile(): blocked(true)
        {
        }
};

class Level
{
private:

    Blame *m_BlameCallback;

    std::vector< std::vector<Tile*> > m_Map;

    unsigned int m_Width;
    unsigned int m_Height;

    void genLevel();

public:
    Level(unsigned int width, unsigned int height);
    ~Level();

    sf::Vector2i getDims();

    Tile *getTile(int x, int y);
    bool setTile(int x, int y, int tilenum);
};
#endif // CLASS_LEVEL
