#include "tools.hpp"

#include <iostream>

#include <math.h>
#include <time.h>

#include <dirent.h> // for directory / files
#include <sys/types.h>
#include <sys/stat.h>

float getMagnitude(sf::Vector2f tvec)
{
    return sqrt( tvec.x*tvec.x + tvec.y*tvec.y);
}

sf::Vector2f normalizeVector(sf::Vector2f tvec)
{
    float mag = getMagnitude(tvec);

    return sf::Vector2f( tvec.x / mag, tvec.y / mag);
}

int rollDice(int num, int sides, int modifier)
{
    int total = 0;

    if( num <= 0 || sides <= 0) return 0;

    for(int i = 0; i < sides; i++)
    {
        total += rand()%sides + 1;
    }

    return total + modifier;

}

bool randBool()
{
    return rand()%2;
}

void drawFloatRect(sf::FloatRect trect, sf::RenderTarget *tscreen)
{
    sf::RectangleShape rbox(sf::Vector2f( trect.width-2, trect.height-2));
    rbox.setOutlineThickness(1);
    rbox.setFillColor(sf::Color::Transparent);
    rbox.setOutlineColor(sf::Color::Green);
    rbox.setPosition(trect.left+1, trect.top+1);

    tscreen->draw(rbox);

}

bool lineIntersectsRect(sf::Vector2f p1, sf::Vector2f p2, sf::FloatRect trect, std::vector<sf::Vector2f> *ipoints)
{
    if( hasIntersection(p1.x, p1.y, p2.x, p2.y, trect.left, trect.top, trect.left + trect.width, trect.top))
        return true;
    else if( hasIntersection(p1.x, p1.y, p2.x, p2.y, trect.left + trect.width, trect.top, trect.left + trect.width, trect.top + trect.height))
        return true;
    else if( hasIntersection(p1.x, p1.y, p2.x, p2.y, trect.left, trect.top + trect.height, trect.left + trect.width, trect.top+trect.height))
        return true;
    else if( hasIntersection(p1.x, p1.y, p2.x, p2.y, trect.left, trect.top, trect.left, trect.top+trect.height))
        return true;

    // no intersection found
    return false;
}

float getDistance(sf::Vector2f p1, sf::Vector2f p2)
{
    return sqrt( (p2.x-p1.x)*(p2.x-p1.x) + (p2.y-p1.y)*(p2.y-p1.y) );
}

// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines
// intersect the intersection point may be stored in the floats i_x and i_y.
bool hasIntersection(float p0_x, float p0_y, float p1_x, float p1_y,
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return true;
    }

    return false; // No collision
}

//////////////////////////////////////////////////////////////////
// FILE TOOLS

std::vector<std::string> csvParse(std::string pstring, char delim)
{
    std::vector<std::string> parsedStrings;

    //don't use " as delimiter, bad things will happen, detect this and return a blank list
    if(delim == '"') return parsedStrings;

    size_t lpos = 0; //last position
    size_t qpos = 0; //quote position

    bool inquotes = false;
    std::string currentstring;

    //walk through each character in string
    for(int i = 0; i < int(pstring.length()); i++)
    {
        //if character is a the delim, stop current string, add to list, and start new string
        if(pstring[i] == delim && !inquotes)
        {
            parsedStrings.push_back(currentstring);

            currentstring = "";

            lpos = i;
        }
        //are we at the end of the string?
        else if(i == int(pstring.length()-1))
        {
            //if last character is not quote
            if(pstring[i] != '"') currentstring.push_back( pstring[i]);
            parsedStrings.push_back(currentstring);
        }
        //if entering a cell in quotes
        else if(pstring[i] == '"')
        {
            if(!inquotes)
            {
                inquotes = true;
                if(pstring[i+1] == '"') currentstring.push_back('"');
            }
            else
            {
                //if next character is a quote (double quotes) add to string
                if(pstring[i+1] == '"') currentstring.push_back('"');
                //else if next character is the delimiter
                else if(pstring[i+1] == delim)
                {
                    inquotes = false;
                }
            }

        }
        else currentstring.push_back(pstring[i]);

    }


    return parsedStrings;

}

std::vector<std::string> getFiles(std::string directory, std::string extension)
{
    //create file list vector
    std::vector<std::string> fileList;

    //create directory pointer and directory entry struct
    DIR *dir;
    struct dirent *ent;
    struct stat st;

    //if able to open directory
    if ( (dir = opendir (directory.c_str() ) ) != NULL)
    {
      //go through each file in directory
      while ((ent = readdir (dir)) != NULL)
      {
            //convert read in file to a string
            std::string filename(ent->d_name);

            if(filename != "." && filename != "..")
            {

                //check that entry is a directory
                stat(std::string(directory+filename).c_str(), &st);
                if(!S_ISDIR(st.st_mode))
                {
                    //check that file matches given extension parameter
                    //if an extension is provided, make sure it matches
                    if(extension != "")
                    {
                        //find location of extension identifier '.'
                        size_t trim = filename.find_last_of('.');
                        if(trim > 200) std::cout << "ERROR:getFiles extension is invalid\n";
                        else
                        {
                            std::string target_extension = filename.substr(trim);

                            if(target_extension == extension) fileList.push_back(filename);
                            //else std::cout << "Filename:" << filename << " does not match extension parameter - ignoring\n";
                        }
                    }
                    else fileList.push_back(filename);
                }

            }

      }

      closedir (dir);
    }
    //else failed to open directory
    else
    {
      std::cout << "Failed to open directory:" << directory << std::endl;
      //return empty file list
      return fileList;
    }

    return fileList;
}
