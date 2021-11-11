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
