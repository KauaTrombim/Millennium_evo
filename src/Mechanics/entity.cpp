#ifndef ENTITY
#define ENTITY

#include "raylib.h"
#include <raymath.h>
#include <cmath>
#include <memory>
#include <string>

class Entity{
    private:

    Texture2D& texture;            //texture to use
    Rectangle source;              //region of the texture to use  

    public:

    int screenHeight, screenWidth; //entity needs to know screen size   

    Vector2 position;              //position x,y
    Vector2 speeds;                //velocity x,y

    float speed_angle;             //angle of the speed vector (rad)
    float facing_angle;            //direction the entity is facing (rad)
    float angularvelocity;         //angular speed (rad)
    float abs_speed;               //speed modulo
    float collisionradius;         //collision radius
    float distancemoved;           //distance moved in entity's lifetime
    
    bool active;                   //is entity active
    bool killable;                 //is entity killable
    unsigned int id;               //entity id
    unsigned int type;             //entity type (0 = player, 1 = asteroid, 2 = bot)

    // constructor -----------------------------------------------------------------------------

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
    id(id),
    active(true),
    killable(false)
    {}
    // getters e setters ------------------------------------------------------------------------

    Vector2 get_position(){
        return(position);
    }

    float get_coll_radius(){
        return(collisionradius);
    }

    float distance_moved(){
        return(distancemoved);
    }


    // methods ----------------------------------------------------------------------------------
    virtual void update(){
        if(!active) return;

        facing_angle += angularvelocity;
        position.x += speeds.x;
        position.y += speeds.y;
        if(facing_angle > 360*DEG2RAD) facing_angle -= DEG2RAD*360.0;
        if(facing_angle < 0) facing_angle += DEG2RAD*360.0;
        abs_speed = sqrt(speeds.x*speeds.x + speeds.y*speeds.y);
        distancemoved += abs_speed;
        speed_angle = atan2f(speeds.y, speeds.x);

        //screen wrap
        /*
        if(position.x > screenWidth) position.x = 0;
        if(position.x < 0) position.x = screenWidth;
        if(position.y > screenHeight) position.y = 0;
        if(position.y < 0) position.y = screenHeight;
        */
        
        //screen edge behavior
        
        if(position.x + speeds.x >= screenWidth || position.x + speeds.x <= 0){
                //bounce off of walls
                position.x -= speeds.x;
                speeds.x *= -1;
                if(killable) kill();
        }
        if(position.y + speeds.y >= screenHeight || position.y + speeds.y <= 0){
                position.y -= speeds.y;
                speeds.y *= -1;
                if(killable) kill();
        }
        
    }

    virtual void Draw(){
        if(!active) return;

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

    virtual void DrawExtra(){
        if(!active) return;

        Vector2 velocity_endpoint = {position.x + 10*speeds.x, position.y + 10*speeds.y};
        Vector2 arrowhead_1 = {velocity_endpoint.x - abs_speed*cosf(speed_angle+DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(speed_angle+DEG2RAD*30)};
        Vector2 arrowhead_2 = {velocity_endpoint.x - abs_speed*cosf(speed_angle-DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(speed_angle-DEG2RAD*30)};
        Vector2 arrowhead_3 = {velocity_endpoint.x, velocity_endpoint.y};
        DrawLineV(position, velocity_endpoint, RED);
        DrawTriangle(arrowhead_1, arrowhead_2, arrowhead_3, RED);
        
        DrawCircleLines(position.x, position.y, collisionradius, RED);
    }
    
    void kill(){
        active = false;
    }

    void unstuck(Entity* other, float dist){
        if(dist == 0) return;
        float overlap = this->collisionradius + other->collisionradius - dist;
        if(overlap >= 0){
            Vector2 normal = {(other->position.x - this->position.x)/dist, (other->position.y - this->position.y)/dist};
            this->position.x -= normal.x*overlap*0.5f;
            this->position.y -= normal.y*overlap*0.5f;
            other->position.x += normal.x*overlap*0.5f;
            other->position.y += normal.y*overlap*0.5f;
        }
    }

    bool coll_check(Entity* other){
        if(!active || !other->active) return false;
        float dist = Vector2Distance(this->position, other->position);
        if(this->collisionradius + other->collisionradius >= dist) return true;
        return false;


    }

    void coll_response(Entity* other){
        float dist = Vector2Distance(this->position, other->position);
        unstuck(other, dist);
        if(this->killable) kill();
        if(other->killable) other->kill();
        
        //todo: realistic collision response
        //Vector2 normal = {other->position.x - this->position.x, other->position.y - this->position.y};
        //Vector2 rel_veloc = {other->speeds.x - };
    }

    void randomize_position(){
        position = { 1.0f*GetRandomValue(0,screenWidth) , 1.0f*GetRandomValue(0,screenHeight)};
    }

};


#endif
