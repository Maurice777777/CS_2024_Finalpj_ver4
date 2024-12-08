#include "Enemy_combat.h"
#include "../data/DataCenter.h"
#include "../data/GIFCenter.h"
#include "../algif5/algif.h"
#include "../shapes/Rectangle.h"
#include "../Hero.h"
#include <cstdio>
#include <cmath>

void EnemyCombat::follow_hero() 
{
    DataCenter* DC = DataCenter::get_instance();
    Hero* hero = DC->hero;

    double hero_x = hero->shape->center_x();
    double hero_y = hero->shape->center_y();

    double enemy_x = shape->center_x();
    double enemy_y = shape->center_y();

    // 計算方向向量
    double dx = hero_x - enemy_x;
    double dy = hero_y - enemy_y;
    double distance = std::sqrt(dx * dx + dy * dy);

    // 如果距離足夠大，讓敵人移動
    if (distance > 1e-5) // 避免過小距離導致數值不穩定
    { 
        dx /= distance;
        dy /= distance;

        // 更新敵人位置
        shape->update_center_x(enemy_x + dx * speed);
        shape->update_center_y(enemy_y + dy * speed);

        // 更新敵人方向（1-8）
        if (std::abs(dx) > std::abs(dy)) {
            direction = (dx > 0) ? 3 : 7; // 右 (3), 左 (7)
        } 
        else {
            direction = (dy > 0) ? 5 : 1; // 下 (5), 上 (1)
        }

        // 更新動畫
        gifPath = generate_gif_path(EnemyCombatState::WALK, direction);
    }
}
//犯陳于

EnemyCombat* EnemyCombat::create_mons(int x, int y) 
{
    return new EnemyCombat(x, y, EnemyCombatState::APPEAR);
}

EnemyCombat::EnemyCombat(int x, int y, EnemyCombatState state, int direction)
    : state(state), shape(nullptr), direction(direction), gifPath(""), monclose_erase(false),
      initial_x(x), initial_y(y), initial_direction(direction) // 初始化初始位置與方向
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
    switch (state) 
    {
        case EnemyCombatState::APPEAR: movement = "Appear"; break;
        case EnemyCombatState::RUN: movement = "Run"; break;
        case EnemyCombatState::WALK: movement = "Walk"; break;
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
                state = EnemyCombatState::WALK; // 假設進入 RUN 狀態
                gifPath = generate_gif_path(state, direction);
            }
            break;
        case EnemyCombatState::WALK:
            follow_hero();
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


void EnemyCombat::reset(int x, int y, EnemyCombatState initState) 
{
    state = initState; // 使用傳入的初始狀態
    direction = initial_direction;     // 默認方向
    HP = 50;           // 重置 HP

    int reset_x = (x == -1) ? initial_x : x;
    int reset_y = (y == -1) ? initial_y : y;

    // 更新 GIF 路徑
    gifPath = generate_gif_path(state, direction);

    // 重置範圍
    GIFCenter* GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION* gif = GIFC->get(gifPath);
    shape = std::make_unique<Rectangle>(
        reset_x - gif->width / 2, reset_y - gif->height / 2,
        reset_x + gif->width / 2, reset_y + gif->height / 2
    );


    monclose_erase = false; // 確保敵人不被標記為刪除
}
