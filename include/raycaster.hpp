#pragma once

#include "window.hpp"
#include "color.hpp"
#include "screen.hpp"
#include "map.hpp"
#include "camera.hpp"
#include "gameloop.hpp"
#include "input.hpp"
#include "drawable.hpp"
#include "network.hpp"
#include "audio.hpp"
#include "game_object.hpp"
#include <functional>


struct PlayerDescription
{
    uint32_t uniqueID = 0;
    uint32_t avatarID = 0;
    uint32_t locactionID = 0;

    Vector2D pos;
    char walk;
    float speed;
    float runSpeedMul;
};

struct Action
{
    uint32_t playerID = 0;
    uint32_t actionID = 0;
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

    PlayerAction,
};

class NPC_Drake : public Pawn
{
public:
    NPC_Drake(PTexture text, PSound wav) :
          Pawn(text),
          sound(wav)
    {}

private:
    SoundSequence sound;

    // NActor interface
public:
    void interract(Actor *causer) override
    {
        sound.play();
    }
};

class RayCarter : public GameLoop, public Client<CustomMsgTypes>
{
public:
    enum Mode
    {
        None,
        Singleplayer,
        Multiplayer,
    };

public:
    RayCarter(Rectangle2D<TSize> winSize, TReal fpsLimit = 60.f);
    ~RayCarter() override;

    void start() { GameLoop::start();}
    void start(Mode mode);
    void addLocation(Location &&location, TString locationName);
    void addActor(Actor *actor, TString locationName);
    //void setPlayerActor(Player *player);


    World world;
    Viewport viewport;
    MovementComponent *controller = nullptr;

    TString  host;
    uint16_t port;
    uint32_t playerID;
    TextureDB textureDB;
    AudioDB autiodb;

private:
    InputManager &keyMap;

    void initPlayer(Mode mode);
    Mode currentMode;

    bool isWaitingForConnection;

    // Player Controller
    void moveForward();
    void moveBack();
    void moveStop();
    void turnRight();
    void turnLeft();
    void turnStop();
    void sprintOn();
    void sprintOff();
    void interact();

    THashMap<uint32_t, Player*> playerMap;


    // Network serialization
    Player *desc2Actor(const PlayerDescription &desc);
    void actor2Desc(const Player &player, PlayerDescription &desc);
    void updateActor(Player *player, const PlayerDescription &desc);
    Player *findPlayer(uint32_t playerID);


private:
    void physX();

    // GameLoop interface
protected:
    void init() override;
    void input() override;
    void render() override;
    void update(TReal lag) override;
};
