#pragma once

#include "window.hpp"
#include "color.hpp"
#include "screen.hpp"
#include "map.hpp"
#include "camera.hpp"
#include "gameloop.hpp"
#include "input.hpp"
#include "player.hpp"
#include "drawable.hpp"
#include "network.hpp"
#include <functional>

using TActorAction = std::function<void (Actor *)>;
using TActor2ActorAction = std::function<void (Actor *, Actor *)>;


/*
 * Messages for Raycarter
 *
 * Client -> Server
 *
 * ActorAction
 * |<----8---->|<--8-->|
 * | ActionId  | Param |
 *
 *
 * ActorsInfo
 *          |<------ActorInfo------->|
 * |<--32-->|<---32--->|<-32->|<-32->|     |<---32--->|<-32->|<-32->|
 * |  Size  |  ActorID | PosX | PosY | ... |  ActorID | PosX | PosY |
 *          |<--------------------Size * ActorInfo----------------->|
*/

struct PlayerDescription
{
    uint32_t uniqueID = 0;
    uint32_t avatarID = 0;

    float radius = 0.5f;

    Vector2D pos;
    char walk;
};


enum class CustomMsgTypes : uint32_t
{
    ServerStatus,
    ServerPing,

    ClientAccepted,
    ClientAssignID,
    ClientRegister,
    ClientUnregister,

    GameAddPlayer,
    GameRemovePlayer,
    GameUpdatePlayer,
};



class RayCarter : public Window, public GameLoop
{
public:
    RayCarter(Rectangle2D<TSize> winSize, TReal fpsLimit = 60.f, Window *parent = nullptr);
    ~RayCarter() override;

    Map &map();
    Player _player;

private:
    Screen &screen;
    InputManager &keyMap;

    Camera _camera;
    Map _map;

    Point2D _mapOrigin;
    Point2D _mapSize;

    TSize _guiHeight;
    Font  _font;
    constexpr static TSize mapW = 16;
    constexpr static TSize mapH = 16;
    static const char mapdata[];

private:
    void draw() override;
    void drawSprite(const Sprite &sprite, const TArray<float> &depthBuffer);
    void drawGUI();
    void physX();

    static int wall2texcoord(float hx, float hy, int tw);

    // GameLoop interface
protected:
    void init() override;
    void input() override;
    void render() override;
    void update(TReal lag) override;
};
