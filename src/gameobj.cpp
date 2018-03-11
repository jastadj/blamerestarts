#include "gameobj.hpp"
#include "blame.hpp"

GameOBJ::GameOBJ()
{
    // init sprite state
    m_SpriteState = 0;

    // link gameobj to Blame game
    m_BlameCallback = Blame::getInstance();

    // register self with game
    m_BlameCallback->registerGameOBJ(this);
}

GameOBJ::~GameOBJ()
{
    // cleanup all the sprites created for this object
    for(int i = 0; i < int(m_Sprites.size()); i++)
        delete m_Sprites[i];

}

void GameOBJ::update()
{

}

void GameOBJ::draw(sf::RenderTarget *tscreen)
{

}
