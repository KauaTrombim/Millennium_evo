#ifndef BOTSHIP
#define BOTSHIP

#include <raylib.h>
#include <vector>
#include <cmath>
#include "entity.cpp"
#include "ship.cpp"

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

class BotShip : public Ship {
    private:

    float acceleration;            //linear acceleration
    float drag;                    //linear drag
    float angular_acceleration;    //angular acceleration
    float angular_drag;            //angular drag
    float max_angular_velocity;    //angular velocity cap

    float fovradius;   // prov para bot_nave
    bool fov_detect;  //prov, remover dps

    bool in_fov_radius(Entity* other){
        if(!active || !other->active) return false;
        float dist = Vector2Distance(this->position, other->position);
        if(this->fovradius + other->collisionradius >= dist) return true;
        return false;
    }

    public:
    
    // constructor -----------------------------------------------------------------------------
    BotShip(float x, float y, int window_w, int window_h, Texture2D& ship_tex, unsigned int id)
    : Ship(x,y,window_w,window_h,ship_tex,id)
    {
        type                      = 2;
        fovradius              = 250;
        fov_detect             = 0;
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

    void update() override{
        Entity::update();

        angularvelocity *= angular_drag;
        speeds = { speeds.x*(float)drag, speeds.y*(float)drag };

        //cap ship angular velocity
        if(angularvelocity > max_angular_velocity) angularvelocity = max_angular_velocity;
        if(angularvelocity < -max_angular_velocity) angularvelocity = -max_angular_velocity;
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
        float halfFov = DEG2RAD * 30.0f;

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
            DrawCircleSector(position,250,facing_angle*RAD2DEG+30,facing_angle*RAD2DEG,15,Color{10,120,200,100});
            DrawCircleSector(position,250,facing_angle*RAD2DEG,facing_angle*RAD2DEG-30,15,Color{200,10,120,100});
        }
        else if(fov_detect == 1){
            DrawCircleSector(position,250,facing_angle*RAD2DEG+30,facing_angle*RAD2DEG-30,15,GREEN);
        }
        
    }
};

#endif