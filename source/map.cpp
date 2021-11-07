#include "map.hpp"
#include "actor.hpp"


Map::Map(const char *map, TSize width, TSize height, const TString &filename) :
      mapRef(map),
      _width(width),
      _height(height),
      _textureDB(filename,SDL_PIXELFORMAT_ABGR8888)
{}

bool intersection(Vector2D point, Vector2D apos, Vector2D rectsize)
{
    Vector2D p1 = Vector2D(apos.x() - rectsize.x(), apos.y() - rectsize.y());
    Vector2D p2 = Vector2D(apos.x() + rectsize.x(), apos.y() - rectsize.y());
    Vector2D p3 = Vector2D(apos.x() + rectsize.x(), apos.y() + rectsize.y());
    Vector2D p4 = Vector2D(apos.x() - rectsize.x(), apos.y() + rectsize.y());
//---------- 1
    Vector2D min = p1;
    Vector2D max = p1;
//---------- 2
    if(p2.x() < min.x())
        min.x() = p2.x();
    else if(p2.x() > max.x())
        max.x() = p2.x();

    if(p2.y() < min.y())
        min.y() = p2.y();
    else if(p2.y() > max.y())
        max.y() = p2.y();
//---------- 3
    if(p3.x() < min.x())
        min.x() = p3.x();
    else if(p3.x() > max.x())
        max.x() = p3.x();

    if(p3.y() < min.y())
        min.y() = p3.y();
    else if(p3.y() > max.y())
        max.y() = p3.y();
//---------- 4
    if(p4.x() < min.x())
        min.x() = p4.x();
    else if(p4.x() > max.x())
        max.x() = p4.x();

    if(p4.y() < min.y())
        min.y() = p4.y();
    else if(p4.y() > max.y())
        max.y() = p4.y();

//---------- Check

    if(point.x() > min.x() && point.x() < max.x() &&
        point.y() > min.y() && point.y() < max.y())
        return true;

    return false;
}

Actor *Map::trace(Vector2D origin, TReal agle, TReal d, Actor *actorToIgnore=nullptr)
{
    const auto acos = std::cos(agle);
    const auto asin = std::sin(agle);

    for(float t = 0; t <= d; t+=.01f)
    {
        const float cx = origin.x() + t*acos;
        const float cy = origin.y() + t*asin;

        auto &list = GameObject::getRegister();
        for(auto &o : list)
        {
            auto actor = dynamic_cast<class Actor*>(o);
            if(actor == actorToIgnore) continue;

            if(intersection({cx,cy}, actor->position(), actor->rectSize))
            {
                return actor;
            }
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

