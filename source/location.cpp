#include "location.hpp"
#include "geometry.hpp"
#include "actor.hpp"

Location::Location(const char *m, TSize w, TSize h) :
      width(w),
      height(h),
      map(w*h)
{
    const auto mapSize = w*h;
    for(auto i = mapSize;i--;map[i] = m[i]);
}


Actor *Location::trace(Vector2D origin, TReal direction, TReal d, Actor *actorToIgnore)
{
    const auto rotator = Vector2D{std::cos(direction),std::sin(direction)};

    for(TReal t = 0; t <= d; t+=.01f)
    {
        const auto pos = origin + t*rotator;

        for(auto &actor : actorList)
        {
            if(actor == nullptr) continue;
            if(actor == actorToIgnore) continue;

            if(intersect(pos, actor->getCollisionBody()))
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

Texture Location::generateMinimap(TSize w, TSize h)
{
    const auto mapRectH = h/height;
    const auto mapRectW = w/width;
    Texture txt({h,w});

    for(TSize j=0;j<height;++j)
        for(TSize i=0;i<width;++i)
        {
            const auto mapPos = i+j*width;
            if(map[mapPos] == ' ') continue;

            const TSize rectX = txt.size.x() - (i+1)*mapRectW + 1;
            const TSize rectY = txt.size.y() - (j+1)*mapRectH + 1;

            for(auto ki = 0;ki<mapRectW;++ki)
                for(auto kj = 0;kj<mapRectH;++kj)
                    txt.set({rectX-ki,rectY-kj},Color::Black);
        }

    return txt;
}

void Location::update(TReal dt)
{}

void World::addLocation(Location *l, TString name)
{
    locationMap[name] = l;
}

Location *World::operator [](TString name)
{
    return locationMap[name];
}


