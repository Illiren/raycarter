#pragma once

#include "stddefines.hpp"
#include <list>

class GarbageCollector
{
public:
    void collect(bool verbose); //Collect garbage (mark and sweep). Set verbose for verbose output to std::cout
    void update(TReal dt); //Update every registred object

    friend GarbageCollector &GC();
    friend class GameObject;

private:
    void mark(bool verbose);
    void sweep(bool verbose);

    GarbageCollector() {}

    std::list<class GameObject*> roots;
    std::list<class GameObject*> heap;
};

GarbageCollector &GC();

//Game object is object registred in GC
class GameObject
{
public:
    GameObject();
    virtual ~GameObject();

    void *operator new(std::size_t count, GameObject *parent = nullptr); //If object created without parent, then it considered as root
    void operator delete(void *ptr); //remove object from gc list

    virtual void update(TReal dt) = 0;

    TString name; //Unique name of object?
    bool isTickable = true;

    friend class GarbageCollector;

private:
    bool _isMarked = false;
    bool _isRemove = false;
    void mark();
    void markChildren();

    std::list<GameObject*> children;

    friend class GC;
};






