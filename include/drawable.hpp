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

    static const std::list<Drawable*> &getRegister();

    void updateDist(TReal x, TReal y);

    TReal dist;
    TReal x,y;

private:
    static std::list<Drawable*> _drawableRegister;
};


struct Sprite : public Drawable
{
    Sprite(PTexture texture, Vector2D pos);
    ~Sprite() override;

    void update() override;
    bool operator<(const Sprite &s) const;
    uint32_t get(TSize i, TSize j, TSize spriteScreenSize) const;

    PTexture tex;
};
