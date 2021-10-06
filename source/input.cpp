#include "input.hpp"

InputManager &GetInputManager()
{
    static InputManager inptmgr;
    return inptmgr;
}
