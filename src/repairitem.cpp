#include "repairitem.hpp"

SpriteSheet *RepairItem::m_RepairItemSS = new SpriteSheet(".\\data\\art\\uigfx.png", 3, 1);

RepairItem::RepairItem()
{
    m_Sprites.push_back(m_RepairItemSS->createSprite(2));
    m_Sprites.back()->setOrigin(sf::Vector2f(16,16));

    m_BoundingBox.width = 32;
    m_BoundingBox.height = 32;


}

RepairItem::~RepairItem()
{

}

void RepairItem::update()
{
    m_Sprites[0]->setPosition(m_Position);
    m_BoundingBox.left = m_Position.x - 16;
    m_BoundingBox.top = m_Position.y - 16;
}

void RepairItem::draw(sf::RenderTarget *tscreen)
{
    tscreen->draw(*m_Sprites[0]);
}
