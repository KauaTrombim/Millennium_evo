#ifndef SHIP_H
#define SHIP_H

#include "rng.cpp"
#include <string>
#include <iostream>
#include <raylib.h>

#define M_PI 3.14159265358979323846
using namespace std;

enum class SensorType {
            Velocidade,
            //ObjetoDetectado,
            //ObjetoDx,
            //ObjetoDy,
            // adicionar mais dps
            COUNT
        };
class Ship{
private:

    Texture ship_texture = LoadTexture("../assets/xwing.png");
    float ship_width = ship_texture.width/1.0f;
    float ship_height = ship_texture.height/1.0f;
    Rectangle sourceShip = { 0, 0, ship_texture.width/1.0f, ship_texture.height/1.0f };
    Vector2 originrec = {ship_texture.width/20.0f, ship_texture.height/20.0f};
    int screenHeight;
    int screenWidth;

public:

    Vector2 shipposition;         // posicao (x,y) da nave
    float ship_speed_angle;                 // Velocidade angular
    float ship_facing_angle;          // Angulo da frente da nave
    Vector2 shipspeeds;                       // Componentes Vx Vy da velocidade total
    float shipangularvelocity;      // Velocidade angular da nave
    float max_angular_velocity;            // Velocidade angular max
    float abs_speed;                   // Modulo da velocidade linear
    float ship_acceleration;                        // Aceleracao linear
    float ship_angular_acceleration;          // Aceleracao angular
    
    //construtor
    Ship() = default;

    Ship(int windowWidth, int windowHeight, float min_axis_speed, float max_axis_speed){

        Random rng = Random();         // Gerador de numero aleatorio

        shipposition = { (float)rng.random_double(ship_width, windowWidth - ship_width) , 
                        (float)rng.random_double(ship_height, windowHeight - ship_height)};
        ship_speed_angle = 0;
        ship_facing_angle = (float)rng.random_double(0, 2*M_PI);
        shipspeeds = { (float)rng.random_double(min_axis_speed, max_axis_speed) , 
                        (float)rng.random_double(min_axis_speed, max_axis_speed) };
        max_angular_velocity = 0.1;
        ship_acceleration = 0.5;
        abs_speed = 0;
        ship_angular_acceleration = 0.01;
        screenHeight = windowHeight;
        screenWidth = windowWidth;
    }
    
    void update(){
        //update positions
        shipposition.x += shipspeeds.x;
        shipposition.y += shipspeeds.y;

        //update angular velocity and speeds
        ship_facing_angle += shipangularvelocity;
        shipangularvelocity *= 0.9;
        shipspeeds = { shipspeeds.x*0.98f, shipspeeds.y*0.98f };
        if(ship_facing_angle > 360*DEG2RAD) ship_facing_angle -= DEG2RAD*360.0;
        if(ship_facing_angle < 0) ship_facing_angle += DEG2RAD*360.0;
        abs_speed = sqrt(shipspeeds.x*shipspeeds.x + shipspeeds.y*shipspeeds.y);
        ship_speed_angle = atan2f(shipspeeds.y, shipspeeds.x);
        
        

        //screen wrap
        if(shipposition.x > screenWidth) shipposition.x = 0;
        if(shipposition.x < 0) shipposition.x = screenWidth;
        if(shipposition.y > screenHeight) shipposition.y = 0;
        if(shipposition.y < 0) shipposition.y = screenHeight;

        //cap ship angular velocity
        if(shipangularvelocity > max_angular_velocity) shipangularvelocity = max_angular_velocity;
        if(shipangularvelocity < -max_angular_velocity) shipangularvelocity = -max_angular_velocity;
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
        if (inputs[2] > 0.5) shipangularvelocity += ship_angular_acceleration;
        
        // Rotação para esquerda
        if (inputs[3] > 0.5) shipangularvelocity -= ship_angular_acceleration;
        
        // Acelerar
        if (inputs[1] > 0.5){
            shipspeeds.x += ship_acceleration*cosf(ship_facing_angle);
            shipspeeds.y += ship_acceleration*sinf(ship_facing_angle);
        }
        
        // Freiar
        if (inputs[0] > 0.5){
            shipspeeds.x -= 0.5*cosf(ship_facing_angle);
            shipspeeds.y -= 0.5*sinf(ship_facing_angle);
        }
    }

    void Draw(){
        Rectangle destrec = {shipposition.x, shipposition.y,ship_texture.width/10.0f,ship_texture.height/10.0f};
        DrawTexturePro(ship_texture, sourceShip, destrec, originrec, RAD2DEG*ship_facing_angle, WHITE);
    }

    void DrawExtra(){
        Vector2 velocity_endpoint = {shipposition.x + 10*shipspeeds.x, shipposition.y + 10*shipspeeds.y};
        Vector2 arrowhead_1 = {velocity_endpoint.x - abs_speed*cosf(ship_speed_angle+DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(ship_speed_angle+DEG2RAD*30)};
        Vector2 arrowhead_2 = {velocity_endpoint.x - abs_speed*cosf(ship_speed_angle-DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(ship_speed_angle-DEG2RAD*30)};
        Vector2 arrowhead_3 = {velocity_endpoint.x, velocity_endpoint.y};
        DrawLineV(shipposition, velocity_endpoint, RED);
        DrawTriangle(arrowhead_1, arrowhead_2, arrowhead_3, RED);

        DrawCircleSector(shipposition,250,ship_facing_angle*RAD2DEG+30,ship_facing_angle*RAD2DEG-30,15,Color{10,120,200,100});
    }

    vector<double> getSensors() const {
        vector<double> s((size_t)SensorType::COUNT);

        s[(int)SensorType::Velocidade]     = abs_speed;
        //s[(int)SensorType::ObjetoDetectado]= objetoDetectado ? 1.0 : 0.0;
        //s[(int)SensorType::ObjetoDx]       = objetoDx;
        //s[(int)SensorType::ObjetoDy]       = objetoDy;

        return s;
    }



};

#endif