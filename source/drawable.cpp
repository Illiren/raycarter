#include "drawable.hpp"

std::list<Drawable*> Drawable::_drawableRegister;

Drawable::Drawable() :
      dist(0)
{
    _drawableRegister.push_back(this);
}

Drawable::Drawable(TReal xx, TReal yy) :
      x(xx),
      y(yy),
      dist(0)
{
    _drawableRegister.push_back(this);
}

void Drawable::updateDist(TReal cx, TReal cy)
{
    dist = std::sqrt(pow(cx - x,2) + pow(cy - y, 2));
}

Drawable::~Drawable()
{
    _drawableRegister.remove(this);
}

const std::list<Drawable *> &Drawable::getRegister() {return _drawableRegister;}

void Drawable::sort()
{
    _drawableRegister.sort();
}

Sprite::Sprite(PTexture texture, Vector2D pos) :
      Drawable(pos.x(),pos.y()),
      tex(texture)
{}

Sprite::~Sprite()
{}

void Sprite::update()
{}

bool Drawable::operator < (const Drawable &s) const
{
    return dist < s.dist;
}

uint32_t Sprite::get(TSize i, TSize j, TSize spriteScreenSize) const
{
    assert(isValid(tex) && "texture is invalid");
    const auto p = tex.lock();
    return p->get(i*p->h/spriteScreenSize,j*p->w/spriteScreenSize);
}
