#pragma once

#include "geometry.hpp"
#include "stddefines.hpp"
#include "window.hpp"
#include "screen.hpp"
#include "drawable.hpp"

//Camera class hold camera parameters
struct Camera
{
    Vector2D origin;  //Position of camera
    TReal direction;  //direction
    TReal fov; //field of view
    TReal distance; //distance
    //another useful commentary
};


struct CameraComponent;
class Actor;

//Viewport represend the render algorithm
class Viewport : public Window
{
public:
    Viewport(Rectangle2D<TSize> rect, Window *parent = nullptr);

    void setCamera(CameraComponent *cam);

    Vector2U mapSize;
    TSize fps = 0;
    TReal dt = 0.01f;

    //Temporary GUI
    Texture hudWeapon;
    Texture playerFace;

private:    
    Camera *camera = nullptr;
    Actor *actor = nullptr;
    TArray<float> depthBuffer;
    Screen &screen;


    void drawSprite(const Sprite &sprite);
    static int wall2texcoord(Vector2D h, int tw);


    //GUI
    Font  _font;
    Texture _minimap;
    Vector2U _mapRect;

protected:
    void drawWorld();
    void drawActors();
    void drawHUD();

    // Window interface
public:
    void draw() override;
};


