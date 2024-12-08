#ifndef ENEMY_COMBAT_H_INCLUDED
#define ENEMY_COMBAT_H_INCLUDED
#include "../Object.h"
#include <string>
#include <memory>
#include "../shapes/Rectangle.h"

enum class EnemyCombatState 
{
    APPEAR,
    RUN,
    DIE,
    KNOCK,
    CROUCH,
    MONFIXSTATE_MAX
};

class EnemyCombat : public Object 
{
    public:
        void init(int x, int y);
        void update();
        void draw();
        static EnemyCombat* create_mons(int x, int y);
        EnemyCombat(int x, int y, EnemyCombatState state, int direction = 1);
        EnemyCombatState get_state() const { return state; }
        void set_state(EnemyCombatState new_state) { state = new_state; }
        bool monclose_erase;
        int get_width();
        int get_height();
        bool be_killed()const;
    private:
        std::string generate_gif_path(EnemyCombatState state, int direction);
        EnemyCombatState state;
        std::unique_ptr<Rectangle> shape; // 紀錄敵人的範圍
        int direction;                    // 敵人方向（1-8）
        std::string gifPath;              // 當前動作的 GIF 路徑
        int HP=50;
};

#endif
