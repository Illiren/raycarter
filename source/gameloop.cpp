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
    float fps = 0.f;

    cout << "Starting..." << endl;

    while(_state)
    {
        auto currentTime = chrono::steady_clock::now();
        auto elapsed = chrono::duration<float,milli>(currentTime - previous);
        previous = currentTime;

        time += elapsed.count();
        ++fps;
        lag=elapsed.count();
        //lag += elapsed.count();

        {
            auto start = chrono::steady_clock::now();
            this->input();
            auto end = chrono::steady_clock::now();
            auto diff = end - start;
            //cout << "Input time: " << chrono::duration<float, milli>(diff).count() << " ms" << endl;
        }


        {
            auto start = chrono::steady_clock::now();
            /*while(lag >= msPerUpdate())
            {
                lag-=msPerUpdate();
                if(_state != Pause)
                    update(msPerUpdate());
            }*/

            if(_state != Pause)
                this->update(lag);

            auto end = chrono::steady_clock::now();
            auto diff = end - start;
            //cout << "Update time: " << chrono::duration<float, milli>(diff).count() << " ms" << endl;
        }

        {
            auto start = chrono::steady_clock::now();
            this->render();
            auto end = chrono::steady_clock::now();
            auto diff = end - start;
            //cout << "Render time: " << chrono::duration<float, milli>(diff).count() << " ms" << endl;
        }

        auto endOfFrame = chrono::steady_clock::now();

        auto freeTime = TimeMS(msPerUpdate()) - TimeMS(endOfFrame - currentTime);
        this_thread::sleep_for(freeTime);

        //cout << "Start time " << TimeMS(currentTime).count() << '\n';
        //cout << "End time " << duration_cast(endOfFrame).count() << '\n';

        if(time >= 1000)
        {
            cout << "Sleeping for " << freeTime.count() << '\n';
            cout << "FPS: " << fps << endl;
            if(printEvent) printEvent();
            _fps = fps;
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

GameLoop::~GameLoop()
{
    //_loopthread.join();
}

void GameLoop::start()
{
    if(_state)
        return;
    _state = Running;
    init();
    run();
    //_loopthread = std::thread([this]() {this->run();});
    //_loopthread.join();
}

void GameLoop::pause()
{
    if(_state == Running)
        _state = Pause;
    else if (_state == Pause)
        _state = Running;
}

void GameLoop::stop()
{
    _state = Idle;

}

int GameLoop::fps() const noexcept
{
    return _fps;
}

TReal GameLoop::msPerUpdate() const noexcept
{
    return TReal(1)/_fpsLimit;
}
