#pragma once

#include "typedefines.hpp"
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

    static const std::list<Drawable*> &getRegister() {return _drawableRegister;}

    void updateDist(TReal x, TReal y);

    TReal dist;
    TReal x,y;
private:
    static std::list<Drawable*> _drawableRegister;
};


struct Sprite : public Drawable
{
    Sprite(TString fileName, TReal posx, TReal posy, TSize texid = 0);
    ~Sprite() override;

    void update() override;
    uint32_t get(TSize i, TSize j, TSize spriteScreenSize) const;


    bool operator<(const Sprite &s) const;

    TSize texid;
    Texture tex;
};
