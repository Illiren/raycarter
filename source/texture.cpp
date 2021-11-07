#include "texture.hpp"
#include <iostream>
#include "color.hpp"

Texture::Texture(TString filename, const uint32_t format)
{
    using namespace std;
    SDL_Surface *tmp = SDL_LoadBMP(filename.c_str());
    if(!tmp)
    {
        cout << "Can't open the file" << endl;
        return;
    }

    SDL_Surface *surface = SDL_ConvertSurfaceFormat(tmp,format,0);
    SDL_FreeSurface(tmp);

    if(!surface)
    {
        cout << "Can't convert the image" << endl;
        return;
    }

    w = surface->w;
    h = surface->h;

    if(w*4!=surface->pitch)
    {
        cout << "Image format invalid" << endl;
        SDL_FreeSurface(surface);
        return;
    }

    if(w!=h*int(w/h))
    {
        cout << "Image format invalid" << endl;
        SDL_FreeSurface(surface);
        return;
    }

    count = w/h;
    size = w/count;
    uint8_t *pixmap = reinterpret_cast<uint8_t*>(surface->pixels);

    img = TArray<uint32_t>(w*h);
    for(int i=0;i<h;++i)
        for(int j=0;j<w;++j)
        {
            auto r = pixmap[(j+i*w)*4+0];
            auto g = pixmap[(j+i*w)*4+1];
            auto b = pixmap[(j+i*w)*4+2];
            auto a = pixmap[(j+i*w)*4+3];
            img[j+i*w]= (r << 0) | (g << 8) | (b << 16) | (a << 24);
        }
    SDL_FreeSurface(surface);
}

uint32_t Texture::get(TSize i, TSize j, TSize id) const
{
    return img[i+id*size+j*w];
}

TArray<uint32_t> Texture::getScaledColumn(TSize texId, TSize texCoord, TSize height) const
{
    TArray<uint32_t> column(height);
    for(TSize i=0;i<height;++i)
        column[i] = get(texCoord, (i*size)/height,texId);
    return column;
}

NTexture::NTexture() :
      w(0),
      h(0)
{ }

NTexture::NTexture(uint32_t width, uint32_t height) :
      w(width),
      h(height),
      img(w*h)
{}

NTexture::NTexture(const NTexture &txt) :
      w(txt.w),
      h(txt.h),
      img(txt.img)
{}

NTexture::NTexture(NTexture &&txt) :
      w(txt.w),
      h(txt.h),
      img(std::move(txt.img))
{
    txt.w = txt.h = 0;
}

NTexture &NTexture::operator=(const NTexture &txt)
{
    w = txt.w;
    h = txt.h;
    img = txt.img;

    return *this;
}

void NTexture::set(TSize x, TSize y, uint32_t value)
{
    img[x+y*w] = value;
}

NTexture &NTexture::operator=(NTexture &&txt)
{
    w = txt.w;
    h = txt.h;
    txt.img = std::move(txt.img);
    w=h=0;
    return *this;
}

uint32_t NTexture::get(TSize i, TSize j) const
{
    return img[i+j*w];
}

uint32_t NTexture::get(TSize i, TSize j, TSize spriteScreenSize) const
{
    return get(i*h/spriteScreenSize,j*w/spriteScreenSize);
}

TArray<uint32_t> NTexture::getScaledColumn(TSize texCoord, TSize height) const
{
    TArray<uint32_t> column(height);
    for(TSize i=0;i<height;++i)
        column[i] = get(texCoord, (i)/height);
    return column;
}

NTextureDB::NTextureDB()
{}

bool NTextureDB::loadFont(TString filename)
{
    auto surface = loadSurface(filename);
    if(!surface) return false;

    uint32_t w = surface->w;
    uint32_t h = surface->h;
    uint8_t *pixmap = reinterpret_cast<uint8_t*>(surface->pixels);

    NTexture *txt = createTexture(w,h,w,pixmap,0);
    _fontText = *txt;
    delete txt;

    SDL_FreeSurface(surface);
    return true;
}

bool NTextureDB::load(TString filename, uint32_t count, TString nameBase)
{
    auto surface = loadSurface(filename);
    if(!surface) return false;

    uint32_t w = surface->w;
    uint32_t h = surface->h;
    uint32_t size = w/count;
    uint8_t *pixmap = reinterpret_cast<uint8_t*>(surface->pixels);

    for(TSize k = 0; k < count; ++k)
    {
        NTexture *txt = createTexture(w,h,size,pixmap,k);
        TString name = getName(nameBase);
        _db.insert({name, TSharedPtr(txt)});
    }

    SDL_FreeSurface(surface);
    return true;
}

NTexture NTextureDB::generateTexture(TString text)
{
    constexpr auto charWidth = 15;
    constexpr auto charHeight = 15;
    constexpr auto tableHeight = 16;
    constexpr auto tableWidth = 16;
    const auto w = text.size()*charWidth;
    const auto h = charHeight;

    NTexture txt(w,h);

    for(auto c=0;c<text.size();++c)
    {
        const int charid = text[c] - ' ';
        const auto tx = charid%tableWidth;
        const auto ty = charid/tableWidth;

        for(auto i=0;i<charWidth;++i)
            for(auto j=0;j<charHeight;++j)
            {
                const auto indent = 1; // indent between chars
                const auto x = tx*(charHeight+indent);
                const auto y = ty*(charWidth+indent);
                Color color = _fontText.get(x+i,y+j);
                txt.set(i+c*charHeight,j,color);
            }
    }

    return txt;
}

bool NTextureDB::load(TString filename, TString nameBase)
{
    auto surface = loadSurface(filename);
    if(!surface) return false;

    uint32_t w = surface->w;
    uint32_t h = surface->h;
    uint8_t *pixmap = reinterpret_cast<uint8_t*>(surface->pixels);

    NTexture *txt = createTexture(w,h,w,pixmap,0);
    TString name = getName(nameBase);
    _db.insert({name, TSharedPtr(txt)});

    SDL_FreeSurface(surface);
    return true;
}

PTexture NTextureDB::operator [](TString id) const
{
    return _db.at(id);
}

TString NTextureDB::getName(TString baseName) const
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

SDL_Surface *NTextureDB::loadSurface(TString filename, uint32_t format)
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

NTexture *NTextureDB::createTexture(uint32_t w, uint32_t h, uint32_t s, uint8_t *pixmap, uint32_t k)
{
    const auto r = k*s;
    NTexture *texture = new NTexture(s, h);

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
    auto surface = NTextureDB::loadSurface(filename);
    if(!surface) return;

    uint32_t w = surface->w;
    uint32_t h = surface->h;
    uint8_t *pixmap = reinterpret_cast<uint8_t*>(surface->pixels);

    NTexture *txt = NTextureDB::createTexture(w,h,w,pixmap,0);
    fontTable = *txt;
    delete txt;

    SDL_FreeSurface(surface);
}

NTexture Font::createText(TString text)
{
    const auto w = text.size()*charWidth;
    const auto h = charHeight;

    NTexture txt(w,h);

    for(auto c=0;c<text.size();++c)
    {
        const int charid = text[c] - startCharacter;
        const auto tx = charid%tableWidth;
        const auto ty = charid/tableWidth;

        for(auto i=0;i<charWidth;++i)
            for(auto j=0;j<charHeight;++j)
            {
                const auto indent = 1; // indent between chars
                const auto x = tx*(charHeight+indent);
                const auto y = ty*(charWidth+indent);
                Color color = fontTable.get(x+i,y+j);
                txt.set(i+c*charHeight,j,color);
            }
    }

    return txt;
}
