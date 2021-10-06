#include "map.hpp"

Map::Map(const char *map, TSize width, TSize height, const TString &filename) :
      mapRef(map),
      _width(width),
      _height(height),
      _textureDB(filename,SDL_PIXELFORMAT_ABGR8888)
{}

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

