#ifndef SHIP
#define SHIP

#include <raylib.h>
#include <vector>
#include <raymath.h>
#include <cmath>
#include "entity.cpp"

using namespace std;

class Ship : public Entity {
    private:

    float acceleration;            //linear acceleration
    float drag;                    //linear drag
    float angular_acceleration;    //angular acceleration
    float angular_drag;            //angular drag
    float max_angular_velocity;    //angular speed cap

    public:
    
    // constructor -----------------------------------------------------------------------------
    Ship(float x, float y, int window_w, int window_h, Texture2D& ship_tex, unsigned int id)
    : Entity(x,y,window_w,window_h,ship_tex,id)
    {
        acceleration           = 0.5;
        angular_acceleration   = 0.01;
        angular_drag           = 0.95;
        max_angular_velocity   = 0.1;
        collisionradius        = 25;
        drag                   = 0.99;
        type                   = 0;
        killable               = true;
    }

    // methods ----------------------------------------------------------------------------------
    vector<double> scan_inputs(){
        vector<double> output(4);
        output = {0,0,0,0};

        if(IsKeyDown(KEY_DOWN)){
            output[0] = 1;
        }
        if(IsKeyDown(KEY_UP)){
            output[1] = 1;
        }
        if(IsKeyDown(KEY_RIGHT)){
            output[2] = 1;
        }
        if(IsKeyDown(KEY_LEFT)){
            output[3] = 1;
        }

        return(output);
    }

    void movement(vector<double> inputs){
        // rotate right
        if (inputs[2] > 0.5) angularvelocity += angular_acceleration;
        
        // rotate left
        if (inputs[3] > 0.5) angularvelocity -= angular_acceleration;
        
        // thrust forward
        if (inputs[1] > 0.5){
            speeds.x += acceleration*cosf(facing_angle);
            speeds.y += acceleration*sinf(facing_angle);
        }
        
        // thrust backward
        if (inputs[0] > 0.5){
            speeds.x -= 0.7*acceleration*cosf(facing_angle);
            speeds.y -= 0.7*acceleration*sinf(facing_angle);
        }
    }

    void update() override
    {   
        if(!active) return;

        Entity::update();
        movement(scan_inputs());
        angularvelocity *= angular_drag;
        speeds = { speeds.x*(float)drag, speeds.y*(float)drag };

        //cap ship angular velocity
        if(angularvelocity > max_angular_velocity) angularvelocity = max_angular_velocity;
        if(angularvelocity < -max_angular_velocity) angularvelocity = -max_angular_velocity;
    }

    //todo
    //bool check_fov(float start_angle, float end_angle, int max_dist){
    //}



};

#endif