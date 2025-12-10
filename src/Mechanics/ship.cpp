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
    
    // Proximity Sensors (Raycasts)
    Ray_Left90,   
    Ray_Left45,   
    Ray_Front,    
    Ray_Right45,  
    Ray_Right90,  
    
    COUNT
};

class Ship : public Entity {
    private:
    static const int NUM_RAYS = 5;
    float ray_max_dist;            
    vector<float> ray_sensors;     

    public:

    float acceleration;            
    float drag;                    
    float angular_acceleration;    
    float angular_drag;            
    float score;                   
    float max_angular_velocity;    
    bool eh_o_melhor;
    
    Ship(float x, float y, int window_w, int window_h, Texture2D& ship_tex, unsigned int id)
    : Entity(x,y,window_w,window_h,ship_tex,id)
    {
        acceleration           = 0.4;
        angular_acceleration   = 0.01;
        angular_drag           = 0.95;
        max_angular_velocity   = 0.1;
        collisionradius        = 25;
        drag                   = 0.98;
        type                   = 0;
        score                  = 0;
        ray_max_dist           = 150;
        killable               = true;
        eh_o_melhor            = false;
        ray_sensors.resize(NUM_RAYS, 0.0f);
    }
    
    virtual vector<double> scan_inputs(){
        vector<double> output(4);
        output = {0,0,0,0};

        if(IsKeyDown(KEY_DOWN)) output[0] = 1;
        if(IsKeyDown(KEY_UP)) output[1] = 1;
        if(IsKeyDown(KEY_RIGHT)) output[2] = 1;
        if(IsKeyDown(KEY_LEFT)) output[3] = 1;

        return(output);
    }

    void movement(vector<double> inputs){
        if (inputs[2] > 0.5) angularvelocity += angular_acceleration;
        if (inputs[3] > 0.5) angularvelocity -= angular_acceleration;
        
        if (inputs[1] > 0.5){
            speeds.x += acceleration*cosf(facing_angle);
            speeds.y += acceleration*sinf(facing_angle);
        }
        
        if (inputs[0] > 0.5){
            speeds.x -= 0.6*acceleration*cosf(facing_angle);
            speeds.y -= 0.6*acceleration*sinf(facing_angle);
        }
    }

    void update() override
    {   
        if(!active) return; // If dead, freeze score

        Entity::update();
        if(type == 0) movement(scan_inputs());
        angularvelocity *= angular_drag;
        speeds = { speeds.x*(float)drag, speeds.y*(float)drag };

        if(angularvelocity > max_angular_velocity) angularvelocity = max_angular_velocity;
        if(angularvelocity < -max_angular_velocity) angularvelocity = -max_angular_velocity;

        // 1. Base reward for survival (incentivizes not crashing)
        float survival_reward = 0.5f; 

        // 2. Reward for speed/distance (incentivizes moving)
        // abs_speed is the magnitude of the current velocity.
        // Summing velocity every frame is mathematically equivalent to summing total distance.
        float distance_reward = abs_speed * 0.1f; 

        // 3. Light penalty for spinning too much (optional, helps avoid "drunk" ships spinning in place)
        float spin_penalty = abs(angularvelocity) * 2.0f;

        score += survival_reward + distance_reward - spin_penalty;
        
        // Ensures score never becomes negative
        if (score < 0) score = 0;
    }
    
    void set_melhor_nave(){ eh_o_melhor = true; }
    void reset_sensors() { std::fill(ray_sensors.begin(), ray_sensors.end(), 0.0f); }

    void sense_walls() {
        if (!active) return;
        float angles[] = { -90, -45, 0, 45, 90 };
        for(int i = 0; i < NUM_RAYS; i++) {
            float ang_rad = facing_angle + angles[i] * DEG2RAD;
            float dx = cosf(ang_rad);
            float dy = sinf(ang_rad);
            if (abs(dx) < 0.0001f) dx = 0.0001f;
            if (abs(dy) < 0.0001f) dy = 0.0001f;
            float dist_wall = ray_max_dist * 2.0f; 

            if (dx > 0) {
                float d = (screenWidth - position.x) / dx;
                if (d < dist_wall) dist_wall = d;
            } else {
                float d = (0 - position.x) / dx;
                if (d < dist_wall) dist_wall = d;
            }
            if (dy > 0) {
                float d = (screenHeight - position.y) / dy;
                if (d < dist_wall) dist_wall = d;
            } else {
                float d = (0 - position.y) / dy;
                if (d < dist_wall) dist_wall = d;
            }
            if (dist_wall < ray_max_dist) {
                float signal = 1.0f - (dist_wall / ray_max_dist);
                if (signal < 0) signal = 0;
                if (signal > 1) signal = 1;
                if (signal > ray_sensors[i]) ray_sensors[i] = signal;
            }
        }
    }

    void update_sensor_with_entity(Entity* other) {
        if (!active || !other->active) return;
        Vector2 to_obj = { other->get_position().x - position.x, other->get_position().y - position.y };
        float dist_sq = to_obj.x*to_obj.x + to_obj.y*to_obj.y;
        float max_reach = ray_max_dist + other->get_coll_radius();
        if (dist_sq > max_reach * max_reach) return;
        float dist = sqrtf(dist_sq);
        float angle_to_obj = atan2f(to_obj.y, to_obj.x) - facing_angle;
        while (angle_to_obj <= -PI) angle_to_obj += 2*PI;
        while (angle_to_obj > PI) angle_to_obj -= 2*PI;
        float deg = angle_to_obj * RAD2DEG;
        float sector_width = 25.0f; 
        int sensor_idx = -1;
        if      (deg >= -90 - sector_width && deg <= -90 + sector_width) sensor_idx = 0; 
        else if (deg >= -45 - sector_width && deg <= -45 + sector_width) sensor_idx = 1; 
        else if (deg >=   0 - sector_width && deg <=   0 + sector_width) sensor_idx = 2; 
        else if (deg >=  45 - sector_width && deg <=  45 + sector_width) sensor_idx = 3; 
        else if (deg >=  90 - sector_width && deg <=  90 + sector_width) sensor_idx = 4; 
        if (sensor_idx != -1) {
            float dist_to_surface = max(0.0f, dist - other->get_coll_radius());
            float signal = 1.0f - (dist_to_surface / ray_max_dist);
            if (signal < 0) signal = 0;
            if (signal > 1) signal = 1;
            if (signal > ray_sensors[sensor_idx]) ray_sensors[sensor_idx] = signal;
        }
    }

    vector<double> getSensors() const {
        vector<double> s((size_t)SensorType::COUNT);
        s[(int)SensorType::Speed]  = abs_speed / 20.0f; 
        s[(int)SensorType::SpeedX] = speeds.x / 20.0f;
        s[(int)SensorType::SpeedY] = speeds.y / 20.0f;
        s[(int)SensorType::Ray_Left90]  = ray_sensors[0];
        s[(int)SensorType::Ray_Left45]  = ray_sensors[1];
        s[(int)SensorType::Ray_Front]   = ray_sensors[2];
        s[(int)SensorType::Ray_Right45] = ray_sensors[3];
        s[(int)SensorType::Ray_Right90] = ray_sensors[4];
        return s;
    }

    void DrawExtra() override {
        if(!active) return;
        Entity::DrawExtra();
        float angles[] = { -90, -45, 0, 45, 90 };
        for(int i=0; i<NUM_RAYS; i++) {
            float val = ray_sensors[i];
            Color c = (val > 0) ? Color{255, (unsigned char)(255*(1-val)), 0, 200} : Color{0, 255, 0, 50};
            float len = ray_max_dist;
            float ang_rad = facing_angle + angles[i] * DEG2RAD;
            Vector2 end = { position.x + cosf(ang_rad) * len, position.y + sinf(ang_rad) * len };
            if(val > 0) {
                Vector2 hit = { position.x + cosf(ang_rad) * len * (1.0f - val), position.y + sinf(ang_rad) * len * (1.0f - val) };
                DrawLineV(position, hit, c);
                DrawCircleV(hit, 5, RED);
            }
        }
        DrawText(TextFormat("Score: %.0f", score), position.x + 20, position.y + 20, 10, GREEN);
        if(eh_o_melhor){
            DrawText("Previous Best",position.x + 20, position.y + 40, 15, GREEN);
            DrawCircleLines(position.x, position.y, collisionradius*2, GREEN);
        }
    }
};

#endif