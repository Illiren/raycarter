#pragma once

#include "geometry.hpp"
#include "stddefines.hpp"
#include "window.hpp"
#include "screen.hpp"
#include "drawable.hpp"

struct Camera
{
    Vector2D origin;
    TReal direction;  //direction
    TReal fov; //field of view
    TReal distance; //distance
    //another useful comment
};


class CameraComponent;
class Actor;

class Viewport : public Window
{
public:
    Viewport(Rectangle2D<TSize> rect, Window *parent = nullptr);

    void setCamera(CameraComponent *cam);

    Math::Vector2D<TSize> mapSize;
    TSize fps = 0;

    //GUI
    Texture hudWeapon;
    Texture playerFace;

private:
    //CameraComponent *camera = nullptr;
    Camera *camera = nullptr;
    Actor *actor = nullptr;
    TArray<float> depthBuffer;
    Screen &screen;

    void drawSprite(const Sprite &sprite);
    static int wall2texcoord(TReal hx, TReal hy, int tw);

    //GUI
    Font  _font;
    Texture minimap;
    TSize _mapRectW = 0;
    TSize _mapRectH = 0;

protected:
    void drawWorld();
    void drawActors();
    void drawHUD();

    // Window interface
public:
    void draw() override;
};
