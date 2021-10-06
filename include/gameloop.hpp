#pragma once

#include <thread>
#include <functional>
#include "geometry.hpp"


class GameLoop
{
public:
    enum State
    {
        Idle = 0,
        Running = 1,
        Pause = 2,
    };

public:
    GameLoop(TReal fpsimit = 60.f);

    void start();
    void pause();
    void stop();


private:
    State _state;
    TReal _fpsLimit;
    std::thread _loopthread;

    inline auto msPerUpdate() const { return TReal(1)/_fpsLimit;}
    void run();

protected:
    virtual void input() = 0;
    virtual void render() = 0;
    virtual void update(TReal lag) = 0;
};
