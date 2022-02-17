#include "resourcemanager.hpp"
#include "color.hpp"
#include <iostream>

using namespace std;

AudioDB::AudioDB()
{}

bool AudioDB::load(TString filename, TString nameBase)
{
    TString name = getName(nameBase);
    Sound *w = new Sound(filename);
    if(!w || !(*w))
    {
        std::cout << "Couldn't load wav" << std::endl;
        delete w;
        return false;
    }

    _db.insert({name, TShared(w)});
    return true;
}

PSound AudioDB::operator[](TString id) const
{
    if(_db.contains(id))
        return _db.at(id);
    return PSound();
}

TString AudioDB::getName(TString baseName) const
{
    unsigned int counter = 0;
    TString newName = baseName;

    while(1) //THE BAD THING
    {
        auto it = _db.find(newName);

        if(it == _db.end())
            return newName;
        else
        {
            if(counter == std::numeric_limits<decltype(counter)>::max())
                break;
            newName = baseName+std::to_string(counter++);
        }
    }

    return "FUCK"; //Signals an error
}

TextureDB::TextureDB() :
    _defaultTexture(new Texture({64,64}))
{
    for(TSize i=0;i<_defaultTexture->size.x();++i)
        for(TSize j=0;j<_defaultTexture->size.y();++j)
            _defaultTexture->set({i,j},Color(125,0,0));
    _db["default"] = _defaultTexture;
}


bool TextureDB::load(TString filename, uint32_t count, TString nameBase)
{
    auto surface = loadSurface(filename);
    if(!surface) return false;

    uint32_t w = surface->w;
    uint32_t h = surface->h;
    uint32_t size = w/count;
    uint8_t *pixmap = reinterpret_cast<uint8_t*>(surface->pixels);

    for(TSize k = 0; k < count; ++k)
    {
        Texture *txt = createTexture(w,h,size,pixmap,k);
        TString name = getName(nameBase+std::to_string(k));
        _db.insert({name, TSharedPtr(txt)});
    }

    SDL_FreeSurface(surface);
    return true;
}

bool TextureDB::load(TString filename, TString nameBase)
{
    auto surface = loadSurface(filename);
    if(!surface) return false;

    uint32_t w = surface->w;
    uint32_t h = surface->h;
    uint8_t *pixmap = reinterpret_cast<uint8_t*>(surface->pixels);

    Texture *txt = createTexture(w,h,w,pixmap,0);
    TString name = getName(nameBase);
    _db.insert({name, TSharedPtr(txt)});

    SDL_FreeSurface(surface);
    return true;
}

PTexture TextureDB::operator [](TString id) const
{
    if(_db.contains(id))
        return _db.at(id);
    return _defaultTexture;
}

TString TextureDB::getName(TString baseName) const
{
    TSize counter = 0;
    TString newName = baseName;

    while(1) //THE BAD THING
    {
        auto it = _db.find(newName);

        if(it == _db.end())
            return newName;
        else
        {
            if(counter == std::numeric_limits<decltype(counter)>::max())
                break;
            newName = baseName+std::to_string(counter++);
        }
    }

    return "FUCK"; //Signals an error
}

SDL_Surface *TextureDB::loadSurface(TString filename, uint32_t format)
{
    using namespace std;
    SDL_Surface *tmp = SDL_LoadBMP(filename.c_str());
    if(!tmp)
    {
        cout << "Can't open the file" << endl;
        return nullptr;
    }

    SDL_Surface *surface = SDL_ConvertSurfaceFormat(tmp,format,0);
    SDL_FreeSurface(tmp);

    if(!surface)
    {
        cout << "Can't convert the image" << endl;
        return nullptr;
    }

    int32_t w = surface->w;

    if(w*4!=surface->pitch)
    {
        cout << "Image format invalid" << endl;
        SDL_FreeSurface(surface);
        return nullptr;
    }

    return surface;
}

Texture *TextureDB::createTexture(uint32_t w, uint32_t h, uint32_t s, uint8_t *pixmap, uint32_t k)
{
    const auto r = k*s;
    Texture *texture = new Texture({s, h});

    for(TSize i=0;i<h;++i)
        for(TSize j=0;j<s;++j)
        {
            const auto pindex = (j+r+i*w)*4;
            const auto index = j+i*s;
            auto r = pixmap[pindex+0];
            auto g = pixmap[pindex+1];
            auto b = pixmap[pindex+2];
            auto a = pixmap[pindex+3];
            assert(index < texture->img.size() && "ARRR");
            texture->img[index] = (r << 0) | (g << 8) | (b << 16) | (a << 24);
        }

    return texture;
}


bool ResourceManager::loadTexture(TString filename, TString namebase)
{
    return _textureDB.load(filename,namebase);
}

bool ResourceManager::loadTexture(TString filename, uint32_t count, TString namebase)
{
    return _textureDB.load(filename,count,namebase);
}

bool ResourceManager::loadAudio(TString filename, TString namebase)
{
    return _audioDB.load(filename, namebase);
}

PSound ResourceManager::sound(TString filename)
{
    return _audioDB[filename];
}

PTexture ResourceManager::texture(TString filename)
{
    return _textureDB[filename];
}
