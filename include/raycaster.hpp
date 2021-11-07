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



class NetworkActor : public Actor
{
public:
    NetworkActor(uint32_t ID) :
        Actor(),
          _id(ID)
    {}

    uint32_t id() const {return _id;}

private:
    uint32_t _id; //id of connection
};


enum class CustomMsgTypes : uint32_t
{
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage
};

class CustomServer : public Server<CustomMsgTypes>
{
public:
    CustomServer(uint16_t nPort);

protected:
    bool onClientConnect(std::shared_ptr<Connection<CustomMsgTypes>> client) override;
    void onClientDisconnect(std::shared_ptr<Connection<CustomMsgTypes>> client) override;
    void onMessage(std::shared_ptr<Connection<CustomMsgTypes>> client, Message<CustomMsgTypes> &msg) override;
};

class CustomClient : public Client<CustomMsgTypes>
{
public:
    void pingServer();
    void messageAll();
};

class RayCarter : public Window, public GameLoop
{
public:
    RayCarter(Rectangle2D<TSize> winSize, TReal fpsLimit = 60.f, Window *parent = nullptr);
    ~RayCarter() override {}

    Map &map() {return _map;}
    Player _player;
    NTexture testText;


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
    void drawSprite(const NSprite &sprite, const TArray<float> &depthBuffer);
    void drawGUI();
    void physX();

    static bool collisionCheck(const Actor &rect1, const Actor &rect2);

    // GameLoop interface
protected:
    void init() override;
    void input() override;
    void render() override;
    void update(TReal lag) override;
};
