#ifndef CLASS_PARTICLE
#define CLASS_PARTICLE

#include <SFML\Graphics.hpp>

#include <vector>

enum PARTICLETYPE{ PARTICLE_DEFAULT, PARTICLE_TINY};
enum EMITTERTYPE{ PEMIT_CUSTOM, PEMIT_DUST1};

struct Particle
{
    // linked list stuff
    Particle *next;
    Particle *prev;

    // particle graphics
    sf::Sprite *m_sprite;

    // particle params
    sf::Color m_color;
    sf::Time m_born;
    int m_life_time;

    // particle physics
    sf::Vector2f m_position;
    sf::Vector2f m_vel;
    sf::Vector2f m_accel;
};

class ParticleManager
{
private:

    // linked list pointers
    Particle *head;
    Particle *tail;

    // master clock
    sf::Clock m_clock;

    // texture list for different particle types
    std::vector<sf::Texture*> m_textures;

public:
    ParticleManager();
    ~ParticleManager();

    // linked list management
    int getCount();
    void addParticle(Particle *tparticle);
    void deleteParticle(Particle *tparticle);
    void clear();

    // get particle manager stuff
    sf::Texture *getTexture(int index);
    sf::Clock *getClock() {return &m_clock;}

    // draw and update particles
    void draw(sf::RenderTarget *tscreen);
    void update();


};

class ParticleEmitter
{
private:

    int m_type;
    sf::Vector2f m_position;

    ParticleManager *m_particle_manager;

public:
    ParticleEmitter(ParticleManager *p_manager, int ptype, sf::Vector2f ppos = sf::Vector2f(0,0) );
    ~ParticleEmitter();

    void createParticle( sf::Vector2f offsetpos, sf::Vector2f initialvel);
    void once();
    void setPosition( sf::Vector2f tpos);

    // custom emitter parameters
    sf::Color m_custom_color;
    int m_custom_texture_index;
    int m_custom_max_life;
    int m_custom_min_life;
    sf::Vector2f m_custom_accel;

};

#endif // CLASS_PARTICLE
