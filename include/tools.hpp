#ifndef __TOOLS
#define __TOOLS

#include <SFML\Graphics.hpp>



// vector stuff
float getMagnitude(sf::Vector2f tvec);
sf::Vector2f normalizeVector(sf::Vector2f tvec);


bool hasIntersection(float p0_x, float p0_y, float p1_x, float p1_y,
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x = NULL, float *i_y = NULL);
bool lineIntersectsRect(sf::Vector2f p1, sf::Vector2f p2, sf::FloatRect trect, std::vector<sf::Vector2f> *ipoints = NULL);

float getDistance(sf::Vector2f p1, sf::Vector2f p2);


// misc tools
int rollDice(int num, int sides, int modifier);
bool randBool();
void drawFloatRect(sf::FloatRect trect, sf::RenderTarget *tscreen);


// file tools
std::vector<std::string> getFiles(std::string directory, std::string extension = "");
std::vector<std::string> csvParse(std::string pstring, char delim = ',');

#endif // CLASS_TOOLS
