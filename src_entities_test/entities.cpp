#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"
#include <cmath>
#include <memory>
#include <string>

class Entity{
    private:

    public:

    Texture2D& texture;            //textura da entidade
    Rectangle source;              //parte da textura a se usar
    int screenHeight;              //entidade precisa saber o tamanho da tela
    int screenWidth;               //entidade precisa saber o tamanho da tela

    Vector2 position;              //posiçao x,y
    Vector2 speeds;                //velocidade x,y

    float speed_angle;             //angulo da velocidade da entidade (rad)
    float facing_angle;            //angulo que a entidade está olhando (rad)
    float angularvelocity;         //velocidade angular da entidade (rad)
    float abs_speed;               //modulo da velocidade
    float collisionradius;         //raio de colisao da entidade
    float distancemoved;           //distancia percorrida pela entidade
    

    unsigned int id;               //id da entidade
    unsigned int type;             //tipo da entidade (0 = jogador, 1 = asteroide, 2 = bot)

    Entity();

    Entity(float x, float y, int window_w, int window_h, Texture2D& ent_texture, unsigned int id)
    : texture(ent_texture),
    screenWidth(window_w),
    screenHeight(window_h),
    position({x,y}),
    source({ 0, 0, ent_texture.width/1.0f, ent_texture.height/1.0f }),
    distancemoved(0),
    speed_angle(0),
    angularvelocity(0),
    facing_angle(0),
    speeds({0,0}),
    abs_speed(0),
    collisionradius(0),
    id(id)
    {}

    virtual void update(){
        facing_angle += angularvelocity;
        position.x += speeds.x;
        position.y += speeds.y;
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
    }

    virtual void Draw(){
        Rectangle dest;
        switch(type){
            case (0):
            dest = {position.x, position.y, collisionradius*2.3f, collisionradius*2};
            break;
            default:
            dest = {position.x, position.y, collisionradius*2, collisionradius*2};
            break;
        }
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
        
        DrawCircleLines(position.x, position.y, collisionradius, RED);
    }

    void unstuck(Entity* other){
        int dist = sqrt(pow((this->position.x - other->position.x),2) + pow((this->position.y - other->position.y),2));
        if(this->collisionradius + other->collisionradius > dist-10){
            this->position.x -= 10;
            this->position.y -= 10;
            other->position.x += 10;
            other->position.y += 10;
            
        }
    }

    bool coll_check(Entity* other){
        int dist = sqrt(pow((this->position.x - other->position.x),2) + pow((this->position.y - other->position.y),2));
        if(this->collisionradius + other->collisionradius >= dist) return true;
        return false;
    }

    void coll_response(Entity* other){
        unstuck(other);
        DrawCircle((this->position.x+other->position.x)/2,(this->position.y+other->position.y)/2,50,Color{200,50,50,100});
        Vector2 temp = this->speeds;
        this->speeds = other->speeds;
        other->speeds = temp;
    }

};


#endif