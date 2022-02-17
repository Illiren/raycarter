#include "location.hpp"
#include "geometry.hpp"
#include "actor.hpp"

Location::Location(const char *m, Vector2U s) :
      map(s.x()*s.y()),
      size(s)
{
    const auto mapSize = s.x()*s.y();
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

const char &Location::operator [](Vector2U pos) const
{
    return map[pos.x()+pos.y()*size.x()];
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

void Location::setMap(const char m[], Vector2U pos)
{
    map.resize(pos.x()*pos.y());
    for(auto i=pos.x();i--;)
        for(auto j=pos.y();j--;)
            map[i+pos.x()*j] = m[i+pos.x()*j];
}

Texture Location::generateMinimap(Vector2U pos)
{
    const auto mapRect = pos/size;

    Texture txt(pos);

    for(Vector2U it{0,0};it.y()<size.y();++it.y())
        for(it.x()=0;it.x()<size.x();++it.x())
        {
            const auto mapPos = it.x()+it.y()*size.x();
            if(map[mapPos] == ' ') continue;

            const auto rect = txt.size - (it+1)*mapRect + 1;

            for(Vector2U k{0,0};k.x()<mapRect.x();++k.x())
                for(k.y()=0;k.y()<mapRect.y();++k.y())
                    txt.set(rect-k,Color::Black);
        }

    return txt;
}

void Location::update(TReal dt)
{
    (void)dt;
}

void World::addLocation(Location *l, TString name)
{
    locationMap[name] = l;
}

Location *World::operator [](TString name)
{
    return locationMap[name];
}


