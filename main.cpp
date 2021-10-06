#include <iostream>
#include "raycaster.hpp"


using namespace std;

const char RayCarter::mapdata[] =   "0000000000000000"\
                                    "0              0"\
                                    "0     444444   0"\
                                    "0     4        0"\
                                    "0     4  6666660"\
                                    "0     3        0"\
                                    "0   44444      0"\
                                    "0   4   11100  0"\
                                    "0   4   2      0"\
                                    "0   4   2  44440"\
                                    "0       2      0"\
                                    "05555   2      0"\
                                    "0       2      0"\
                                    "0    2222222   0"\
                                    "0              0"\
                                    "0000000000000000";

int main()
{
    bool debug = true;
    const size_t winW = 768;
    const size_t winH = 768;

    RayCarter game({{0,0},{winW,winH}});

    GetInputManager()[SDLK_ESCAPE].keydownEvent = [&game,&debug](){debug = false; game.stop();};
    game.start();

    GetScreen().clear();
    GetScreen().update();

    while(debug)
        GetInputManager().update();

    return 0;
}
