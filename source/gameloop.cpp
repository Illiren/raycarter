#include "gameloop.hpp"
#include <chrono>
#include <iostream>

void GameLoop::run()
{
    using namespace std;

    auto previous = chrono::steady_clock::now();
    using TimeMS = chrono::duration<float>;

    float lag = 0.0;
    float time = 0.f;
    int fps = 0.f;

    cout << "Starting..." << endl;

    while(_state)
    {
        auto currentTime = chrono::steady_clock::now();
        auto elapsed = chrono::duration<float,milli>(currentTime - previous);
        previous = currentTime;

        time += elapsed.count();
        ++fps;
        lag += elapsed.count();

        {
            auto start = chrono::steady_clock::now();
            input();
            auto end = chrono::steady_clock::now();
            auto diff = end - start;
            //cout << "Input time: " << chrono::duration<float, milli>(diff).count() << " ms" << endl;
        }

        if(_state != Pause)
        {
            auto start = chrono::steady_clock::now();
            while(lag >= msPerUpdate())
            {
                lag-=msPerUpdate();
                update(msPerUpdate());
            }
            auto end = chrono::steady_clock::now();
            auto diff = end - start;
            //cout << "Update time: " << chrono::duration<float, milli>(diff).count() << " ms" << endl;
        }

        {
            auto start = chrono::steady_clock::now();
            render();
            auto end = chrono::steady_clock::now();
            auto diff = end - start;
            //cout << "Render time: " << chrono::duration<float, milli>(diff).count() << " ms" << endl;
        }

        auto endOfFrame = chrono::steady_clock::now();

        auto freeTime = TimeMS(msPerUpdate()) - (endOfFrame - currentTime);

        this_thread::sleep_for(freeTime);

        if(time >= 1000)
        {
            cout << "FPS: " << fps << endl;
            fps = 0;
            time = 0;
        }
    }

    cout << "Ending " << endl;
}


GameLoop::GameLoop(TReal fpsLimit) :
    _state(Idle),
    _fpsLimit(fpsLimit)
{}

void GameLoop::start()
{
    if(_state)
        return;
    _state = Running;
    run();

    //_loopthread = std::thread([a = this]{a->run();});
    //_loopthread.join();
}

void GameLoop::stop()
{
    _state = Idle;
}
