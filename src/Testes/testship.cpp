#include "raylib.h"
#include <cmath>

//parametros para o comportamento da nave
#define drag 0.99
#define accel 0.5
#define angaccel 0.01
#define maxang 0.1
#define angdrag 0.9
#define collrad 25

class Ship{
    private:

    Texture texture = LoadTexture("millenium.png");
    Rectangle source = { 0, 0, texture.width/1.0f, texture.height/1.0f };
    int screenHeight;
    int screenWidth;

    public:

    Vector2 position;
    Vector2 speeds;
    float speed_angle;
    float facing_angle;
    float angularvelocity;
    float max_angular_velocity;
    float abs_speed;
    float acceleration;
    float angular_acceleration;
    float collisionradius;
    float distancemoved;
    int bounce_on_edges;

    //construtor
    Ship() = default;
    Ship(float x, float y, int windowWidth, int windowHeight){
        position = { x , y };
        distancemoved = 0;
        speed_angle = 0;
        facing_angle = 0;
        speeds = { 0 , 0 };
        max_angular_velocity = maxang;
        acceleration = accel;
        abs_speed = 0;
        angular_acceleration = angaccel;
        screenHeight = windowHeight;
        screenWidth = windowWidth;
        collisionradius = collrad;
    }
    
    void update(){
        //update positions
        position.x += speeds.x;
        position.y += speeds.y;

        //update angular velocity and speeds
        facing_angle += angularvelocity;
        angularvelocity *= angdrag;
        speeds = { speeds.x*(float)drag, speeds.y*(float)drag };
        if(facing_angle > 360*DEG2RAD) facing_angle -= DEG2RAD*360.0;
        if(facing_angle < 0) facing_angle += DEG2RAD*360.0;
        abs_speed = sqrt(speeds.x*speeds.x + speeds.y*speeds.y);
        distancemoved += abs_speed;
        speed_angle = atan2f(speeds.y, speeds.x);            

        //screen wrap
        /*if(position.x > screenWidth) position.x = 0;
        if(position.x < 0) position.x = screenWidth;
        if(position.y > screenHeight) position.y = 0;
        if(position.y < 0) position.y = screenHeight;*/

        //screen edge bounce
        if(position.x + speeds.x >= screenWidth || position.x + speeds.x <= 0){
            position.x -= speeds.x;
            speeds.x *= -1;
        }
        if(position.y + speeds.y >= screenHeight || position.y + speeds.y <= 0){
            position.y -= speeds.y;
            speeds.y *= -1;
        }

        //cap ship angular velocity
        if(angularvelocity > max_angular_velocity) angularvelocity = max_angular_velocity;
        if(angularvelocity < -max_angular_velocity) angularvelocity = -max_angular_velocity;
    }

    void scan_inputs(){
        if (IsKeyDown(KEY_RIGHT)) angularvelocity += angular_acceleration;
        if (IsKeyDown(KEY_LEFT)) angularvelocity -= angular_acceleration;
        if (IsKeyDown(KEY_UP)){
            speeds.x += acceleration*cosf(facing_angle);
            speeds.y += acceleration*sinf(facing_angle);
        }
        if (IsKeyDown(KEY_DOWN)){
            speeds.x -= 0.5*cosf(facing_angle);
            speeds.y -= 0.5*sinf(facing_angle);
        }
    }

    void Draw(){
        Rectangle dest = {position.x, position.y,collisionradius*2.5f,collisionradius*2.0f};
        Vector2 origin = {dest.width/2.0f, dest.height/2.0f};
        DrawTexturePro(texture, source, dest, origin, RAD2DEG*facing_angle, WHITE);
    }

    void DrawExtra(){
        Vector2 velocity_endpoint = {position.x + 10*speeds.x, position.y + 10*speeds.y};
        Vector2 arrowhead_1 = {velocity_endpoint.x - abs_speed*cosf(speed_angle+DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(speed_angle+DEG2RAD*30)};
        Vector2 arrowhead_2 = {velocity_endpoint.x - abs_speed*cosf(speed_angle-DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(speed_angle-DEG2RAD*30)};
        Vector2 arrowhead_3 = {velocity_endpoint.x, velocity_endpoint.y};
        DrawLineV(position, velocity_endpoint, RED);
        DrawTriangle(arrowhead_1, arrowhead_2, arrowhead_3, RED);
        
        DrawCircleLines(position.x, position.y, collisionradius, BLACK);

        DrawCircleSector(position,250,facing_angle*RAD2DEG+30,facing_angle*RAD2DEG,15,Color{10,120,200,100});
        DrawCircleSector(position,250,facing_angle*RAD2DEG,facing_angle*RAD2DEG-30,15,Color{200,10,120,100});
    }



    };