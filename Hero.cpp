#include "Hero.h"
#include "data/DataCenter.h"
#include "data/GIFCenter.h"
#include "algif5/algif.h"
#include "shapes/Rectangle.h"
#include "Hero_bullet.h"
#include <iostream>
#include<cmath>
namespace HeroSetting
{
    static constexpr char gif_root_path[50]="./assets/gif/Hero";
    static constexpr char gif_postfix[][50]={
        "left",
        "right",
        "front",
        "back",
    };
}

void Hero::init()
{
    for(size_t type=0;type<static_cast<size_t>(HeroState::HEROSTATE_MAX);++type)
    {
        char buffer[50];
        sprintf(buffer,"%s/dragonite_%s.gif",
                HeroSetting::gif_root_path,
                HeroSetting::gif_postfix[static_cast<int>(type)]);
        gifPath[static_cast<HeroState>(type)] = std::string{buffer};
    }
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gifPath[state]);
    DataCenter *DC = DataCenter::get_instance();
    shape.reset(new Rectangle{DC->window_width/2,
                                DC->window_height/2,
                                DC->window_width/2+gif->width,
                                DC->window_height/2+gif->height});
}

void Hero::fire_bullet() {
    DataCenter *DC = DataCenter::get_instance();
    Direction bullet_direction;

    switch (state) {
        case HeroState::BACK:
            bullet_direction = Direction::FRONT;
            break;
        case HeroState::FRONT:
            bullet_direction = Direction::BACK;
            break;
        case HeroState::LEFT:
            bullet_direction = Direction::LEFT;
            break;
        case HeroState::RIGHT:
            bullet_direction = Direction::RIGHT;
            break;
        default:
            return;
    }

    Hero_bullet *bullet = new Hero_bullet(
        Point(shape->center_x(), shape->center_y()), 
        10.0, 
        bullet_direction
    );

    DC->herobullets.emplace_back(bullet);

    // std::cout << "Bullet created at position: (" 
    //           << shape->center_x() << ", " 
    //           << shape->center_y() << ") in direction " 
    //           << static_cast<int>(bullet_direction) << std::endl;
}


void Hero::draw()
{
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gifPath[state]);
    algif_draw_gif(gif,
                    shape->center_x()-gif->width/2,
                    shape->center_y()-gif->height/2,
                    0);
}


void Hero::update()
{
    DataCenter *DC = DataCenter::get_instance();
    static int fireCooldown=0;

    if(DC->key_state[ALLEGRO_KEY_W])
    {
        shape->update_center_y(shape->center_y()-speed);
        state = HeroState::BACK;
    }
    else if(DC->key_state[ALLEGRO_KEY_A])
    {
        shape->update_center_x(shape->center_x()-speed);
        state = HeroState::LEFT;
    }
    else if(DC->key_state[ALLEGRO_KEY_S])
    {
        shape->update_center_y(shape->center_y()+speed);
        state = HeroState::FRONT;
    }
    else if(DC->key_state[ALLEGRO_KEY_D])
    {
        shape->update_center_x(shape->center_x()+speed);
        state = HeroState::RIGHT;
    }

    // Fire bullet if cooldown allows
    if (fireCooldown == 0) {
        fire_bullet();
        fireCooldown = 30; // Cooldown duration in frames
    } else {
        fireCooldown--;
    }
}

void Hero::reset() 
{
    DataCenter *DC = DataCenter::get_instance();

    shape->update_center_x(DC->window_width / 2);
    shape->update_center_y(DC->window_height / 2);

    state = HeroState::FRONT;

    std::cout << "Hero position reset to center.\n";
}

