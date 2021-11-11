#include <iostream>
#include "raycaster.hpp"
#include "audio.hpp"
#include "network.hpp"
#include <thread>

struct NActor : public GameObject
{
    enum WalkState
    {
        Idle = 0,
        Forward = -1,
        Back = 1,
        Left = -1,
        Right = 1
    };

    char walk = 0;
    char turn = 0;
    float runningSpeed = 1;
    float speed = .0f;
    float maxSpeed = 0.002f;

    Map *map;
    std::shared_ptr<Drawable> drawable;
    uint32_t id;
    Vector2D rectSize;
    TReal dir;


    NActor() :
          GameObject()
    {}
    ~NActor() override {}

    virtual void setPos(const Vector2D &v)
    {
        if(drawable)
        {
            drawable->x = v.x();
            drawable->y = v.y();
        }
        else return;
    }
    virtual void setDirection(TReal d)
    {
        dir = d;
    }
    virtual Vector2D position() const
    {
        if(!drawable) return {0.f,0.f};
        return {drawable->x,drawable->y};
    }
    virtual TReal direction() const
    {
        return dir;
    }
    virtual void collision(Actor *another)
    {
        /*
        auto apos = another->position();
        auto ppos = position();
        Vector2D dir = {ppos.x() - apos.x(), ppos.y() - apos.y()};
        auto l = std::sqrt(dir.norm());
        dir *= 1/l;

        auto pos = position();
        pos.x() += dir.x();
        pos.y() += dir.y();*/
    }

    virtual void interract(Actor *causer) {}
    FRectangle2D getCollisionRect() const
    {
        Rectangle2D<TReal> rect;
        rect.topleft.x() = position().x() - rectSize.x();
        rect.topleft.y() = position().y() - rectSize.y();

        rect.botright.x() = position().x() + rectSize.x();
        rect.botright.y() = position().y() + rectSize.y();

        return rect;
    }

    void update(TReal dt) override
    {
        Vector2D pos = Vector2D(drawable->x, drawable->y);

        dir += float(turn)*0.002*dt;

        constexpr float length=0.5f;

        const auto cosa = std::cos(dir);
        const auto sina = std::sin(dir);

        float nx = pos.x() + cosa*speed*runningSpeed*dt;
        float ny = pos.y() + sina*speed*runningSpeed*dt;

        float dx = length*cosa+nx;
        float dy = length*sina+ny;

        //const size_t currentPos = pos.x()+pos.y()*map.width();

        if(int(nx) >= 0 && int(nx) < int(map->width()) && int(ny) >= 0 && int(ny)<int(map->height()))
        {
            const size_t posX = size_t(dx)+size_t(pos.y())*map->width();
            const size_t posY = size_t(pos.x())+size_t(dy)*map->width();

            if((*map)[posX] == ' ')
                pos.x() = nx;
            if((*map)[posY] == ' ')
                pos.y() = ny;
        }
        setPos(pos);
    }
};

struct CameraObject : public GameObject
{
    CameraObject() :
        GameObject()
    {}
    ~CameraObject() override {}

    void update(TReal dt) override
    {
        if(actor)
        {
            pos = actor->position();
            direction = actor->dir;
        }
    }


    NActor *actor = nullptr;
    Vector2D pos;
    TReal direction;  //direction
    TReal fov; //field of view
    TReal distance; //distance
};



//CLIENT
class ClientTest : public GameLoop, virtual public Client<CustomMsgTypes>
{

public:
    ClientTest(TReal fpsLimit = 60.f) :
          GameLoop(fpsLimit),
          keyMap(GetInputManager()),
          screen(GetScreen()),
          audio(GetAudio()),
          map(mapdata,mapWidth,mapHeight),
          font("smallfont.bmp",15,15,16,16,1,' ')
    {
        screen.resize(width,height);
        if(!screen.init())
        {
            std::cout << "Can't init screen fb\n";
            return;
        }

        textDB.load("dwarf.bmp", "player0");
        textDB.load("orc.bmp", "player1");
        textDB.load("troll.bmp", "player2");
        textDB.load("walltext.bmp",6,"wall");
        textDB.load("player.bmp","player");
        textDB.load("weapon.bmp","weapon");
        audioDB.load("test.wav", "drakesay1");

        for(auto i=0;i<6;++i)
        {
            wallTextures.push_back(*(textDB["wall"+std::to_string(i)].lock()));
            wallCollors.push_back(Color(20*(i+1),20*(i+1),20*(i+1)));
        }

        //player.camera = &camera;

        keyMap['a'].keydownEvent = [this](){ if(camera.actor) camera.actor->turn = -1; };
        keyMap['a'].keyupEvent = [this](){ if(camera.actor) camera.actor->turn = 0; };

        keyMap['d'].keydownEvent = [this](){ if(camera.actor) camera.actor->turn = 1; };
        keyMap['d'].keyupEvent = [this](){ if(camera.actor) camera.actor->turn = 0; };

        keyMap['w'].keydownEvent = [this](){ if(camera.actor) camera.actor->speed = camera.actor->maxSpeed; };
        keyMap['w'].keyupEvent = [this](){ if(camera.actor) camera.actor->speed = 0; };

        keyMap['s'].keydownEvent = [this](){ if(camera.actor) camera.actor->speed = -camera.actor->maxSpeed; };
        keyMap['s'].keyupEvent = [this](){ if(camera.actor) camera.actor->speed = 0; };

        keyMap[SDLK_LSHIFT].keydownEvent = [this](){ if(camera.actor) camera.actor->runningSpeed = 2; };
        keyMap[SDLK_LSHIFT].keyupEvent = [this](){ if(camera.actor) camera.actor->runningSpeed = 1; };

    }

    ~ClientTest() override {disconnect();}

    InputManager &keyMap;
    Screen       &screen;
    Audio        &audio;

    TSize height = 768;
    TSize width = 768;

    Font font;
    std::unordered_map<uint32_t,NActor> mapObject;
    uint32_t playerID = 0;
    PlayerDescription desc;
    TextureDB textDB;
    AudioDB   audioDB;

    //Player Controller
    //NActor  player;
    CameraObject  camera;
    Texture       face;
    Texture       weapon;

    //Map
    TArray<Texture> wallTextures;
    TArray<Color> wallCollors; //For Test
    static const char mapdata[];
    TSize mapWidth = 16;
    TSize mapHeight = 16;
    Map map;


    // GameLoop interface
protected:
    void init() override
    {
        if(!connect("127.0.0.1", 1234))
            std::cout << "Can't connect ot server" << std::endl;

        camera.pos = {3.456f,2.345f};
        camera.direction = 4.5f*static_cast<float>(M_PI)/2.f;
        camera.fov = 1;
        camera.distance = 20;
    }

    void input() override { keyMap.update(); }
    void render() override
    {
        screen.clear(Color(155,155,155));
        std::vector<float> depthBuffer(width, 1e3);

        if(isWaitingForConnection)
        {
            Texture text = font.createText("Waiting for Connection");
            screen.drawTexture(text,{300,300});
            screen.update();
            return;
        }


        for(TSize i=0;i<width;++i)
        {
            const TReal angle = camera.direction - camera.fov/2 + camera.fov*i/float(width);
            const auto acos = std::cos(angle);
            const auto asin = std::sin(angle);

            for(TReal t=0;t<camera.distance; t+=.01f)
            {
                const TReal cx = camera.pos.x() + t*acos;
                const TReal cy = camera.pos.y() + t*asin;

                const auto mapPos = static_cast<int>(cx) + static_cast<int>(cy) * static_cast<int>(map.width());

                if(map[mapPos] != ' ')
                {
                    const auto dist = t*std::cos(angle-camera.direction);
                    depthBuffer[i] = dist;
                    const TSize columnH = TSize(height/dist);

                    const auto itex = map[mapPos] - '0';
                    //auto tex = wallTextures[itex];
                    //auto xtex = wall2texcoord(cx,cy,tex.w);
                    //TArray<uint32_t> column = tex.getScaledColumn(xtex,columnH);

                    const auto x = i;
                    const auto y = height/2-columnH/2;

                    Color color = wallCollors[itex];
                    screen.drawRectangle({x,y},1,columnH,color);
                    /*
                    for(TSize j=0;j<height;++j)
                    {
                        TSize py = j + screen.height()/2 - columnH/2;
                        if(py>=0)
                        {

                        }
                    }*/

                    break;
                }
            }
        }

        for(auto &actor : mapObject)
        {
            if(camera.actor == &actor.second)
                continue;
            Drawable *d = actor.second.drawable.get();
            if(auto s = dynamic_cast<Sprite*>(d))
            {
                s->updateDist(camera.pos.x(),camera.pos.y());
                drawSprite(*s,depthBuffer);
            }
        }


        screen.update();
    }

    void drawSprite(const Sprite &sprite, const TArray<float> &depthBuffer)
    {
        auto texture = sprite.tex.lock();
        float spriteDir = atan2(sprite.y - camera.pos.y(), sprite.x - camera.pos.x());
        while(spriteDir - camera.direction > M_PI) spriteDir -= 2*M_PI;
        while(spriteDir - camera.direction < - M_PI) spriteDir += 2*M_PI;

        auto dist = sprite.dist < 1 ? 1 : sprite.dist;
        TSize spriteScreenSize = std::min(static_cast<int>(width/dist), static_cast<int>(height/dist));
        int hOffset = (spriteDir - camera.direction)*(width)/(camera.fov) + (width)/2 - spriteScreenSize/2;
        int vOffset = height/2 - spriteScreenSize/2;

        //#pragma omp parallel for
        for(TSize i=0;i<spriteScreenSize;++i)
        {
            if(hOffset+int(i)<0 || hOffset+i >= width) continue;
            if(depthBuffer[hOffset + i]<sprite.dist) continue;
            for(TSize j=0;j<spriteScreenSize;++j)
            {
                if(vOffset+int(j)<0 || vOffset+j>=height) continue;
                Color color = texture->get(i,j,spriteScreenSize);
                if(color.alpha() > 128)
                {
                    const auto x = hOffset+i;
                    const auto y = vOffset+j;
                    if(y < height)
                        screen.drawPoint({x,y},color);
                }
            }
        }
    }

    void update(TReal lag) override
    {
        if(isConnected())
        {
            while(!incoming().empty())
            {
                auto msg = incoming().pop_front().msg;

                switch (msg.header.id)
                {
                case CustomMsgTypes::ClientAccepted:
                {
                    std::cout << "Server accepted client - you're in!\n";
                    Message<CustomMsgTypes> msg;
                    msg.header.id = CustomMsgTypes::ClientRegister;
                    desc.pos = {3.456f,2.345f};
                    desc.avatarID = 0;

                    msg << desc;
                    send(msg);
                    break;
                }
                case CustomMsgTypes::ClientAssignID:
                {
                    msg >> playerID;
                    std::cout << "Assigned Client ID = " << playerID << "\n";
                    break;
                }
                case CustomMsgTypes::GameAddPlayer:
                {                    
                    PlayerDescription d;
                    msg >> d;
                    //mapObject.insert_or_assign(d.uniqueID, d);
                    mapObject.insert({d.uniqueID,actorFromDesc(d)});

                    std::cout << "Game Add Player id:" << d.uniqueID << std::endl;

                    if(d.uniqueID == playerID)
                    {
                        std::cout << "Player id ack, id=" << playerID << std::endl;
                        camera.actor = &mapObject[d.uniqueID];
                        isWaitingForConnection = false;
                    }
                    else
                    {
                        std::cout << "Player id nak, id=" << playerID << " in id: " << d.uniqueID << std::endl;
                    }
                    break;
                }
                case CustomMsgTypes::GameRemovePlayer:
                {
                    uint32_t removalID = 0;
                    msg >> removalID;
                    mapObject.erase(removalID);
                    break;
                }
                case CustomMsgTypes::GameUpdatePlayer:
                {
                    PlayerDescription d;
                    msg >> d;
                    if(mapObject.contains(d.uniqueID))
                    {
                        updateActor(mapObject[d.uniqueID],d);
                    }
                    //mapObject.insert_or_assign(d.uniqueID,d);
                    break;
                }
                default:
                    break;
                }
            }
        }

        if(isWaitingForConnection)
        {
            //TODO: Draw Text "Waiting for connection...
            return;
        }

        auto &list = GameObject::getRegister();
        for(auto &item : list)
            item->update(lag);

        for(auto &item : mapObject)
        {
            item.second.update(lag);
        }

        if(camera.actor)
        {
            PlayerDescription d;
            Message<CustomMsgTypes> msg;
            msg.header.id = CustomMsgTypes::GameUpdatePlayer;
            actor2desc(d,*camera.actor);
            d.uniqueID = playerID;
            msg << d;
            send(msg);
        }
    }



    static int wall2texcoord(float hx, float hy, int tw);

private:
    bool isWaitingForConnection = true;


    NActor actorFromDesc(const PlayerDescription &d)
    {
        NActor a;
        a.id = d.uniqueID;
        a.drawable.reset(new Sprite(textDB["player"+std::to_string(d.avatarID)],d.pos));
        a.map = &map;
        return a;
    }

    void updateActor(NActor &a, const PlayerDescription &d)
    {
        a.setPos(d.pos);
    }

    void actor2desc(PlayerDescription &d, const NActor &a)
    {
        d.pos = a.position();
        d.uniqueID = a.id;
        //d.walk = a.walk;
    }
};

int ClientTest::wall2texcoord(float hx, float hy, int tw)
{
    float x = hx - floor(hx+.5);
    float y = hy - floor(hy+.5);

    int tex = x * tw;
    if(abs(y) > abs(x))
        tex = y*tw;
    if(tex < 0)
        tex+=tw;
    return tex;
}



const char ClientTest::mapdata[] =
    "0000000000000000"\
    "0              0"\
    "0     444      0"\
    "0     4        0"\
    "0     4        0"\
    "0     3        0"\
    "0   44444      0"\
    "0   4   2      0"\
    "0   4   2      0"\
    "0   4   2      0"\
    "0       2      0"\
    "05555   2      0"\
    "0       2      0"\
    "0    2222      0"\
    "0              0"\
    "0000000000000000";


int main()
{
    ClientTest game;
    bool debug = true;

    GetInputManager()[SDLK_ESCAPE].keydownEvent = [&game,&debug](){game.stop(); debug = false;};
    game.start();

    while(debug)
    {
        GetInputManager().update();
    }

    return 0;
}

