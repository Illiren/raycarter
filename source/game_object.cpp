#include "game_object.hpp"
#include <iostream>

GarbageCollector &GC()
{
    static GarbageCollector gc;
    return gc;
}

GameObject::GameObject()
{
    //static TSize i = 0; //Bad UID
    //It be cool, if c++ has reflection
    name = "Object " + std::to_string(TSize(this));//std::to_string(i++);
    //object and its address
}

void *GameObject::operator new(std::size_t count, GameObject *parent)
{
    auto ptr = ::operator new(count);
    auto &gc = GC();
    if(parent)
    {
        parent->children.push_back((GameObject*)ptr);
        gc.heap.push_back((GameObject*)ptr);
    }
    else
    {
        gc.roots.push_back((GameObject*)ptr);
    }

    return ptr;
}

void GameObject::operator delete(void *ptr)
{
    auto &gc = GC();

    gc.roots.remove((GameObject*)ptr);
    gc.heap.remove((GameObject*)ptr);

    ::operator delete(ptr);
    ptr = nullptr;
}

void GameObject::mark()
{
    _isMarked = true;
    markChildren();
}

void GameObject::markChildren()
{
    for(auto &child : children)
        if(!child->_isRemove)
            child->mark();
        else
            children.remove(child);
}


GameObject::~GameObject()
{}



void GarbageCollector::update(TReal dt)
{
    for(auto &o : roots)
        o->update(dt);

    for(auto &o : heap)
        o->update(dt);
}

void GarbageCollector::collect(bool verbose)
{
    mark(verbose);
    sweep(verbose);
}

void GarbageCollector::mark(bool verbose)
{
    using namespace std;
    for(auto &o : roots)
        o->mark();

    if(verbose)
    {
        cout << "Roots: " << roots.size() << " roots" << endl;
        cout << "Heap: " << heap.size() << " objects in heap" << endl;
    }
}

void GarbageCollector::sweep(bool verbose)
{
    using namespace std;
    unsigned int live = 0,
                 total = 0;

    TArray<TList<GameObject*>::iterator> toErase;
    const auto heapEnd = heap.end();
    for(auto it = heap.begin(); it!=heapEnd; ++it)
    {
        GameObject *p = *it;
        total++;
        if(p->_isMarked)
        {
            p->_isMarked = false;
            ++live;
        }
        else
            toErase.push_back(it);
    }

    auto dead = toErase.size();
    const auto eraseEnd = toErase.end();
    for(auto it = toErase.begin(); it != eraseEnd; ++it)
    {
        GameObject *p = **it;
        heap.erase(*it);
        delete p;
    }

    if(verbose)
    {
        cout << "GC: " << live << " objects survive after sweep" << endl;
        cout << "GC: " << dead << " objects died after sweep" << endl;
        cout << "GC: " << total << " total objects sweeped" << endl;
    }
}
