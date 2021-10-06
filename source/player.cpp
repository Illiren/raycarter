#include "player.hpp"


Player::Player(Camera &c, Map &m) :
      camera(c),
      map(m)
{}

void Player::update(float dt)
{
    camera.direction += float(turn)*.002f*dt;

    constexpr float length=0.5f;

    const auto cosa = std::cos(camera.direction);
    const auto sina = std::sin(camera.direction);

    float nx = camera.origin.x() + walk*cosa*.002f*runningSpeed*dt;
    float ny = camera.origin.y() + walk*sina*.002f*runningSpeed*dt;

    float dx = length*cosa+nx;
    float dy = length*sina+ny;

    const size_t currentPos = camera.origin.x()+camera.origin.y()*map.width();


    if(int(nx) >= 0 && int(nx) < int(map.width()) && int(ny) >= 0 && int(ny)<int(map.height()))
    {
        const size_t posX = size_t(dx)+size_t(camera.origin.y())*map.width();
        const size_t posY = size_t(camera.origin.x())+size_t(dy)*map.width();

        if(map[posX] == ' ')
            camera.origin.x() = nx;
        if(map[posY] == ' ')
            camera.origin.y() = ny;
    }
}

