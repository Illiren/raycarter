#pragma once

#include <functional>
#include "stddefines.hpp"
#include "SDL.h"


class InputManager
{

private:
    InputManager() = default;

public:
    using Functor = std::function<void ()>;

    struct KeyboardEvent
    {
        Functor keydownEvent;
        Functor keyupEvent;
    };

    using TKeyMap = THashMap<int,KeyboardEvent>;

public:
    friend InputManager &GetInputManager();
    KeyboardEvent &operator[](int a);

    void update();

private:
    TKeyMap _keymap;
};

InputManager &GetInputManager();


