#pragma once

#include <unordered_map>
#include <memory>
#include <SDL.h>
#include "geometry.hpp"

struct Texture
{
    Texture();
    //Texture(uint32_t w, uint32_t h);
    Texture(Vector2U size);

    Texture(const Texture &txt);
    Texture(Texture &&txt);
    Texture &operator=(const Texture &txt);
    Texture &operator=(Texture &&txt);

    //void set(TSize x, TSize y, uint32_t value);
    //uint32_t get(TSize x, TSize y) const;
    //uint32_t get(TSize i, TSize j, TSize spriteScreenSize) const;
    TArray<uint32_t> getScaledColumn(TSize texCoord, TSize height) const;

    explicit operator bool() const noexcept {return img.size() != 0;}

    //TSize w,h;
    TArray<uint32_t> img;
    Vector2U size;

    void set(Vector2U pos, uint32_t value);
    uint32_t get(Vector2U pos) const;
    uint32_t get(Vector2U pos, TSize spriteScreenSize) const;
};

using PTexture = std::weak_ptr<Texture>;

inline bool isValid(const PTexture &weak)
{
    return !weak.expired();
}

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
