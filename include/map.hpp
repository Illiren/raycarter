#pragma once

#include "texture.hpp"
#include "geometry.hpp"
//#include "actor.hpp"


class Map
{
public:
    Map(const char *map, TSize width, TSize height);

    //void spawn();
    class Actor *spawn();
    class Actor *trace(Vector2D origin, TReal agle, TReal dt, class Actor *actorToIgnore);

    TSize width() const;
    TSize height() const;

    const char &operator [](TSize pos) const;    
    const Texture &getTexture(TSize texId) const;
    void addTexture(PTexture txt);


private:
    const char *mapRef = nullptr;
    TSize _width = 0,
          _height = 0;
    TArray<Texture> n_textureDB;

    constexpr static char empty = ' ';
    constexpr static char actor = 'a';
};



class NActor;

struct Location
{
    Location();
    Location(const Location &) = default;
    Location(Location &&) = default;
    Location &operator=(const Location &) = default;
    Location &operator=(Location &&) = default;

    NActor *trace(Vector2D origin, TReal direction, TReal dt, NActor *actorToIgnore);
    const char &operator [](TSize pos) const;
    const Texture &getTexture(TSize texId) const;
    void addTexture(PTexture txt);
    void addActor(NActor *actor);
    void removeActor(NActor *actor);
    void setMap(const char m[], TSize w, TSize h);

    TList<NActor*>  actorList;
    Texture         floor;
    Texture         ceiling;
    TArray<Texture> textureDB;
    TArray<char>    map;
    TSize           width = 0,
                    height = 0;
};

struct World
{
    void addLocation(Location &&l);

    //Texture map();
    THashMap<TString, Location> locationMap;
};


