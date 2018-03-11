#include "level.hpp"
#include "blame.hpp"

Level::Level(unsigned int width, unsigned int height)
{
    // get game callback
    m_BlameCallback = Blame::getInstance();

    // init dimensions
    m_Width = width;
    m_Height = height;

    // control minimum dimensions
    if(m_Width < 10) m_Width = 10;
    if(m_Height < 10) m_Height = 10;

    // create 2d array vector
    m_Map.resize(m_Height);
    for(int i = 0; i < int(m_Height); i++) m_Map[i].resize(m_Width);

    // zeroize map data
    for(int i = 0; i < int(m_Height); i++)
        for(int n = 0; n < int(m_Width); n++) m_Map[i][n] = m_BlameCallback->createTile(0);

    // generate level
    genLevel();
}

Level::~Level()
{
    // delete all tiles
    for(int i = 0; i < int(m_Height); i++)
        for(int n = 0; n < int(m_Width); n++) delete m_Map[i][n];
}

sf::Vector2i Level::getDims()
{
    return sf::Vector2i(m_Width, m_Height);
}

Tile *Level::getTile(int x, int y)
{
    if(x < 0 || y < 0) return NULL;

    if(x >= int(m_Width) || y >= int(m_Height)) return NULL;

    return m_Map[y][x];
}

bool Level::setTile(int x, int y, int tilenum)
{
    sf::FloatRect bb;

    if(x < 0 || y < 0) return false;

    if(x >= int(m_Width) || y >= int(m_Height)) return false;

    // delete existing tile
    delete m_Map[y][x];

    // create new tile
    m_Map[y][x] = m_BlameCallback->createTile(tilenum);
    m_Map[y][x]->sprite->setPosition(x*TILE_SIZE, y*TILE_SIZE);

    // set new tile bounding box
    bb.left = x * TILE_SIZE;
    bb.width = TILE_SIZE;
    bb.top = y * TILE_SIZE;
    bb.height = TILE_SIZE;
    m_Map[y][x]->boundingbox = bb;

    return true;

}

void Level::genLevel()
{

    for(int i = 0; i < int(m_Width); i++) setTile(i, 0, 1);

    for(int i = 0; i < int(m_Width); i++) setTile(i, m_Height-1, 1);

    for(int i = 0; i < int(m_Height); i++) setTile(0, i, 1);

    for(int i = 0; i < int(m_Width); i++) setTile(m_Width-1, i, 1);

    setTile(5,5,1);
    setTile(5,4,1);
    setTile(5,3,1);
    setTile(5,2,1);
    setTile(5,1,1);

    setTile(2,8, 1);
    setTile(3,8, 1);
    setTile(4,8, 1);

}
