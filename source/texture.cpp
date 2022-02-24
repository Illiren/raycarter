#include "texture.hpp"
#include <iostream>
#include "resourcemanager.hpp"
#include "color.hpp"

Texture::Texture() :
      size(0,0)
{}

Texture::Texture(Vector2U s) :
      size(s),
      img(s.x()*s.y())
{

}

Texture::Texture(const Texture &txt) :
      size(txt.size),
      img(txt.img)
{}

Texture::Texture(Texture &&txt) :
      size(txt.size),
      img(std::move(txt.img))
{
    txt.size = Vector2U{0,0};
}

Texture &Texture::operator=(const Texture &txt)
{
    size = txt.size;
    img = txt.img;

    return *this;
}

Texture &Texture::operator=(Texture &&txt)
{
    size = txt.size;
    img = std::move(txt.img);
    txt.size = Vector2U{0,0};

    return *this;
}

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

Color Texture::get(Vector2U pos) const
{
    return img[pos.x()+pos.y()*size.x()];
}

Color Texture::get(Vector2U pos, TSize spriteScreenSize) const
{
    return get(pos*size/spriteScreenSize);
}




Font::Font(TString filename, uint8_t cw, uint8_t ch, uint8_t th, uint8_t tw, uint8_t i, char startChar) :
      charSize(cw,ch),
      tableSize(tw,th),
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
    auto size = charSize;
    size.x()*=text.size();
    Texture txt(size);

    for(TSize c=0;c<text.size();++c)
    {
        const int charid = text[c] - startCharacter;
        const auto t = Vector2I{charid%tableSize.x(),
                                charid/tableSize.y()};


        for(auto it = ByteVec{0,0};it.x()<charSize.x();++it.x())
            for(it.y()=0;it.y()<charSize.y();++it.y())
            {
                const ByteVec offset = {(text[c] == 'i' || text[c] == 'I')? TByte(4) : TByte(0),
                                        (text[c] == 'i' || text[c] == 'I')? TByte(4) : TByte(1)};
                const auto pos = t*(charSize+indent);
                Color color = fontTable.get(pos + it);
                txt.set({TSize(it.x()+c*charSize.x()+offset.x()), TSize(it.y())},color);
            }
    }

    return txt;
}
