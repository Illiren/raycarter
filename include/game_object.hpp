#pragma once

#include "stddefines.hpp"
#include <list>


struct GarbageCollector
{
    void collect(bool verbose);
    void update(TReal dt);


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


class GameObject
{
public:
    GameObject();
    virtual ~GameObject();

    void *operator new(std::size_t count, GameObject *parent = nullptr);
    void operator delete(void *ptr);

    virtual void update(TReal dt) = 0;
    bool isValid() const noexcept {return this != nullptr;}

    TString name;
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






