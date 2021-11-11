#include "texture.hpp"
#include <iostream>
#include "color.hpp"

Texture::Texture() :
      w(0),
      h(0)
{}

Texture::Texture(uint32_t width, uint32_t height) :
      w(width),
      h(height),
      img(w*h)
{}

Texture::Texture(const Texture &txt) :
      w(txt.w),
      h(txt.h),
      img(txt.img)
{}

Texture::Texture(Texture &&txt) :
      w(txt.w),
      h(txt.h),
      img(std::move(txt.img))
{
    txt.w = txt.h = 0;
}

Texture &Texture::operator=(const Texture &txt)
{
    w = txt.w;
    h = txt.h;
    img = txt.img;

    return *this;
}

void Texture::set(TSize x, TSize y, uint32_t value)
{
    img[x+y*w] = value;
}

Texture &Texture::operator=(Texture &&txt)
{
    w = txt.w;
    h = txt.h;
    txt.img = std::move(txt.img);
    w=h=0;
    return *this;
}

uint32_t Texture::get(TSize i, TSize j) const
{
    return img[i+j*w];
}

uint32_t Texture::get(TSize i, TSize j, TSize spriteScreenSize) const
{
    return get(i*h/spriteScreenSize,j*w/spriteScreenSize);
}

TArray<uint32_t> Texture::getScaledColumn(TSize texCoord, TSize height) const
{
    TArray<uint32_t> column(height);
    for(TSize i=0;i<height;++i)
        column[i] = get(texCoord, (i*w)/height);
    return column;
}

TextureDB::TextureDB()
{}


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
    return _db.at(id);
}

TString TextureDB::getName(TString baseName) const
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

    uint32_t w = surface->w;

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
    Texture *texture = new Texture(s, h);

    for(int i=0;i<h;++i)
        for(int j=0;j<s;++j)
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


Font::Font(TString filename, uint8_t cw, uint8_t ch, uint8_t th, uint8_t tw, uint8_t i, char startChar) :
      charWidth(cw),
      charHeight(ch),
      tableHeight(th),
      tableWidth(tw),
      indent(i),
      startCharacter(startChar)
{
    auto surface = TextureDB::loadSurface(filename);
    if(!surface) return;

    uint32_t w = surface->w;
    uint32_t h = surface->h;
    uint8_t *pixmap = reinterpret_cast<uint8_t*>(surface->pixels);

    Texture *txt = TextureDB::createTexture(w,h,w,pixmap,0);
    fontTable = *txt;
    delete txt;

    SDL_FreeSurface(surface);
}

Texture Font::createText(TString text)
{
    const auto w = text.size()*charWidth;
    const auto h = charHeight;

    Texture txt(w,h);

    for(auto c=0;c<text.size();++c)
    {
        const int charid = text[c] - startCharacter;
        const auto tx = charid%tableWidth;
        const auto ty = charid/tableWidth;

        for(auto i=0;i<charWidth;++i)
            for(auto j=0;j<charHeight;++j)
            {
                const auto woffset = (text[c] == 'i' || text[c] == 'I')? 4 : 0; // indent between chars
                const auto ioffset = (text[c] == 'i' || text[c] == 'I')? 4 : 1; // indent between chars
                const auto x = tx*(charHeight+indent);
                const auto y = ty*(charWidth+indent);
                Color color = fontTable.get(x+i,y+j);
                txt.set(i+c*charWidth+woffset,j,color);
            }
    }

    return txt;
}
