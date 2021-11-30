#pragma once

#include <thread>
#include <functional>
#include "geometry.hpp"

//Idea: static polymorphism for render, input, update. CRTP maybe


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
    virtual ~GameLoop();

    void start();
    void pause();
    void stop();
    inline State currentState() const noexcept {return _state;}
    int fps() const noexcept;

private:
    State _state;
    TReal _fpsLimit;
    std::thread _loopthread;
    int _fps;

    TReal msPerUpdate() const noexcept;
    void run();


protected:
    virtual void init() = 0;
    virtual void input() = 0;
    virtual void render() = 0;
    virtual void update(TReal lag) = 0;
    std::function<void ()> printEvent;
};
