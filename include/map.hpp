#pragma once

#include "texture.hpp"
#include "geometry.hpp"
//#include "actor.hpp"


class Actor;

struct Location
{
    Location() = default;
    Location(const char *map, TSize width, TSize height);
    Location(const Location &) = default;
    Location(Location &&) = default;
    Location &operator=(const Location &) = default;
    Location &operator=(Location &&) = default;

    Actor *trace(Vector2D origin, TReal direction, TReal dt, Actor *actorToIgnore);
    const char &operator [](TSize pos) const;
    const Texture &getWallText(TSize texId) const;
    void addWallText(PTexture txt);
    void addActor(Actor *actor);
    void removeActor(Actor *actor);
    void setMap(const char m[], TSize w, TSize h);
    Texture generateMinimap(Math::Vector2D<TSize> mapSize);

    TList<Actor*>  actorList;
    Texture         floor;
    Texture         ceil;
    TArray<Texture> textureDB;
    TArray<char>    map;
    TSize           width = 0,
                    height = 0;
};

struct World
{
    void addLocation(Location &&l, TString name);
    Location &operator[](TString name);

    //Texture map();
    THashMap<TString, Location> locationMap;
};


