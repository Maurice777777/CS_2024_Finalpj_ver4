#include "Hero_bullet.h"
#include <cmath>
#include <allegro5/allegro_primitives.h>
#include "data/DataCenter.h"
#include "data/GIFCenter.h"
#include <iostream>

constexpr double PI = 3.14159265358979323846;
constexpr double DEG_TO_RAD = PI / 180.0;

Hero_bullet::Hero_bullet(Point start, double speed, Direction direction)
    : position(start), speed(speed), direction(direction), damage(10.0) {
    init();
}

void Hero_bullet::init() {
    GIFCenter *GIFC = GIFCenter::get_instance();
    gif = GIFC->get("./assets/gif/Hero/fireball.gif"); // 子彈 GIF 的路徑
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
    std::cout << "Bullet updated to position: (" 
              << position.x << ", " << position.y << ") in direction " 
              << static_cast<int>(direction) << std::endl;
}

void Hero_bullet::draw() {
    if (gif != nullptr) {
        // Extract the current frame of the GIF
        ALLEGRO_BITMAP* frame_bitmap = algif_get_bitmap(gif, al_get_time());
        if (!frame_bitmap) {
            std::cerr << "Error: Failed to extract frame from GIF." << std::endl;
            return;
        }

        // Get frame dimensions
        int frame_width = al_get_bitmap_width(frame_bitmap);
        int frame_height = al_get_bitmap_height(frame_bitmap);

        // Calculate the rotation angle based on the direction
        float angle = 0.0;
        switch (direction) {
            case Direction::FRONT: // Up
                angle = PI / 2;
                break;
            case Direction::BACK: // Down
                angle = -PI / 2;
                break;
            case Direction::LEFT: // Left
                angle = 0;
                break;
            case Direction::RIGHT: // Right
                angle = PI;
                break;
        }

        // Draw the rotated bitmap
        al_draw_rotated_bitmap(
            frame_bitmap,          // Current frame
            frame_width / 2,       // Rotation center x (center of the bitmap)
            frame_height / 2,      // Rotation center y
            position.x,            // Screen position x
            position.y,            // Screen position y
            angle,                 // Rotation angle (radians)
            0                      // Flags (e.g., no flip)
        );
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
