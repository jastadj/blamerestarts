#include "gameobj.hpp"
#include "blame.hpp"

GameOBJ::GameOBJ()
{
    m_SpriteState = 0;
    m_BlameCallback = Blame::getInstance();
}

GameOBJ::~GameOBJ()
{
    for(int i = 0; i < int(m_Sprites.size()); i++)
        delete m_Sprites[i];

}

void GameOBJ::update()
{

}

void GameOBJ::draw(sf::RenderTarget *tscreen)
{

}
