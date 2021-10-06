#pragma once

#include "window.hpp"
#include "color.hpp"
#include "screen.hpp"
#include "map.hpp"
#include "camera.hpp"
#include "gameloop.hpp"
#include "input.hpp"
#include "player.hpp"

class RayCarter : public Window, public GameLoop
{
public:
    RayCarter(Rectangle winSize, TReal fpsLimit = 60.f, Window *parent = nullptr);
    ~RayCarter() override {}


private:
    Screen &screen;
    InputManager &keyMap;

    Camera _camera;
    Map _map;
    Player _player;

    Point2D _mapOrigin;
    Point2D _mapSize;


    constexpr static TSize mapW = 16;
    constexpr static TSize mapH = 16;
    static const char mapdata[];

private:
    void draw() override;

    // GameLoop interface
protected:
    void input() override;
    void render() override;
    void update(TReal lag) override;
};
