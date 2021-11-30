#include <iostream>
#include "raycaster.hpp"
#include "audio.hpp"
#include "network.hpp"
#include <thread>


using namespace std;



const char mapdata[] = "2222222222222222"\
                       "2              2"\
                       "2     444444   2"\
                       "2     4        2"\
                       "2     4        2"\
                       "2     3        2"\
                       "2   44444      2"\
                       "2   4   2      2"\
                       "2   4   2      2"\
                       "2   4   2      2"\
                       "2       2      2"\
                       "22222   2      2"\
                       "2       2      2"\
                       "2    2222222   2"\
                       "2              2"\
                       "2222222222222222";






int main()
{
    bool debug = true;
    const size_t winW = 768;
    const size_t winH = 768;

    TextureDB test;
    test.load("walltext.bmp",6,"wall");

    RayCarter game({{0,0},{winW,winH}});
    Location loc(mapdata,16,16);
    for(TSize i=0;i<6;++i)
        loc.addWallText(test["wall"+std::to_string(i)]);
    loc.floor = *test["wall1"].lock().get();
    loc.ceil = *test["wall0"].lock().get();

    game.addLocation(std::move(loc),"level0");

    GetInputManager()[SDLK_ESCAPE].keydownEvent = [&game,&debug](){debug = false; game.stop();};
    GetInputManager()['p'].keydownEvent = [&game,&debug](){game.pause();};
    //GetInputManager()['1'].keydownEvent = [&c](){c.pingServer();};
    //GetInputManager()['2'].keydownEvent = [&c](){c.messageAll();};
    game.start(RayCarter::Singleplayer);

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



