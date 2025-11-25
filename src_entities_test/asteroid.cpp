#ifndef ASTEROID_H
#define ASTEROID_H

#include "raylib.h"
#include <cmath>
#include "entities.cpp"

class Asteroid : public Entity {
    private:
    int max_speed         = 50;
    int min_radius        = 50;
    int max_radius        = 150;

    public:

    Asteroid(float x, float y, int window_w, int window_h, Texture2D& ast_tex, unsigned int id)
    : Entity(x,y,window_w,window_h,ast_tex,id)
    {
        collisionradius     = GetRandomValue(min_radius,max_radius);
        speeds              = {0.01f*GetRandomValue(-max_speed,max_speed),0.01f*GetRandomValue(-max_speed,max_speed)};
        type                = 1;
    }
    
};

#endif