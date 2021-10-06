#pragma once

#include "camera.hpp"
#include "map.hpp"

struct Player
{
    enum WalkState
    {
        Idle = 0,
        Forward = -1,
        Back = 1,
        Left = -1,
        Right = 1
    };

    Player(Camera &camera, Map &map);

    char walk = 0;
    char turn = 0;
    float runningSpeed = 1;

    void update(float dt);

    Map &map;
    Camera &camera;
};
