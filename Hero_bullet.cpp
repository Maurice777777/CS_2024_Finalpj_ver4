#include "Hero_bullet.h"
#include <cmath>
#include <allegro5/allegro_primitives.h>
#include "data/DataCenter.h"
#include "data/GIFCenter.h"

constexpr double PI = 3.14159265358979323846;
constexpr double DEG_TO_RAD = PI / 180.0;

Hero_bullet::Hero_bullet(Point start, double speed, Direction direction)
    : position(start), speed(speed), direction(direction), damage(10.0) {
    init();
}

void Hero_bullet::init() {
    GIFCenter *GIFC = GIFCenter::get_instance();
    gif = GIFC->get("./assets/gif/Hero/fireball.gif"); // 這裡是你子彈 GIF 的路徑
}

void Hero_bullet::update() {
    double angle = 0.0;
    switch (direction) {
        case Direction::FRONT:
            angle = 270.0; // 向上
            break;
        case Direction::BACK:
            angle = 90.0; // 向下
            break;
        case Direction::LEFT:
            angle = 180.0; // 向左
            break;
        case Direction::RIGHT:
            angle = 0.0; // 向右
            break;
    }

    position.x += speed * cos(angle * DEG_TO_RAD);
    position.y += speed * sin(angle * DEG_TO_RAD);
}

void Hero_bullet::draw() {
    if (gif != nullptr) {
        algif_draw_gif(gif, position.x, position.y, 0);
    }
}

bool Hero_bullet::is_out_of_bound() const {
    DataCenter *DC = DataCenter::get_instance();
    return position.x < 0 || position.x > DC->window_width ||
           position.y < 0 || position.y > DC->window_height;
}

Point Hero_bullet::get_position() const {
    return position;
}

double Hero_bullet::get_damage() const {
    return damage;
}

void Hero_bullet::setGifPath(const std::string& path) {
    gifPath = path;
}
