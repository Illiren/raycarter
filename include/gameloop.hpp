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
    inline int fps() const {return _fps;}

private:
    State _state;
    TReal _fpsLimit;
    std::thread _loopthread;
    int _fps;

    inline auto msPerUpdate() const { return TReal(1)/_fpsLimit;}
    void run();


protected:
    virtual void init() = 0;
    virtual void input() = 0;
    virtual void render() = 0;
    virtual void update(TReal lag) = 0;
};
