#ifndef HERO_H_INCLUDED
#define HERO_H_INCLUDED
#include <string>
#include <map>
#include "Object.h"

enum class HeroState
{
    LEFT,
    RIGHT,
    FRONT,
    BACK,
    HEROSTATE_MAX
};

class Hero : public Object
{
    public:
        void init();
        void update();
        void draw();
        void reset();

        // double get_center_x() const { return shape->center_x(); }
        // double get_center_y() const { return shape->center_y(); }
    private:
        void fire_bullet();
        HeroState state = HeroState::FRONT;
        double speed = 5;
        std::map<HeroState,std::string>gifPath;
        //std::unique_ptr<Rectangle> shape;
};

#endif