#include "map.hpp"
#include "geometry.hpp"
#include "game_object.hpp"

Location::Location(const char *m, TSize w, TSize h) :
      map(w*h),
      width(w),
      height(h)
{
    const auto size = w*h;
    for(auto i = size;i--;map[i] = m[i]);
}


Actor *Location::trace(Vector2D origin, TReal direction, TReal d, Actor *actorToIgnore)
{
    const auto acos = std::cos(direction);
    const auto asin = std::sin(direction);

    for(float t = 0; t <= d; t+=.01f)
    {
        const float cx = origin.x() + t*acos;
        const float cy = origin.y() + t*asin;

        auto &list = GameObject::getRegister();
        for(auto &o : list)
        {
            auto actor = dynamic_cast<class Actor*>(o);
            if(actor == nullptr) continue;
            if(actor == actorToIgnore) continue;

            if(intersect({cx,cy}, actor->getCollisionBody()))
            {
                return actor;
            }
        }
    }
    return nullptr;
}

const char &Location::operator [](TSize pos) const
{
    return map[pos];
}

const Texture &Location::getWallText(TSize texId) const
{
    return textureDB[texId];
}

void Location::addWallText(PTexture txt)
{
    auto t = txt.lock();
    textureDB.push_back(*t);
}

void Location::addActor(Actor *actor)
{
    assert(actor != nullptr && "Actor is invalid");
    actor->location = this;
    actorList.push_back(actor);
}

void Location::removeActor(Actor *actor)
{
    assert(actor != nullptr && "Actor is invalid");
    actorList.remove(actor);
    actor->location = nullptr;
}

void Location::setMap(const char m[], TSize w, TSize h)
{
    map.resize(w*h);
    for(auto i=w;i--;)
        for(auto j=h;j--;)
            map[i+w*j] = m[i+w*j];
}

Texture Location::generateMinimap(Math::Vector2D<TSize> mapSize)
{
    const auto mapRectH = mapSize.y()/height;
    const auto mapRectW = mapSize.x()/width;
    Texture txt(mapSize.x(), mapSize.y());

    for(TSize j=0;j<height;++j)
        for(TSize i=0;i<width;++i)
        {
            const auto mapPos = i+j*width;
            if(map[mapPos] == ' ') continue;

            const TSize rectX = txt.w - (i+1)*mapRectW + 1;
            const TSize rectY = txt.h - (j+1)*mapRectH + 1;

            for(auto ki = 0;ki<mapRectW;++ki)
                for(auto kj = 0;kj<mapRectH;++kj)
                    txt.set(rectX-ki,rectY-kj,Color::Black);
        }

    return txt;
}

void World::addLocation(Location &&l, TString name)
{
    locationMap[name] = l;
}

Location &World::operator[](TString name)
{
    return locationMap[name];
}
