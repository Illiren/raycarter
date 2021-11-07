#include "player.hpp"
#include "drawable.hpp"


Player::Player(Camera &c, Map &m) :
      Actor(),
      camera(c),
      map(m)
{}

void Player::update(TReal dt)
{
    Actor::update(dt);
    camera.direction += float(turn)*0.002*dt;

    constexpr float length=0.5f;

    const auto cosa = std::cos(camera.direction);
    const auto sina = std::sin(camera.direction);

    float nx = camera.origin.x() + cosa*speed*runningSpeed*dt;
    float ny = camera.origin.y() + sina*speed*runningSpeed*dt;

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

void Player::doInteract()
{
    auto actor = map.trace({camera.origin.x(), camera.origin.y()}, camera.direction, 2, this);
    if(actor)
        actor->interract(this);
}

Vector2D Player::position() const
{
    return {camera.origin.x(), camera.origin.y()};
}

void Player::collision(Actor *another)
{
    //speed = 0;
    auto apos = another->position();
    auto ppos = position();
    Vector2D dir = {ppos.x() - apos.x(), ppos.y() - apos.y()};
    auto l = std::sqrt(dir.norm());
    dir *= 1/l;

    camera.origin.x() += dir.x();
    camera.origin.y() += dir.y();
}

