#include "teleporter.hpp"

SpriteSheet *Teleporter::m_TeleporterSS = new SpriteSheet(".\\data\\art\\teleporter.png", 2, 1);

Teleporter::Teleporter(TELE_TYPE ttype)
{
    m_Type = ttype;

    if(m_Type == TELEPORTER_END) m_Sprites.push_back(m_TeleporterSS->createSprite(0));
    else m_Sprites.push_back(m_TeleporterSS->createSprite(1));

    m_Sprites.back()->setOrigin(sf::Vector2f(0, 32));

    // init bounding box
    m_BoundingBox.width = 16;
    m_BoundingBox.height = 22;
}

Teleporter::~Teleporter()
{

}

void Teleporter::update()
{
    m_Sprites[0]->setPosition(m_Position);
    m_BoundingBox.left = m_Position.x + 22;
    m_BoundingBox.top = m_Position.y + 5;
}

void Teleporter::draw(sf::RenderTarget *tscreen)
{
    tscreen->draw(*m_Sprites[0]);
}
