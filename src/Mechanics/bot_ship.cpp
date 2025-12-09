#ifndef BOTSHIP
#define BOTSHIP

#include <raylib.h>
#include <vector>
#include <cmath>
#include <algorithm> // Necessário para max()
#include "entity.cpp"
#include "ship.cpp"

using namespace std;

// Definição dos Inputs da Rede Neural
enum class SensorType {
    Speed,
    SpeedX,
    SpeedY,
    
    // Sensores de Proximidade (Raycasts)
    Ray_Left90,   // Esquerda total
    Ray_Left45,   // Diagonal Esquerda
    Ray_Front,    // Frente
    Ray_Right45,  // Diagonal Direita
    Ray_Right90,  // Direita total
    
    COUNT
};

class BotShip : public Ship {
private:
    // Configurações dos sensores
    static const int NUM_RAYS = 5;
    float ray_max_dist;     // Alcance máximo da visão
    vector<float> ray_sensors; // Armazena os valores atuais (0.0 a 1.0)

public:
    
    BotShip(float x, float y, int window_w, int window_h, Texture2D& ship_tex, unsigned int id)
    : Ship(x,y,window_w,window_h,ship_tex,id)
    {
        type = 2;
        
        collisionradius = 18;
        
        // Configuração dos Sensores Raycast
        ray_max_dist = 300.0f; 
        ray_sensors.resize(NUM_RAYS, 0.0f);
    }

    // --- CORREÇÃO PRINCIPAL: O BOT DEVE TER SEU PRÓPRIO UPDATE ---
    // Isso impede que ele use o update da Ship, que lê o teclado.
    void update() override {
        // 1. Atualiza posição baseada na velocidade atual (Inércia)
        Entity::update(); 

        // 2. Aplica atrito (Drag)
        angularvelocity *= angular_drag;
        speeds = { speeds.x*(float)drag, speeds.y*(float)drag };

        // 3. Limita velocidade de rotação
        if(angularvelocity > max_angular_velocity) angularvelocity = max_angular_velocity;
        if(angularvelocity < -max_angular_velocity) angularvelocity = -max_angular_velocity;
        
        // NOTA: Não chamamos scan_inputs() aqui! 
        // O movimento será aplicado externamente pela classe Bot (Cérebro).
    }

    // --- FUNÇÕES DE SENSORIAMENTO ---

    void set_melhor_nave(){
        eh_o_melhor = true;
    }

    void reset_sensors() {
        std::fill(ray_sensors.begin(), ray_sensors.end(), 0.0f);
    }

    void sense_walls() {
        if (!active) return;

        float angles[] = { -90, -45, 0, 45, 90 };
        
        // Loop pelos 5 sensores
        for(int i = 0; i < NUM_RAYS; i++) {
            float ang_rad = facing_angle + angles[i] * DEG2RAD;
            
            // Vetor direção do raio
            float dx = cosf(ang_rad);
            float dy = sinf(ang_rad);

            // Evita divisão por zero
            if (abs(dx) < 0.0001f) dx = 0.0001f;
            if (abs(dy) < 0.0001f) dy = 0.0001f;

            float dist_wall = ray_max_dist * 2.0f; // Começa com valor alto (longe)

            // --- Matemática de Intersecção Ray-Box (AABB) ---
            
            // 1. Checa paredes Verticais (X)
            if (dx > 0) {
                // Raio indo para direita -> Distância até screenWidth
                float d = (screenWidth - position.x) / dx;
                if (d < dist_wall) dist_wall = d;
            } else {
                // Raio indo para esquerda -> Distância até 0
                float d = (0 - position.x) / dx;
                if (d < dist_wall) dist_wall = d;
            }

            // 2. Checa paredes Horizontais (Y)
            if (dy > 0) {
                // Raio indo para baixo -> Distância até screenHeight
                float d = (screenHeight - position.y) / dy;
                if (d < dist_wall) dist_wall = d;
            } else {
                // Raio indo para cima -> Distância até 0
                float d = (0 - position.y) / dy;
                if (d < dist_wall) dist_wall = d;
            }

            // --- Processa o sinal do sensor ---
            
            // Se a parede está dentro do alcance da visão
            if (dist_wall < ray_max_dist) {
                // Inverte para input neural (1.0 = muito perto, 0.0 = longe/sem nada)
                float signal = 1.0f - (dist_wall / ray_max_dist);
                
                // Clamping
                if (signal < 0) signal = 0;
                if (signal > 1) signal = 1;

                // O sensor pega o MAIOR perigo (seja parede ou asteroide que já foi calculado)
                if (signal > ray_sensors[i]) {
                    ray_sensors[i] = signal;
                }
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

            if (signal > ray_sensors[sensor_idx]) {
                ray_sensors[sensor_idx] = signal;
            }
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
            Vector2 end = {
                position.x + cosf(ang_rad) * len,
                position.y + sinf(ang_rad) * len
            };
            
            DrawLineV(position, end, c);
            
            if(val > 0) {
                Vector2 hit = {
                    position.x + cosf(ang_rad) * len * (1.0f - val),
                    position.y + sinf(ang_rad) * len * (1.0f - val)
                };
                DrawCircleV(hit, 5, RED);
            }
        }
    }
};

#endif