#pragma once

#include <thread>
#include <functional>
#include "geometry.hpp"

template<typename Base>
class GameLoop
{
public:
    enum State
    {
        Idle = 0,
        Running = 1,
        Pause = 2,
    };

    GameLoop()
    {
        static_assert(true, "Wrong use of the class. Inheritance is needed");
    }

protected:
    GameLoop(TReal fpsimit = 60.f) :
          _state(Idle),
          _fpsLimit(fpsimit)
    {}
    ~GameLoop()
    { }

public:
    void start()
    {
        if(_state)
            return;
        _state = Running;
        static_cast<Base*>(this)->init();
        run();
    }
    void pause()
    {
        if(_state == Running)
            _state = Pause;
        else if (_state == Pause)
            _state = Running;
    }
    void stop()
    {
        _state = Idle;
    }
    inline State currentState() const noexcept {return _state;}
    int fps() const noexcept { return _fps; }

private:
    State _state;
    TReal _fpsLimit;
    int _fps;

    TReal msPerUpdate() const noexcept { return TReal(1)/_fpsLimit; }

    void run()
    {
        using namespace std;

        auto previous = chrono::steady_clock::now();
        using TimeMS = chrono::duration<float>;

        float lag = 0.f;
        float time = 0.f;
        float fps = 0.f;

        while(_state)
        {
            auto currentTime = chrono::steady_clock::now();
            auto elapsed = chrono::duration<float,milli>(currentTime - previous);
            previous = currentTime;

            time += elapsed.count();
            ++fps;
            lag=elapsed.count();

            static_cast<Base*>(this)->input();

            if(_state != Pause)
            {
                static_cast<Base*>(this)->update(lag);
                /*while(lag>0.f) //More correct way
                {
                    static_cast<Base*>(this)->update(msPerUpdate());
                    lag-=msPerUpdate();
                }*/
            }

            static_cast<Base*>(this)->render();

            auto endOfFrame = chrono::steady_clock::now();

            auto freeTime = TimeMS(msPerUpdate()) - TimeMS(endOfFrame - currentTime);
            this_thread::sleep_for(freeTime);

            if(time >= 1000)
            {
                if(printEvent) printEvent();
                _fps = fps;
                fps = 0;
                time = 0;
            }
        }
    }


protected:
    std::function<void ()> printEvent;
};
