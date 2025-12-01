#ifndef BOTSHIP
#define BOTSHIP

#include <raylib.h>
#include <vector>
#include <cmath>
#include "entity.cpp"

using namespace std;

class BotShip : public Entity {
    private:

    float acceleration;            //linear acceleration
    float drag;                    //linear drag
    float angular_acceleration;    //angular acceleration
    float angular_drag;            //angular drag
    float max_angular_velocity;    //angular velocity cap

    public:
    
    // constructor -----------------------------------------------------------------------------
    BotShip(float x, float y, int window_w, int window_h, Texture2D& ship_tex, unsigned int id)
    : Entity(x,y,window_w,window_h,ship_tex,id)
    {
        acceleration              = 0.5;
        angular_acceleration      = 0.01;
        angular_drag              = 0.9;
        max_angular_velocity      = 0.1;
        collisionradius           = 20;
        drag                      = 0.99;
        type                      = 2;
    }

    void movement(vector<double> inputs){
        // Rotação para direita
        if (inputs[2] > 0.25) angularvelocity += angular_acceleration;
        
        // Rotação para esquerda
        if (inputs[3] > 0.25) angularvelocity -= angular_acceleration;
        
        // Acelerar
        if (inputs[1] > 0.25){
            speeds.x += acceleration*cosf(facing_angle);
            speeds.y += acceleration*sinf(facing_angle);
        }
        
        // Freiar
        if (inputs[0] > 0.25){
            speeds.x -= acceleration*cosf(facing_angle);
            speeds.y -= acceleration*sinf(facing_angle);
        }
    }

    void update() {
        Entity::update();

        angularvelocity *= angular_drag;
        speeds = { speeds.x*(float)drag, speeds.y*(float)drag };

        //cap ship angular velocity
        if(angularvelocity > max_angular_velocity) angularvelocity = max_angular_velocity;
        if(angularvelocity < -max_angular_velocity) angularvelocity = -max_angular_velocity;
    }

    //vector<double> getSensors() const {
    //}



};

#endif