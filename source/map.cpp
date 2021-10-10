#include "map.hpp"
#include "actor.hpp"


Map::Map(const char *map, TSize width, TSize height, const TString &filename) :
      mapRef(map),
      _width(width),
      _height(height),
      _textureDB(filename,SDL_PIXELFORMAT_ABGR8888)
{}

bool intersection(Vector2D origin, Vector2D end, Vector2D apos, Vector2D rectsize)
{
    Vector2D hitPoint;

    Vector2D p1 = Vector2D(apos.x() - rectsize.x(), apos.y() - rectsize.y());
    Vector2D p2 = Vector2D(apos.x() + rectsize.x(), apos.y() - rectsize.y());
    Vector2D p3 = Vector2D(apos.x() + rectsize.x(), apos.y() + rectsize.y());
    Vector2D p4 = Vector2D(apos.x() - rectsize.x(), apos.y() + rectsize.y());

    Vector2D v1 = Math::normalize(p2-p1);
    Vector2D v2 = Math::normalize(p3-p2);
    Vector2D v3 = Math::normalize(p4-p3);
    Vector2D v4 = Math::normalize(p1-p4);


    return true;
}

class Actor *Map::trace(Vector2D origin, TReal agle, TReal d)
{
    const auto acos = std::cos(agle);
    const auto asin = std::sin(agle);

    const float cx = origin.x() + d*acos;
    const float cy = origin.y() + d*asin;

    auto &list = GameObject::getRegister();
    for(auto &o : list)
    {
        auto actor = dynamic_cast<class Actor*>(o);

        if(intersection(origin, {cx,cy}, actor->position(), actor->rectSize))
        {
            return actor;
        }
    }
    return nullptr;
}

TSize Map::width() const {return _width;}

TSize Map::height() const {return _height;}

const char &Map::operator [](TSize pos) const
{
    assert(mapRef != nullptr);
    return mapRef[pos];
}

char Map::operator[](TSize pos)
{
    assert(mapRef != nullptr);
    return mapRef[pos];
}


TArray<uint32_t> Map::getTexture(TSize texId, TSize texCoord, TSize height) const
{
    return _textureDB.getScaledColumn(texId,texCoord,height);
}

int Map::wall2texcoord(float hx, float hy) const
{
    float x = hx - floor(hx+.5);
    float y = hy - floor(hy+.5);

    int tex = x * _textureDB.size;
    if(abs(y) > abs(x))
        tex = y*_textureDB.size;
    if(tex < 0)
        tex+=_textureDB.size;
    return tex;
}

