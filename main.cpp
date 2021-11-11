#include <iostream>
#include "raycaster.hpp"
#include "audio.hpp"
#include "network.hpp"
#include <thread>


using namespace std;


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

    TextureDB test;
    test.load("monsters.bmp",4,"monster");
    test.load("walltext.bmp",6,"wall");
    test.load("player.bmp", "player");
    test.load("weapon.bmp", "weapon");
    test.load("dwarf.bmp", "dwarf");
    test.load("orc.bmp", "orc");
    test.load("troll.bmp", "troll");
    autiodb.load("test.wav", "drakesay1");
    auto sound = autiodb["drakesay1"];

    Drake a1(sound);
    a1.drawable.reset(new Sprite(test["monster2"], {3.523, 3.812}));

    Actor a2;
    a2.drawable.reset(new Sprite(test["orc"], {3.523, 13.812}));

    Actor a3;
    a3.drawable.reset(new Sprite(test["troll"], {12.0, 12.0}));

    Actor a4;
    a4.drawable.reset(new Sprite(test["monster3"],{6.42, 10.5}));

    RayCarter game({{0,0},{winW,winH}});
    game._player.face = test["player"];
    game._player.weapontext = test["weapon"];

    for(TSize i=0;i<6;++i)
        game.map().addTexture(test["wall"+std::to_string(i)]);


    GetInputManager()[SDLK_ESCAPE].keydownEvent = [&game,&debug](){debug = false; game.stop();};
    GetInputManager()['p'].keydownEvent = [&game,&debug](){game.pause();};
    //GetInputManager()['1'].keydownEvent = [&c](){c.pingServer();};
    //GetInputManager()['2'].keydownEvent = [&c](){c.messageAll();};
    game.start();

    while(debug)
    {
        //GetScreen().clear();
        //GetScreen().update();
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



