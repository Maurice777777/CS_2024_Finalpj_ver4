#ifndef HERO_BULLET_H_INCLUDED
#define HERO_BULLET_H_INCLUDED

#include "./shapes/Point.h"
#include "./algif5/algif.h"
#include<string>

enum class Direction {
    FRONT,
    BACK,
    LEFT,
    RIGHT
};

class Hero_bullet {
public:
    Hero_bullet(Point start, double speed, Direction direction);
    void update(); // 更新位置
    void draw(); // 繪製子彈
    void init();
    bool is_out_of_bound() const; // 檢查是否超出視窗範圍
    Point get_position() const; // 獲取子彈位置
    double get_damage() const; // 獲取子彈傷害值
    void setGifPath(const std::string& path);

private:
    Point position;
    double speed;
    Direction direction; // 用於確定移動方向
    double damage;
    std::string gifPath;
    ALGIF_ANIMATION* gif;
};

#endif // HERO_BULLET_H_INCLUDED