#pragma once

#include "stddefines.hpp"
#include "texture.hpp"
#include <list>


class Drawable
{
public:
    Drawable();
    Drawable(TReal x, TReal y);
    virtual ~Drawable();
    virtual void update() = 0;
    bool operator<(const Drawable &s) const;
    static const std::list<Drawable*> &getRegister();
    static void sort();

    void updateDist(TReal x, TReal y);

    TReal dist;
    TReal x,y;

    Vector2D pos;

private:
    static std::list<Drawable*> _drawableRegister;
};


struct Sprite : public Drawable
{
    Sprite(PTexture texture, Vector2D pos);
    ~Sprite() override;

    void update() override;

    uint32_t get(TSize i, TSize j, TSize spriteScreenSize) const;

    PTexture tex;
};
