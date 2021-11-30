#include "drawable.hpp"

std::list<Drawable*> Drawable::_drawableRegister;

Drawable::Drawable() :
      dist(0)
{
    _drawableRegister.push_back(this);
}

Drawable::Drawable(Vector2D position) :
      pos(position),
      dist(0)
{
    _drawableRegister.push_back(this);
}

void Drawable::updateDist(Vector2D r)
{
    dist = std::sqrt(pow(r.x() - pos.x(), 2) + pow(r.y() - pos.y(), 2));
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
      Drawable(pos),
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
