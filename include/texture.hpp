#pragma once

#include <unordered_map>
#include <memory>
#include <SDL.h>
#include "geometry.hpp"

struct Texture
{
    Texture();
    Texture(uint32_t w, uint32_t h);

    Texture(const Texture &txt);
    Texture(Texture &&txt);
    Texture &operator=(const Texture &txt);
    Texture &operator=(Texture &&txt);

    void set(TSize x, TSize y, uint32_t value);
    uint32_t get(TSize x, TSize y) const;
    uint32_t get(TSize i, TSize j, TSize spriteScreenSize) const;
    TArray<uint32_t> getScaledColumn(TSize texCoord, TSize height) const;

    TSize w,h;
    TArray<uint32_t> img;
};

using PTexture = std::weak_ptr<Texture>;

inline bool isValid(const PTexture &weak)
{
    return !weak.expired();
}

class TextureDB
{
    using TSharedPtr = std::shared_ptr<Texture>;
public:
    TextureDB();

    bool load(TString filename, TString nameBase = "Undefined");
    bool load(TString filename, uint32_t count, TString nameBase = "Undefined");
    PTexture operator[](TString id) const;

private:
    std::unordered_map<TString, TSharedPtr> _db;

    TString getName(TString baseName) const;
    static SDL_Surface *loadSurface(TString filename, uint32_t format = SDL_PIXELFORMAT_ABGR8888);
    static Texture *createTexture(uint32_t w, uint32_t h, uint32_t s, uint8_t *pixmap, uint32_t index = 0);

    friend struct Font;
};


struct Font
{
    Font(TString filename, uint8_t cw, uint8_t ch, uint8_t th, uint8_t tw, uint8_t i, char startChar = ' ');
    Font(const Font &) = default;
    Font(Font &&) = default;
    Font &operator = (const Font &) = default;
    Font &operator = (Font &&) = default;
    ~Font() = default;

    uint8_t   charWidth   = 15;
    uint8_t   charHeight  = 15;
    uint8_t   tableHeight = 16;
    uint8_t   tableWidth  = 16;
    uint8_t   indent      = 1;
    char      startCharacter = ' ';
    Texture  fontTable;

    Texture createText(TString text);
};
