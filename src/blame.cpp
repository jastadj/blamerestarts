#include "blame.hpp"

#include "keys.hpp"

Blame *Blame::m_Instance = NULL;

Blame::Blame()
{
    std::cout << "Blame" << std::endl;

    m_Screen = NULL;
    m_ParticleManager = NULL;
    m_Player = NULL;
    m_TilesSS = NULL;
    m_CurrentLevel = NULL;
    dbg_txt = NULL;
    m_UISS = NULL;

    // default screem dimensions
    m_ScreenWidth = 800;
    m_ScreenHeight = 600;

    // init vars
    m_DeltaTime = 0;
    m_Instance = this;
    m_DebugMode = false;
    m_EndLevelTriggered = false;
    m_GameMode = TITLESCREEN;

    init();

}

Blame::~Blame()
{

}

void Blame::toggleDebugMode()
{
#ifdef DEBUG
    m_DebugMode = !m_DebugMode;
    m_KeyList = getRandomKeys();
#endif
}

bool Blame::init()
{
    // init render window
    m_Screen = new sf::RenderWindow(sf::VideoMode(m_ScreenWidth, m_ScreenHeight,32), "Blame Restarts");
    m_View = sf::View(sf::Vector2f(m_ScreenWidth/2, m_ScreenHeight/2), sf::Vector2f(m_ScreenWidth, m_ScreenHeight) );

    if(!m_Screen) {std::cout << "Error creating render window.\n"; exit(1);}

    // render settings
    m_Screen->setFramerateLimit(60); // 60fps limit
    m_Screen->setVerticalSyncEnabled(true);
    // zoom in view
    m_View.zoom(0.5);

    // init tile sprite sheet
    const int tilesw = 4;
    const int tilesh = 1;
    m_TilesSS = new SpriteSheet(".\\data\\art\\tiles.png", tilesw, tilesh);

    // init ui sprite sheet
    m_UISS = new SpriteSheet(".\\data\\art\\uigfx.png", 3, 1);
    m_UI_Health_Sprites.push_back( m_UISS->createSprite(0));
    m_UI_Health_Sprites.push_back( m_UISS->createSprite(1));
    m_UI_Repair_Sprite = m_UISS->createSprite(2);


    // create fonts and debug text
    m_Font.loadFromFile(".\\data\\font\\droidsansmono.ttf");
    dbg_txt = new sf::Text("test", m_Font, 12);

    // create particle manager
    m_ParticleManager = new ParticleManager;

    // load title textures
    m_TitleTextures.push_back(sf::Texture());
    m_TitleTextures.back().loadFromFile(".\\data\\art\\title.png");

    // load sounds
    m_Sounds.push_back(sf::SoundBuffer());
    m_Sounds.back().loadFromFile(".\\data\\sounds\\healsound.wav");
    m_Sounds.push_back(sf::SoundBuffer());
    m_Sounds.back().loadFromFile(".\\data\\sounds\\tele.wav");
    m_Sounds.push_back(sf::SoundBuffer());
    m_Sounds.back().loadFromFile(".\\data\\sounds\\start.ogg");
    m_Sounds.push_back(sf::SoundBuffer());
    m_Sounds.back().loadFromFile(".\\data\\sounds\\land.wav");
    m_Sounds.push_back(sf::SoundBuffer());
    m_Sounds.back().loadFromFile(".\\data\\sounds\\shoot.ogg");
    m_Sounds.push_back(sf::SoundBuffer());
    m_Sounds.back().loadFromFile(".\\data\\sounds\\death.ogg");

    while(1)
    {
        if(m_GameMode == TITLESCREEN) titleLoop();
        else if(m_GameMode == NEWGAME) newGame();
        else if(m_GameMode == QUIT) break;
    }


    return true;
}

Tile *Blame::createTile(int tilenum)
{
    Tile *newtile = new Tile;

    if(tilenum >0 && tilenum <= m_TilesSS->getCount())
    {
        newtile->sprite = m_TilesSS->createSprite(tilenum-1);
        newtile->id = tilenum;

        // spikes
        if(tilenum == 2 || tilenum == 3)
        {
            newtile->tiledamage = 1;
            newtile->blocked = false;
        }
        // lava
        else if(tilenum == 4)
        {
            newtile->tiledamage = 2;
            newtile->blocked = false;
        }
    }
    else
    {
        newtile->sprite = new sf::Sprite();
        newtile->blocked = false;
        newtile->id = 0;

        if(tilenum != 0) std::cout << "Error creating tile, tilenum " << tilenum << " is not valid.\n";
    }

    return newtile;
}

bool Blame::registerGameOBJ(GameOBJ *tobj)
{
    if(!tobj) return false;

    // check that game object isn't already registered...
    for(int i = 0; i < int(m_GameObjects.size()); i++)
    {
        if(m_GameObjects[i] == tobj) return false;
    }

    m_GameObjects.push_back(tobj);

    return true;
}

bool Blame::destroyGameOBJ(GameOBJ *tobj)
{
    if(!tobj) return false;

    // check that game object exists
    for(int i = 0; i < int(m_GameObjects.size()); i++)
    {
        if(m_GameObjects[i] == tobj)
        {
            delete m_GameObjects[i];
            m_GameObjects.erase(m_GameObjects.begin() + i);
            return true;
        }
    }

    // object not found
    return false;
}

void Blame::newGame()
{
    //  init random seed
    srand( time(NULL));

    // get random keys
    m_KeyList = getRandomKeys();

    // delete all current game objects
    for(int i = 0; i < int(m_GameObjects.size()); i++)
        delete m_GameObjects[i];
    m_GameObjects.clear();

    // delete level
    for(int i = 0; i < int(m_Levels.size()); i++)
        delete m_Levels[i];
    m_Levels.clear();

    // clear any existing particles
    m_ParticleManager->clear();

    // create levels and set current level to first level
    m_Levels.push_back(new Level(".\\data\\levels\\level_01.txt"));
    m_Levels.push_back(new Level(".\\data\\levels\\level_02.txt"));
    m_CurrentLevel = m_Levels[0];

    // init player
    m_Player = new Player( m_CurrentLevel->getStartingPosition() );
    //if(!registerGameOBJ(m_Player)) {std::cout << "Error registering player game object.\n"; exit(2);}

    // start first level
    m_CurrentLevel->startLevel();
    showMessage("Controls are randomized A-Z.", sf::Color::White);
    int returnmode = mainLoop();

    // next level
    while(returnmode != 0 && returnmode != 2)
    {

        std::cout << "Finding next level...\n";

        m_EndLevelTriggered = false;
        if(m_CurrentLevel == m_Levels.back())
        {
            showMessage("YOU WIN!", sf::Color::Green);
            break;
        }

        // find next level
        for(int i = 0; i < int(m_Levels.size()); i++)
        {
            if(m_CurrentLevel == m_Levels[i])
            {
                m_CurrentLevel = m_Levels[i+1];
                m_Player->m_Position = m_CurrentLevel->getStartingPosition();
                m_Player->reset();
                break;
            }
        }


        // init level - init game objects
        m_CurrentLevel->startLevel();

        // start game loop
        returnmode = mainLoop();
    }

    m_GameMode = TITLESCREEN;

}

void Blame::titleLoop()
{
    bool quit = false;

    m_Screen->setView(m_Screen->getDefaultView());

    sf::Sprite tsprite(m_TitleTextures[0]);

    sf::Text newgametxt("New Game", m_Font, 22);
    newgametxt.setOrigin(newgametxt.getLocalBounds().width/2, newgametxt.getLocalBounds().height/2);
    newgametxt.setPosition(m_ScreenWidth/2, 400);
    newgametxt.setScale(2.f, 2.f);

    sf::Text quittxt("Quit", m_Font, 22);
    quittxt.setOrigin(quittxt.getLocalBounds().width/2, quittxt.getLocalBounds().height/2);
    quittxt.setPosition(m_ScreenWidth/2, 480);
    quittxt.setScale(2.f, 2.f);

    while(!quit)
    {

        // clear screen
        m_Screen->clear(sf::Color(0x0b,0x48,0xf4));

        sf::Event event;

        sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(*m_Screen));;

        while(m_Screen->pollEvent(event))
        {
            if(event.type == sf::Event::Closed) quit = true;
            else if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::Escape) quit = true;
            }
            else if(event.type == sf::Event::MouseButtonPressed)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    if(newgametxt.getGlobalBounds().contains(mousePos))
                    {
                        m_GameMode = NEWGAME;
                        quit = true;
                    }

                    if(quittxt.getGlobalBounds().contains(mousePos))
                    {
                        m_GameMode = QUIT;
                        quit = true;
                    }
                }
            }
        }

        // update
        // if mouse is inside newgame text
        if(newgametxt.getGlobalBounds().contains(mousePos))
        {
            newgametxt.setColor(sf::Color::Yellow);
        }
        else newgametxt.setColor(sf::Color::White);

        if(quittxt.getGlobalBounds().contains(mousePos))
        {
            quittxt.setColor(sf::Color::Yellow);
        }
        else quittxt.setColor(sf::Color::White);

        // draw
        m_Screen->draw(tsprite);
        m_Screen->draw(newgametxt);
        m_Screen->draw(quittxt);

        // show screen
        m_Screen->display();

    }
}

int Blame::mainLoop()
{
    bool quit = false;
    bool playerdead = false;

    /*
    ParticleEmitter p1(m_ParticleManager, PEMIT_CUSTOM, sf::Vector2f( 400, 300) );
    p1.m_custom_min_life = 500;
    p1.m_custom_max_life = 1000;
    //p1.m_custom_color = sf::Color(236,191,44,255);
    p1.m_custom_accel = sf::Vector2f(0, 0.05);
    */

    clearSoundQue();

    // register player
    registerGameOBJ(m_Player);

    sf::Clock frameclock;

    std::cout << "Starting main loop.  " << m_GameObjects.size() << " game objects.\n";

    sf::Keyboard::Key mykey = sf::Keyboard::Escape;

    playSound(SOUND_TELE);

    // main game loop
    while(!quit)
    {
        // clear screen
        m_Screen->clear();

        // set view
        //m_Screen->setView(m_View);


        // event que
        sf::Event event;

        // actively pressed / not pressed
        if(!sf::Keyboard::isKeyPressed(m_KeyList[C_RIGHT]) && !sf::Keyboard::isKeyPressed(m_KeyList[C_LEFT]) ) m_Player->m_Drive = 0;
        if(!sf::Keyboard::isKeyPressed(m_KeyList[C_UP]) && !sf::Keyboard::isKeyPressed(m_KeyList[C_DOWN]) ) m_Player->lookInY(0);


        // handle all input events
        while(m_Screen->pollEvent(event))
        {
            // if render window is not in focus, ignore inputs
            if(!m_Screen->hasFocus()) break;

            // if window closed
            if(event.type == sf::Event::Closed) quit = true;
            // if a key was pressed
            else if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::Escape) quit = true;
                else if(m_EndLevelTriggered) break;
                else if(event.key.code == sf::Keyboard::F1) toggleDebugMode();
                else if(event.key.code == sf::Keyboard::Space && m_DebugMode) m_Player->jump();
                else if(event.key.code == m_KeyList[C_JUMP] && !m_DebugMode) m_Player->jump();
                else if(event.key.code == m_KeyList[C_REPAIR]) m_Player->doRepair();
                else if(event.key.code == m_KeyList[C_RIGHT]) m_Player->m_Drive = 1;
                else if(event.key.code == m_KeyList[C_LEFT]) m_Player->m_Drive = -1;
                else if(event.key.code == m_KeyList[C_UP]) m_Player->lookInY(1);
                else if(event.key.code == m_KeyList[C_DOWN]) m_Player->lookInY(-1);
                else if(event.key.code == m_KeyList[C_FIRE]) m_Player->shoot();
            }
            else if(m_EndLevelTriggered) break;
            else if(event.type == sf::Event::KeyReleased)
            {
                if(event.key.code == m_KeyList[C_RIGHT])
                {
                    if(sf::Keyboard::isKeyPressed(m_KeyList[C_LEFT])) m_Player->m_Drive = -1;
                }
                else if(event.key.code == m_KeyList[C_LEFT])
                {
                    if(sf::Keyboard::isKeyPressed(m_KeyList[C_RIGHT])) m_Player->m_Drive = 1;
                }
                else if(event.key.code == m_KeyList[C_UP])
                {
                    if(sf::Keyboard::isKeyPressed(m_KeyList[C_DOWN])) m_Player->lookInY(-1);
                }
                else if(event.key.code == m_KeyList[C_DOWN])
                {
                    if(sf::Keyboard::isKeyPressed(m_KeyList[C_UP])) m_Player->lookInY(1);
                }
            }
            // if mouse button pressed
            else if(event.type == sf::Event::MouseButtonPressed)
            {
                // if left mouse button pressed
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    // particle test
                    //p1.createParticle(sf::Vector2f(0,0), sf::Vector2f(1,-0.7));
                    if(m_DebugMode) m_Player->shoot();
                }
            }
        }






        // update stuff
        m_ParticleManager->update();
        for(int i = int(m_GameObjects.size()-1); i >= 0; i--)
        {
            if(!m_EndLevelTriggered) m_GameObjects[i]->update();
        }

        // if player is dead
        if(m_Player->getHealth() <= 0)
        {
            playSound(SOUND_DEATH);
            playerdead = true;
            quit = true;
        }

        // update view
        m_View.setCenter(m_Player->m_Position);
        m_Screen->setView(m_View);

        // draw stuff
        drawLevel();

        // draw all game objects
        for(int i = int(m_GameObjects.size()-1); i >= 0; i--)
        {
            m_GameObjects[i]->draw(m_Screen);
        }
        // draw particles
        m_ParticleManager->draw(m_Screen);

        // draw rects (debug)
        if(m_DebugMode)
        {
            for(int i = int(m_GameObjects.size()-1); i >= 0; i--)
            {
                drawRect( m_GameObjects[i]->m_BoundingBox);
            }
        }

        // draw hud
        m_Screen->setView( m_Screen->getDefaultView());
        drawHUD();
        if(m_DebugMode) m_Screen->draw(*dbg_txt);

        // render screen
        m_Screen->display();

        m_DeltaTime = frameclock.getElapsedTime().asMilliseconds();
        frameclock.restart();

        if(m_EndLevelTriggered) quit = true;

    }

    // cleanup level
    m_CurrentLevel->endLevel();

    // delete all game objects except player
    for(int i = 0; i < int(m_GameObjects.size()); i++)
    {
        if(m_GameObjects[i] == m_Player) m_GameObjects[i] = NULL;
        else destroyGameOBJ(m_GameObjects[i]);
    }
    // clear entire list
    m_GameObjects.clear();
    // add player back into list
    m_GameObjects.push_back(m_Player);

    // if player quit manually
    if(m_EndLevelTriggered) return 1;
    // else
    else if(playerdead)
    {
        showMessage("YOU DIED", sf::Color::Red);
        return 2;
    }
    else return 0;

}

void Blame::drawLevel()
{
    sf::Vector2i dims = m_CurrentLevel->getDims();

    for(int i = 0; i < dims.y; i++)
    {
        for(int n = 0; n < dims.x; n++)
        {
            Tile *ttile = m_CurrentLevel->getTile(n,i);

            // if tile id is not 0 (empty), draw tile
            if(ttile->id != 0)
            {
                m_Screen->draw( *m_CurrentLevel->getTile(n,i)->sprite );
                if(m_DebugMode)
                {
                    drawRect( m_CurrentLevel->getTile(n,i)->boundingbox);
                }
            }

        }
    }
}

void Blame::drawRect(sf::FloatRect trect)
{
    sf::RectangleShape rshape(sf::Vector2f(trect.width-2, trect.height-2));
    rshape.setFillColor(sf::Color::Transparent);
    rshape.setOutlineThickness(1.f);
    rshape.setOutlineColor(sf::Color::Red);
    rshape.setPosition(sf::Vector2f(trect.left+1, trect.top+1) );

    m_Screen->draw(rshape);

}

void Blame::drawHUD()
{
    sf::Vector2f healthoffset;
    sf::Vector2f repairoffset;

    int pmaxh = m_Player->getMaxHealth();
    int pcurh = m_Player->getHealth();

    int pcurr = m_Player->getRepairs();

    for(int i = 0; i < pmaxh; i++)
    {
        if(i < pcurh)
        {
            m_UI_Health_Sprites[1]->setPosition( sf::Vector2f(i*32, 0) + healthoffset);
            m_Screen->draw( *m_UI_Health_Sprites[1]);
        }
        else
        {
            m_UI_Health_Sprites[0]->setPosition( sf::Vector2f(i*32, 0) + healthoffset);
            m_Screen->draw( *m_UI_Health_Sprites[0]);
        }
    }

    repairoffset = healthoffset + sf::Vector2f(pmaxh*32, 0) + sf::Vector2f(32,0);

    for(int i = 0; i < pcurr; i++)
    {
        if(i < pcurr)
        {
            m_UI_Repair_Sprite->setPosition( sf::Vector2f(i*32,0) + repairoffset);
            m_Screen->draw( *m_UI_Repair_Sprite);
        }
    }





}

Tile *Blame::getMapCollision(GameOBJ *tobj)
{
    if(!tobj)
    {
        std::cout << "Error in getMapCollision, target object is null!\n";
        return NULL;
    }

    // map array "tile number" range for top left and bottom right
    // these are used to scan each occupied tile by the game object
    sf::Vector2i p1( int(floor(tobj->m_BoundingBox.left/TILE_SIZE)),
                     int(floor(tobj->m_BoundingBox.top/TILE_SIZE)) );
    sf::Vector2i p2( int(floor( (tobj->m_BoundingBox.left + tobj->m_BoundingBox.width)/TILE_SIZE ) ),
                     int(floor( (tobj->m_BoundingBox.top + tobj->m_BoundingBox.height)/TILE_SIZE) ) );


    // go through each occupied tile by the game obj and determine if its a "blocked" tile
    for(int i = p1.y; i <= p2.y; i++)
    {
        for(int n = p1.x; n <= p2.x; n++)
        {
            // the one of the occupied tiles from the current level
            Tile *ttile = m_CurrentLevel->getTile(n,i);

            // successfully retrieved map tile
            if(ttile)
            {
                // tile is blocked (can't pass through)
                if(ttile->blocked)
                {
                    // return this tile
                    return ttile;
                }
                // else if player is touching a damaging tile
                else if(ttile->tiledamage > 0 && tobj == m_Player && ttile->boundingbox.intersects(m_Player->m_BoundingBox) )
                {
                    m_Player->takeDamage( ttile->tiledamage );
                }
            }

        }
    }

    // all tiles non blocked
    return NULL;
}

std::vector<GameOBJ*> Blame::getObjectCollisions(GameOBJ *tobj)
{
    std::vector<GameOBJ*> ocol;

    if(!tobj) return ocol;

    for(int i = 0; i < int(m_GameObjects.size()); i++)
    {
        if(m_GameObjects[i] == tobj) continue;

        if(m_GameObjects[i]->m_BoundingBox.intersects( tobj->m_BoundingBox))
            ocol.push_back(m_GameObjects[i]);
    }

    return ocol;
}

void Blame::triggerEndLevel()
{
    m_EndLevelTriggered = true;
}

std::vector<sf::Keyboard::Key> Blame::getRandomKeys()
{
    std::vector<sf::Keyboard::Key> keys;

    std::vector<sf::Keyboard::Key> allkeys;
    allkeys.push_back(sf::Keyboard::A);
    allkeys.push_back(sf::Keyboard::B);
    allkeys.push_back(sf::Keyboard::C);
    allkeys.push_back(sf::Keyboard::D);
    allkeys.push_back(sf::Keyboard::E);
    allkeys.push_back(sf::Keyboard::F);
    allkeys.push_back(sf::Keyboard::G);
    allkeys.push_back(sf::Keyboard::H);
    allkeys.push_back(sf::Keyboard::I);
    allkeys.push_back(sf::Keyboard::J);
    allkeys.push_back(sf::Keyboard::K);
    allkeys.push_back(sf::Keyboard::L);
    allkeys.push_back(sf::Keyboard::M);
    allkeys.push_back(sf::Keyboard::N);
    allkeys.push_back(sf::Keyboard::O);
    allkeys.push_back(sf::Keyboard::P);
    allkeys.push_back(sf::Keyboard::Q);
    allkeys.push_back(sf::Keyboard::R);
    allkeys.push_back(sf::Keyboard::S);
    allkeys.push_back(sf::Keyboard::T);
    allkeys.push_back(sf::Keyboard::U);
    allkeys.push_back(sf::Keyboard::V);
    allkeys.push_back(sf::Keyboard::W);
    allkeys.push_back(sf::Keyboard::X);
    allkeys.push_back(sf::Keyboard::Y);
    allkeys.push_back(sf::Keyboard::Z);

    if(m_DebugMode)
    {
        keys.resize(C_TOTAL);

        // WASD
        keys[C_UP] = sf::Keyboard::W;
        keys[C_DOWN] = sf::Keyboard::S;
        keys[C_LEFT] = sf::Keyboard::A;
        keys[C_RIGHT] = sf::Keyboard::D;

        keys[C_REPAIR] = sf::Keyboard::R;
    }
    else
    {
        // get a key for each key command
        for(int i = 0; i < C_TOTAL; i++)
        {
            // get random key from remaining keys
            int tkey = rand()%int(allkeys.size());

            // put that key into new random key list
            keys.push_back(allkeys[tkey]);

            // remove that key from the master key list so it can't be picked again
            allkeys.erase(allkeys.begin() + tkey);
        }
    }


    return keys;
}

void Blame::showMessage(std::string msg, sf::Color mcolor)
{
    bool quit = false;

    sf::Text msgtxt(msg, m_Font, 18);
    msgtxt.setColor(mcolor);
    msgtxt.setOrigin(msgtxt.getLocalBounds().width/2, msgtxt.getLocalBounds().height/2);
    msgtxt.setPosition(m_ScreenWidth/2, 300);

    sf::Text clicktxt("(click to continue...)", m_Font, 15);
    clicktxt.setColor(mcolor);
    clicktxt.setOrigin(clicktxt.getLocalBounds().width/2, clicktxt.getLocalBounds().height/2);
    clicktxt.setPosition(m_ScreenWidth/2, 500);





    while(!quit)
    {
        m_Screen->clear();

        sf::Event event;

        while(m_Screen->pollEvent(event))
        {
            if(event.type == sf::Event::Closed) exit(0);
            else if(event.type == sf::Event::MouseButtonPressed) quit = true;
            else if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::Escape) quit = true;
                else if(event.key.code == sf::Keyboard::Return) quit = true;
            }
        }

        //
        m_Screen->draw(msgtxt);
        m_Screen->draw(clicktxt);

        m_Screen->display();
    }
}

sf::Sound *Blame::playSound(int soundindex)
{
    if(soundindex < 0 || soundindex >= int(m_Sounds.size()) ) return NULL;

    sf::Sound *newsound = new sf::Sound(m_Sounds[soundindex]);

    if(!newsound) return NULL;

    newsound->play();
    m_SoundQue.push_back(newsound);
}

void Blame::clearSoundQue()
{
    for(int i = 0; i < int(m_SoundQue.size()); i++)
    {
        if(m_SoundQue[i])
        {
            m_SoundQue[i]->stop();
            delete m_SoundQue[i];
        }
    }

    m_SoundQue.clear();
}
