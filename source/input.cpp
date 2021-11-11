#include "input.hpp"

InputManager &GetInputManager()
{
    static InputManager inptmgr;
    return inptmgr;
}

InputManager::KeyboardEvent &InputManager::operator[](int a)
{
    return _keymap[a];
}

void InputManager::update()
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
