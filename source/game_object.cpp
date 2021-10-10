#include "game_object.hpp"

std::list<GameObject*> GameObject::_objectRegister;

GameObject::GameObject()
{
    static TSize i = 0; //Bad UID
    name = "Object " + std::to_string(i++);
    _objectRegister.push_back(this);
}

GameObject::~GameObject()
{
    _objectRegister.remove(this);
}
