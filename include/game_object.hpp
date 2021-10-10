#pragma once

#include "typedefines.hpp"
#include "stddefines.hpp"
#include <list>


class GameObject
{
public:
     GameObject();
     virtual ~GameObject();

     virtual void update(TReal dt) = 0;

     static const std::list<GameObject*> &getRegister() {return _objectRegister;}

     TString name;


private:
     static std::list<GameObject*> _objectRegister;
};
