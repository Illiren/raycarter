#pragma once

#include "stddefines.hpp"
#include "texture.hpp"
#include <list>


class Drawable
{
public:
    Drawable();
    Drawable(Vector2D position);
    virtual ~Drawable();
    virtual void update() = 0;
    bool operator<(const Drawable &s) const;
    static const std::list<Drawable*> &getRegister();
    static void sort();

    void updateDist(Vector2D relPos);

    Vector2D pos;
    TReal dist;    

private:
    static std::list<Drawable*> _drawableRegister;
};


struct Sprite : public Drawable
{
    Sprite(PTexture texture, Vector2D pos);
    Sprite(Vector2D pos);
    ~Sprite() override;

    void update() override;

    uint32_t get(Vector2U pos, TSize spriteScreenSize) const;

    PTexture tex;
};
