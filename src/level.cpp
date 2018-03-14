#include "level.hpp"
#include "blame.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

#include "tools.hpp"

#include "repairitem.hpp"ddd

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

    // create teleporters
    //m_TeleporterStart = new Teleporter(TELEPORTER_START);
    //m_TeleporterEnd = new Teleporter(TELEPORTER_END);
    //m_BlameCallback->registerGameOBJ(m_TeleporterStart);
    //m_BlameCallback->registerGameOBJ(m_TeleporterEnd);


    // generate level
    genLevel();
}

Level::Level(std::string levelfile)
{
    std::ifstream ifile;

    // get game callback
    m_BlameCallback = Blame::getInstance();

    ifile.open(levelfile.c_str());

    if(!ifile.is_open())
    {
        std::cout << "ERROR LOADING LEVEL FROM FILE : " << levelfile << std::endl;
        exit(4);
    }

    std::cout << "Loading level from file : " << levelfile << std::endl;

    // load map data from CSV
    while(!ifile.eof())
    {
        std::string cmdbuf;
        std::string opbuf;
        std::getline(ifile, cmdbuf);

        // look for "command"
        size_t cpos = cmdbuf.find_first_of(':');

        // if line in file is a valid command
        if(cpos != std::string::npos)
        {
            // strip op and cmd
            opbuf = cmdbuf.substr(cpos+1);
            cmdbuf = cmdbuf.substr(0, cpos+1);

            //std::cout << "cmd=" << cmdbuf << std::endl;
            //std::cout << "opbuf=" << opbuf << std::endl;
            //std::cout << "#\n";

            // if cmd is dimensions
            if(cmdbuf == "DIMS:")
            {
                std::vector<std::string> dimcsv;

                // get dimensions
                dimcsv = csvParse(opbuf);
                m_Width = atoi(dimcsv[0].c_str());
                m_Height = atoi(dimcsv[1].c_str());

                std::cout << "Creating level with w:" << m_Width << ", h:" << m_Height << std::endl;

                // create 2d array vector
                m_Map.resize(m_Height);
                for(int i = 0; i < int(m_Height); i++) m_Map[i].resize(m_Width);

                // zeroize map data
                for(int i = 0; i < int(m_Height); i++)
                    for(int n = 0; n < int(m_Width); n++) m_Map[i][n] = m_BlameCallback->createTile(0);

                // read in following map data
                for(int i = 0; i < m_Height; i++)
                {
                    std::string lbuf;
                    std::vector<std::string> mapline;

                    std::getline(ifile, lbuf);

                    mapline = csvParse(lbuf);

                    for(int n = 0; n < m_Width; n++) setTile(n, i, atoi( mapline[n].c_str()) );
                }

            }
            else if(cmdbuf == "TPSTART:")
            {
                std::vector<std::string> pos = csvParse(opbuf);
                m_TeleporterStartPos = sf::Vector2f( atoi(pos[0].c_str()) * TILE_SIZE, atoi(pos[1].c_str()) * TILE_SIZE);
            }
            else if(cmdbuf == "TPEND:")
            {
                std::vector<std::string> pos = csvParse(opbuf);
                m_TeleporterEndPos = sf::Vector2f( atoi(pos[0].c_str()) * TILE_SIZE, atoi(pos[1].c_str()) * TILE_SIZE);
            }
            else if(cmdbuf == "REPAIRITEM:")
            {
                std::vector<std::string> pos = csvParse(opbuf);
                m_SpawnRepairItems.push_back(sf::Vector2f( atof(pos[0].c_str())*TILE_SIZE, atof(pos[1].c_str())*TILE_SIZE));
            }
            else std::cout << "Unregognized map command!\n";


        }

    }


    ifile.close();

}

Level::~Level()
{
    // delete all tiles
    for(int i = 0; i < int(m_Height); i++)
        for(int n = 0; n < int(m_Width); n++) delete m_Map[i][n];

}

void Level::startLevel()
{
    // create and register items

    // note : register game objs not necessary, automatic upon creation

    // create and register teleports
    Teleporter *newstarttp = new Teleporter(TELEPORTER_START);
    newstarttp->m_Position = m_TeleporterStartPos;
    //m_BlameCallback->registerGameOBJ(newstarttp);

    Teleporter *newendtp = new Teleporter(TELEPORTER_END);
    newendtp->m_Position = m_TeleporterEndPos;
    //m_BlameCallback->registerGameOBJ(newendtp);

    // create and register repair items
    for(int i = 0; i < int(m_SpawnRepairItems.size()); i++)
    {
        RepairItem *newrepair = new RepairItem();
        newrepair->m_Position = m_SpawnRepairItems[i];
        //m_BlameCallback->registerGameOBJ(newrepair);
    }


}

void Level::endLevel()
{

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
    // ceiling spikes
    if(tilenum == 2)
    {
        bb.top = y * TILE_SIZE;
        bb.height = 24;
    }
    // floor spikes
    else if(tilenum == 3)
    {
        bb.top = y * TILE_SIZE + 10;
        bb.height = 22;
        bb.left = x * TILE_SIZE + 2;
        bb.width = TILE_SIZE-4;
    }
    // lava
    else if(tilenum == 4)
    {
        bb.top = y * TILE_SIZE + 16;
        bb.height = 16;
    }
    else
    {
        bb.top = y * TILE_SIZE;
        bb.height = TILE_SIZE;
    }
    m_Map[y][x]->boundingbox = bb;

    return true;

}

void Level::genLevel()
{

    for(int i = 0; i < int(m_Width); i++) setTile(i, 0, 1);

    // bottom 2 rows solid
    for(int i = 0; i < int(m_Width); i++) setTile(i, m_Height-1, 1);
    for(int i = 0; i < int(m_Width); i++) setTile(i, m_Height-2, 1);

    // add some lava
    setTile(8, m_Height-2, 4);
    setTile(7, m_Height-2, 4);

    // ceiling spikes
    setTile(1,1, 2);
    setTile(16, m_Height-3, 3);

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

sf::Vector2f Level::getStartingPosition()
{
    return m_TeleporterStartPos;
}
