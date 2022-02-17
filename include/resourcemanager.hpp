#pragma once

#include "audio.hpp"
#include "texture.hpp"


class AudioDB
{
    using TShared = std::shared_ptr<Sound>;
    using TMapData = std::unordered_map<TString, TShared>;
public:
    AudioDB();

    bool load(TString filename, TString nameBase = "Undefined");
    PSound operator[](TString id) const;

private:
    TMapData _db;
    TString getName(TString baseName) const;
};

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
    TSharedPtr _defaultTexture;

    TString getName(TString baseName) const;
    static SDL_Surface *loadSurface(TString filename, uint32_t format = SDL_PIXELFORMAT_ABGR8888);
    static Texture *createTexture(uint32_t w, uint32_t h, uint32_t s, uint8_t *pixmap, uint32_t index = 0);

    friend struct Font;
};


class ResourceManager
{
public:
    bool loadTexture(TString filename, TString namebase = "Undefined");
    bool loadTexture(TString filename, uint32_t count, TString namebase = "Undefined");
    bool loadAudio(TString filename, TString namebase = "Undefined");

    PSound sound(TString filename);
    PTexture texture(TString filename);


    friend ResourceManager &RM();

private:
    AudioDB   _audioDB;
    TextureDB _textureDB;
};


inline ResourceManager &RM()
{
    static ResourceManager rm;
    return rm;
}
