#ifndef CLASS_REPAIRITEM

#include "gameobj.hpp"
#include "spritesheet.hpp"

class RepairItem:public GameOBJ
{
private:

    static SpriteSheet *m_RepairItemSS;

public:
    RepairItem();
    ~RepairItem();

    void update();
    void draw(sf::RenderTarget *tscreen);
};

#endif // CLASS_REPAIRITEM
