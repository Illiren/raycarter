#include "map.hpp"
#include "actor.hpp"
#include "geometry.hpp"

Map::Map(const char *map, TSize width, TSize height) :
      mapRef(map),
      _width(width),
      _height(height)
{}

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

            if(intersect({cx,cy}, actor->getCollisionRect()))
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

const Texture &Map::getTexture(TSize texId) const
{
    return n_textureDB[texId];
}


void Map::addTexture(PTexture txt)
{
    auto t = txt.lock();
    n_textureDB.push_back(*t);
}

