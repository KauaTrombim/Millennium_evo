#ifndef SHIP
#define SHIP

#include <raylib.h>
#include <vector>
#include <raymath.h>
#include <cmath>
#include "entity.cpp"

using namespace std;

enum class SensorType {
        Speed,
        SpeedX,
        SpeedY,
        ObjDetect,
        //ObjetoDx,
        //ObjetoDy,
        // adicionar mais dps
        COUNT
    };

class Ship : public Entity {
    private:

    bool in_fov_radius(Entity* other){
        if(!active || !other->active) return false;
        float dist = Vector2Distance(this->position, other->position);
        if(this->fovradius + other->collisionradius >= dist) return true;
        return false;
    }

    public:

    float acceleration;            //linear acceleration
    float drag;                    //linear drag
    float angular_acceleration;    //angular acceleration
    float angular_drag;            //angular drag
    float max_angular_velocity;    //angular speed cap
    float fovradius;               // prov para bot_nave
    float fov_width;
    bool fov_detect;               //prov, remover dps
    
    // constructor -----------------------------------------------------------------------------
    Ship(float x, float y, int window_w, int window_h, Texture2D& ship_tex, unsigned int id)
    : Entity(x,y,window_w,window_h,ship_tex,id)
    {
        acceleration           = 0.4;
        angular_acceleration   = 0.01;
        angular_drag           = 0.95;
        max_angular_velocity   = 0.1;
        collisionradius        = 18;
        fov_width              = 90;
        fovradius              = 100;
        drag                   = 0.98;
        type                   = 0;
        killable               = true;
        fov_detect             = false;
    }

    // setters e getters ------------------------------------------------------------------------
    void set_fov_detect(bool detect){
        fov_detect = detect;
        return;
    }

    float get_fov_radius(){
        return fovradius;
    }
    
    // methods ----------------------------------------------------------------------------------
    virtual vector<double> scan_inputs(){
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

    vector<double> getSensors() const {
        vector<double> s((size_t)SensorType::COUNT);

        s[(int)SensorType::Speed]     = abs_speed;
        s[(int)SensorType::SpeedX]    = speeds.x;
        s[(int)SensorType::SpeedY]    = speeds.y;
        s[(int)SensorType::ObjDetect] = fov_detect ? 1.0 : 0.0;
        //s[(int)SensorType::ObjetoDx]       = objetoDx;
        //s[(int)SensorType::ObjetoDy]       = objetoDy;
        return(s);
    }

    bool check_fov_border(){
        if (!active) return false;

        // check if screen edge is in fov
        if (position.x + fovradius*cos(facing_angle) > screenWidth || position.x + fovradius*cos(facing_angle) < 0||
            position.x + fovradius*cos(facing_angle+DEG2RAD*fov_width) > screenWidth || position.x + fovradius*cos(facing_angle+DEG2RAD*fov_width) < 0||
            position.x + fovradius*cos(facing_angle-DEG2RAD*fov_width) > screenWidth || position.x + fovradius*cos(facing_angle-DEG2RAD*fov_width) < 0||
            position.y + fovradius*sin(facing_angle) > screenHeight || position.y + fovradius*sin(facing_angle) < 0||
            position.y + fovradius*sin(facing_angle+DEG2RAD*fov_width) > screenHeight || position.y + fovradius*sin(facing_angle+DEG2RAD*fov_width) < 0||
            position.y + fovradius*sin(facing_angle-DEG2RAD*fov_width) > screenHeight || position.y + fovradius*sin(facing_angle-DEG2RAD*fov_width) < 0
        ){
            return true;
        }
        return false;

    }

    bool check_fov(Entity* target)
    {
        if (!active || !target->active) return false;

        // Distância entre centros
        float dx = target->get_position().x - position.x;
        float dy = target->get_position().y - position.y;
        float dist2 = dx*dx + dy*dy;

        float maxDist = fovradius + target->collisionradius;
        if (dist2 > maxDist * maxDist)
            return false;  // nem perto o suficiente

        float dist = sqrtf(dist2);

        // Vetor direção normalizado para o alvo
        float tx = dx / dist;
        float ty = dy / dist;

        // Direção da nave
        float fx = cosf(facing_angle);
        float fy = sinf(facing_angle);

        // Ângulo entre nave → alvo
        float dot = fx * tx + fy * ty; // = cos(angleBetween)

        // FOV base (±30°)
        float halfFov = DEG2RAD * fov_width;

        // Alargamento angular devido ao raio do alvo
        float delta = 0.0f;
        if (dist > 0 && target->collisionradius > 0) {
            float ratio = target->collisionradius / dist;
            if (ratio > 1.0f) ratio = 1.0f;       // clamp
            delta = asinf(ratio);
        }

        // Limite real
        float cosLimit = cosf(halfFov + delta);

        // Teste final
        return dot >= cosLimit;
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
            speeds.x -= 0.6*acceleration*cosf(facing_angle);
            speeds.y -= 0.6*acceleration*sinf(facing_angle);
        }
    }

    void update() override
    {   
        if(!active) return;

        Entity::update();
        if(type == 0) movement(scan_inputs());
        angularvelocity *= angular_drag;
        speeds = { speeds.x*(float)drag, speeds.y*(float)drag };

        //cap ship angular velocity
        if(angularvelocity > max_angular_velocity) angularvelocity = max_angular_velocity;
        if(angularvelocity < -max_angular_velocity) angularvelocity = -max_angular_velocity;
    }

    void DrawExtra() override {
        if(!active) return;

        Vector2 velocity_endpoint = {position.x + 10*speeds.x, position.y + 10*speeds.y};
        Vector2 arrowhead_1 = {velocity_endpoint.x - abs_speed*cosf(speed_angle+DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(speed_angle+DEG2RAD*30)};
        Vector2 arrowhead_2 = {velocity_endpoint.x - abs_speed*cosf(speed_angle-DEG2RAD*30.0), velocity_endpoint.y - abs_speed*sinf(speed_angle-DEG2RAD*30)};
        Vector2 arrowhead_3 = {velocity_endpoint.x, velocity_endpoint.y};
        DrawLineV(position, velocity_endpoint, RED);
        DrawTriangle(arrowhead_1, arrowhead_2, arrowhead_3, RED);
        
        DrawCircleLines(position.x, position.y, collisionradius, RED);

        if(fov_detect == 0){
            DrawCircleSector(position,fovradius,facing_angle*RAD2DEG+fov_width,facing_angle*RAD2DEG,15,Color{10,120,200,100});
            DrawCircleSector(position,fovradius,facing_angle*RAD2DEG,facing_angle*RAD2DEG-fov_width,15,Color{200,10,120,100});
        }
        else if(fov_detect == 1){
            DrawCircleSector(position,fovradius,facing_angle*RAD2DEG+fov_width,facing_angle*RAD2DEG-fov_width,15,Color{10,200,120,100});
        }
        
    }


};

#endif