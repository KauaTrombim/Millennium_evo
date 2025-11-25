#ifndef SHIP_H
#define SHIP_H

#include <raylib.h>
#include <vector>
#include <cmath>
#include "entities.cpp"

using namespace std;

class Ship : public Entity {
private:

public:

    float acceleration;            //aceleracao linear
    float drag;                    //atrito linear
    float angular_acceleration;    //acelaracao angular 
    float angular_drag;            //atrito angular
    float max_angular_velocity;    //velocidade angular maxima
    
    //construtor
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
    }

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
        // Rotação para direita
        if (inputs[2] > 0.5) angularvelocity += angular_acceleration;
        
        // Rotação para esquerda
        if (inputs[3] > 0.5) angularvelocity -= angular_acceleration;
        
        // Acelerar
        if (inputs[1] > 0.5){
            speeds.x += acceleration*cosf(facing_angle);
            speeds.y += acceleration*sinf(facing_angle);
        }
        
        // Freiar
        if (inputs[0] > 0.5){
            speeds.x -= acceleration*cosf(facing_angle);
            speeds.y -= acceleration*sinf(facing_angle);
        }
    }

    void update() override
    {
        Entity::update();
        movement(scan_inputs());
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