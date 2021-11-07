#include <iostream>
#include "raycaster.hpp"
#include "audio.hpp"
#include "network.hpp"
#include <thread>


using namespace std;

const char RayCarter::mapdata[] =   "0000000000000000"\
                                    "0              0"\
                                    "0     444      0"\
                                    "0     4        0"\
                                    "0     4        0"\
                                    "0     3        0"\
                                    "0   44444      0"\
                                    "0   4   2      0"\
                                    "0   4   2      0"\
                                    "0   4   2      0"\
                                    "0       2      0"\
                                    "05555   2      0"\
                                    "0       2      0"\
                                    "0    2222      0"\
                                    "0              0"\
                                    "0000000000000000";

struct Drake : public Actor
{
    Drake(PSound &ad) :
          Actor(),
          s(ad)
    {}

    ~Drake() = default;

    void collision(Actor *) override {}

    void interract(Actor *) override
    {
        cout << "Drake says: Hello" << endl;

        s.play();
    }

    SoundSequence s;
};






int main()
{
    //CustomServer sever(1234);
    //sever.start();

    //CustomClient c;
    //c.connect("192.168.1.96",1234);


    AudioDB autiodb;

    bool debug = true;
    const size_t winW = 768;
    const size_t winH = 768;

    NTextureDB test;
    test.load("monsters.bmp",4,"monster");
    test.load("walltext.bmp",6,"wall");
    test.load("player.bmp", "player");
    test.load("weapon.bmp", "weapon");
    test.loadFont("smallfont.bmp");
    autiodb.load("test.wav", "drakesay1");
    auto sound = autiodb["drakesay1"];

    Sprite e1("monsters.bmp", 3.523, 3.812, 2);
    Drake a1(sound);
    a1.drawable = &e1;

    Sprite e2("monsters.bmp", 3.523, 13.812, 1);
    Actor a2;
    a2.drawable = &e2;

    Sprite e3("monsters.bmp", 12.0, 12.0, 0);
    Actor a3;
    a3.drawable = &e3;

    NSprite n1(test["monster2"],{6.42, 10.5});
    Actor a4;
    a4.drawable = &n1;

    RayCarter game({{0,0},{winW,winH}});
    game._player.face = test["player"];
    game._player.weapontext = test["weapon"];

    GetInputManager()[SDLK_ESCAPE].keydownEvent = [&game,&debug](){debug = false; game.stop();};
    GetInputManager()['p'].keydownEvent = [&game,&debug](){game.pause();};
    //GetInputManager()['1'].keydownEvent = [&c](){c.pingServer();};
    //GetInputManager()['2'].keydownEvent = [&c](){c.messageAll();};
    game.start();

    while(debug)
    {
        //GetScreen().clear();
        GetScreen().update();
        this_thread::sleep_for(1s);
    }
/*
    while(debug)
    {
        GetInputManager().update();
        sever.update(-1,false);

        if(c.isConnected())
        {
            if(!c.incoming().empty())
            {
                auto msg = c.incoming().pop_front().msg;
                switch (msg.header.id)
                {
                case CustomMsgTypes::ServerAccept:
                    std::cout << "Server Accepted Connection\n";
                    break;
                case CustomMsgTypes::ServerPing:
                {
                    std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                    std::chrono::system_clock::time_point timeThen;
                    msg >> timeThen;
                    std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
                }
                break;
                case CustomMsgTypes::ServerMessage:
                {
                    uint32_t clientID;
                    msg >> clientID;
                    std::cout << "Hello from [" << clientID << "]\n";
                }
                break;
                default:
                    break;
                }
            }
        }
    }*/

    return 0;
}



