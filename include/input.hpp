#ifndef INPUT_HPP
#define INPUT_HPP

#include <functional>
#include "typedefines.hpp"
#include "stddefines.hpp"
#include "SDL.h"


class InputManager
{

private:
    InputManager()
    {}

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

    KeyboardEvent &operator[](int a)
    {
        return _keymap[a];
    }

    void update()
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(SDL_KEYUP==event.type)
                for(const auto &i : _keymap)
                    if(i.first == event.key.keysym.sym)
                        if(i.second.keyupEvent)
                            i.second.keyupEvent();

            if(SDL_KEYDOWN==event.type)
                for(const auto &i : _keymap)
                    if(i.first == event.key.keysym.sym)
                        if(i.second.keydownEvent)
                            i.second.keydownEvent();
        }
    }

private:
    TKeyMap _keymap;
};

InputManager &GetInputManager();


#endif //INPUT_HPP
