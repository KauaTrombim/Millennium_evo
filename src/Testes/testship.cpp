#include "raylib.h"
#include <cmath>

class Ship{
    private:

        Texture ship_texture = LoadTexture("Testes/millenium.png");
        Rectangle sourceShip = { 0, 0, ship_texture.width/1.0f, ship_texture.height/1.0f };
        Vector2 originrec = {ship_texture.width/20.0f, ship_texture.height/20.0f};
        int screenHeight;
        int screenWidth;

    public:

        Vector2 shipposition;
        float ship_speed_angle;
        float ship_facing_angle;
        Vector2 shipspeeds;
        float shipangularvelocity;
        float max_angular_velocity;
        float abs_speed;
        float ship_acceleration;
        float ship_angular_acceleration;
        //construtor
        Ship(float x, float y, int windowWidth, int windowHeight){
            shipposition = { x , y};
            ship_speed_angle = 0;
            ship_facing_angle = 0;
            shipspeeds = { 0 , 0 };
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

        void scan_inputs(){
            if (IsKeyDown(KEY_RIGHT)) shipangularvelocity += ship_angular_acceleration;
            if (IsKeyDown(KEY_LEFT)) shipangularvelocity -= ship_angular_acceleration;
            if (IsKeyDown(KEY_UP)){
                shipspeeds.x += ship_acceleration*cosf(ship_facing_angle);
                shipspeeds.y += ship_acceleration*sinf(ship_facing_angle);
            }
            if (IsKeyDown(KEY_DOWN)){
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

    

    };