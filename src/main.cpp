#include <cstdlib>

#include "blame.hpp"

int main(int argc, char *argv[])
{

    Blame *blame;

    // init singleton of game
    blame = Blame::getInstance();

    return 0;
}
