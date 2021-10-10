#include <iostream>
#include "raycaster.hpp"
#include "audio.hpp"


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


struct Drake : public Actor
{
    Drake(Audio &ad) :
          Actor(),
          a(ad)
    {}

    ~Drake() = default;

    void collision(Actor *) override {}

    void interract(Actor *) override
    {
        cout << "Drake says: Hello" << endl;
        new std::thread([this](){a.play();}); //Warning: Memory leak

    }

    std::thread t;
    Audio &a;
};

int main()
{
    Audio a;
    a.load("test.wav");

    bool debug = true;
    const size_t winW = 768;
    const size_t winH = 768;

    Sprite e1("monsters.bmp", 3.523, 3.812, 2);
    Drake a1(a);
    a1.drawable = &e1;

    Sprite e2("monsters.bmp", 3.523, 13.812, 1);
    Actor a2;
    a2.drawable = &e2;

    Sprite e3("monsters.bmp", 12.0, 12.0, 0);
    Actor a3;
    a3.drawable = &e3;

    RayCarter game({{0,0},{winW,winH}});

    GetInputManager()[SDLK_ESCAPE].keydownEvent = [&game,&debug](){debug = false; game.stop();};
    game.start();

    GetScreen().clear();
    GetScreen().update();

    while(debug)
        GetInputManager().update();

    return 0;
}
