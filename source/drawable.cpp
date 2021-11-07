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

Sprite::Sprite(TString fileName, TReal posx, TReal posy, TSize id) :
      Drawable(posx,posy),
      texid(id),
      tex(fileName, SDL_PIXELFORMAT_ABGR8888)
{}

Sprite::~Sprite()
{}

void Sprite::update()
{}

uint32_t Sprite::get(TSize i, TSize j, TSize spriteScreenSize) const
{
    return tex.get(i*tex.size/spriteScreenSize,j*tex.size/spriteScreenSize,texid);
}

bool Sprite::operator<(const Sprite &s) const
{
    return dist > s.dist;
}

NSprite::NSprite(PTexture texture, Vector2D pos) :
      Drawable(pos.x(),pos.y()),
      tex(texture)
{}

NSprite::~NSprite()
{}

void NSprite::update()
{}

bool NSprite::operator < (const NSprite &s) const
{
    return dist > s.dist;
}

uint32_t NSprite::get(TSize i, TSize j, TSize spriteScreenSize) const
{
    assert(isValid(tex) && "texture is invalid");
    const auto p = tex.lock();
    return p->get(i*p->h/spriteScreenSize,j*p->w/spriteScreenSize);
}
