#include "Enemy_combat.h"
#include "../data/DataCenter.h"
#include "../data/GIFCenter.h"
#include "../algif5/algif.h"
#include "../shapes/Rectangle.h"
#include <cstdio>

EnemyCombat* EnemyCombat::create_mons(int x, int y) 
{
    return new EnemyCombat(x, y, EnemyCombatState::APPEAR);
}

EnemyCombat::EnemyCombat(int x, int y, EnemyCombatState state, int direction)
    : state(state), shape(nullptr), direction(direction), gifPath(""), monclose_erase(false) 
{
    gifPath = generate_gif_path(state, direction);

    GIFCenter* GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION* gif = GIFC->get(gifPath);
    shape = std::make_unique<Rectangle>(
        x - gif->width / 2, y - gif->height / 2,
        x + gif->width / 2, y + gif->height / 2
    );
}


std::string EnemyCombat::generate_gif_path(EnemyCombatState state, int direction) 
{
    // 根據狀態生成對應的資料夾名稱
    std::string movement;
    switch (state) {
        case EnemyCombatState::APPEAR: movement = "Appear"; break;
        case EnemyCombatState::RUN: movement = "Run"; break;
        case EnemyCombatState::DIE: movement = "Die"; break;
        case EnemyCombatState::KNOCK: movement = "Knock"; break;
        case EnemyCombatState::CROUCH: movement = "Crouch"; break;
        default: movement = "Unknown"; break;
    }

    char buffer[150];
    sprintf(buffer, "./assets/gif/MonsterClose/%s/%s_dir%d.gif", 
            movement.c_str(), movement.c_str(), direction);
    return std::string(buffer);
}

void EnemyCombat::update() 
{
    GIFCenter* GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION* gif = GIFC->get(gifPath);

    switch (state) 
    {
        case EnemyCombatState::APPEAR:
            if (gif->done) 
            {
                //state = EnemyCombatState::RUN; // 假設進入 RUN 狀態
                gifPath = generate_gif_path(state, direction);
            }
            break;

        case EnemyCombatState::DIE:
            if (gif->done) 
            {
                monclose_erase = true;
            }
            break;

        default:
            break;
    }
}

void EnemyCombat::draw() 
{
    GIFCenter* GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION* gif = GIFC->get(gifPath);
    algif_draw_gif(gif,
                   shape->center_x() - gif->width / 2,
                   shape->center_y() - gif->height / 2, 0);
}

int EnemyCombat::get_width() 
{
    GIFCenter* GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION* gif = GIFC->get(gifPath);
    return gif->width;
}

int EnemyCombat::get_height() 
{
    GIFCenter* GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION* gif = GIFC->get(gifPath);
    return gif->height;
}

bool EnemyCombat::be_killed()const
{
    return (HP<=0 || state==EnemyCombatState::DIE);
}
