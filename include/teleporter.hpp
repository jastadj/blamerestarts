#ifndef CLASS_TELEPORTER
#define CLASS_TELEPORTER

#include "gameobj.hpp"
#include "spritesheet.hpp"

enum TELE_TYPE{TELEPOTER_START, TELEPORTER_END};

class Teleporter:public GameOBJ
{
private:

    static SpriteSheet *m_TeleporterSS;

    int m_Type;

public:
    Teleporter(TELE_TYPE ttype);
    ~Teleporter();

    void update();
    void draw(sf::RenderTarget *tscreen);

    bool isTeleporterEnd() { if(m_Type == TELEPORTER_END) return true; else return false;}
};
#endif // CLASS_TELEPORTER
