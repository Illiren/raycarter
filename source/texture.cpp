#include "texture.hpp"
#include <iostream>

Texture::Texture(const TString filename, const uint32_t format)
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
