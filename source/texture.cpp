#include "texture.hpp"
#include <iostream>
#include "resourcemanager.hpp"
#include "color.hpp"

Texture::Texture() :
      //w(0),
      //h(0),
      size(0,0)
{}
/*
Texture::Texture(uint32_t width, uint32_t height) :
      w(width),
      h(height),
      size(width,height),
      img(w*h)
{}
*/
Texture::Texture(Vector2U s) :
      //w(s.x()),
      //h(s.y()),
      size(s),
      img(s.x()*s.y())
{

}

Texture::Texture(const Texture &txt) :
      //w(txt.w),
      //h(txt.h),
      size(txt.size),
      img(txt.img)
{}

Texture::Texture(Texture &&txt) :
      //w(txt.w),
      //h(txt.h),
      size(txt.size),
      img(std::move(txt.img))
{
    //txt.w = txt.h = 0;
    txt.size = Vector2U{0,0};
}

Texture &Texture::operator=(const Texture &txt)
{
    //w = txt.w;
    //h = txt.h;
    size = txt.size;
    img = txt.img;

    return *this;
}

Texture &Texture::operator=(Texture &&txt)
{
    //w = txt.w;
    //h = txt.h;
    size = txt.size;
    img = std::move(txt.img);
    //txt.w=txt.h=0;
    txt.size = Vector2U{0,0};

    return *this;
}
/*
void Texture::set(TSize x, TSize y, uint32_t value)
{
    assert(x+y*w < img.size());
    img[x+y*w] = value;
}

uint32_t Texture::get(TSize i, TSize j) const
{
    return img[i+j*w];
}

uint32_t Texture::get(TSize i, TSize j, TSize spriteScreenSize) const
{
    return get(i*h/spriteScreenSize,j*w/spriteScreenSize);
}
*/
TArray<uint32_t> Texture::getScaledColumn(TSize texCoord, TSize height) const
{
    TArray<uint32_t> column(height);
    for(TSize i=0;i<height;++i)
        //column[i] = get(texCoord, (i*w)/height);
        column[i] = get({texCoord, (i*size.x())/height});
    return column;
}

void Texture::set(Vector2U pos, uint32_t value)
{
    assert(pos.x()+pos.y()*size.x() < img.size());
    img[pos.x()+pos.y()*size.x()] = value;
}

uint32_t Texture::get(Vector2U pos) const
{
    return img[pos.x()+pos.y()*size.x()];
}

uint32_t Texture::get(Vector2U pos, TSize spriteScreenSize) const
{
    return get(pos*size/spriteScreenSize);
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

    Texture txt({w,h});

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
                //Color color = fontTable.get(x+i,y+j);
                //txt.set(i+c*charWidth+woffset,j,color);
                Color color = fontTable.get({x+i,y+j});
                txt.set({i+c*charWidth+woffset,j},color);
            }
    }

    return txt;
}
