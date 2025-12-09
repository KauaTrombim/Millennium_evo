#ifndef SPATIAL_HASH_GRID_H
#define SPATIAL_HASH_GRID_H

#include <unordered_map>
#include <vector>
#include <cmath>
#include "../Mechanics/entity.cpp"
#include "../Mechanics/ship.cpp"
#include "../Mechanics/bot_ship.cpp"

using namespace std;

class SpatialHashGrid {
public:
    explicit SpatialHashGrid(float cellSize)
        : cellSize(cellSize) {}

    void clear() {
        grid.clear();
    }

    void insert(Entity* e) {
        auto [cx, cy] = getCell(e->position);
        float radius = e->get_coll_radius();
        // Aumentamos um pouco a margem de inserção para garantir que objetos grandes sejam vistos
        float cell_radius = ceil(radius/cellSize);
        for (int ox = -cell_radius; ox <= cell_radius; ++ox) {
            for (int oy = -cell_radius; oy <= cell_radius; ++oy) {
                size_t h = hashCell(cx + ox, cy + oy);
                grid[h].push_back(e);
            }
        }
    }

    void build(vector<Entity*>& entities) {
        clear();
        for (auto e : entities) {
            if (e->active)
                insert(e);
        }
    }

    void computeCollisions(vector<Entity*>& entities) {
        for (auto e : entities) {
            if (!e->active) continue;

            auto [cx, cy] = getCell(e->position);
            float coll_radius = e->get_coll_radius();
            int hash_radius = ceil(coll_radius/cellSize);

            for (int ox = -hash_radius; ox <= hash_radius; ++ox) {
                for (int oy = -hash_radius; oy <= hash_radius; ++oy) {
                    size_t h = hashCell(cx + ox, cy + oy);
                    auto it = grid.find(h);
                    if (it == grid.end()) continue;

                    for (Entity* other : it->second) {
                        if (other == e) continue;
                        if (!other->active) continue;
                        // Otimização: Só checa colisão se ID for maior para não checar A vs B e B vs A
                        if (other->id <= e->id) continue; 

                        if (e->coll_check(other)) {
                            e->coll_response(other);
                        }
                    }
                }
            }
        }
    }

    // --- ATUALIZAÇÃO DA VISÃO ---
    // ... dentro de spatial_hash.cpp
    void computeFOV(vector<Entity*>& entities){
        for (auto e : entities) {
            if(!e->active) continue;
            
            // Só bots processam visão
            if(BotShip* s = dynamic_cast<BotShip*>(e)){
                
                // 1. Limpa sensores antigos
                s->reset_sensors();

                // >>> NOVO: Detecta Paredes antes de checar entidades <<<
                s->sense_walls(); 

                auto [cx,cy] = getCell(s->position);
                // ... resto do código continua igual ...
                
                // O raio de busca no grid deve ser baseado no alcance da visão (300px), não no raio de colisão
                float vision_range = 300.0f; // Tem que bater com o ray_max_dist do bot
                int hash_radius = ceil(vision_range / cellSize);

                for (int ox = -hash_radius; ox <= hash_radius; ++ox) {
                    for (int oy = -hash_radius; oy <= hash_radius; ++oy) {
                        size_t h = hashCell(cx + ox, cy + oy);
                        auto it = grid.find(h);
                        if (it == grid.end()) continue;

                        for (Entity* other : it->second) {
                            if (other == s) continue;
                            if (!other->active) continue;

                            // 2. Passa o objeto para o sensor da nave processar
                            // A nave decide internamente se ele está no ângulo/distância certos
                            s->update_sensor_with_entity(other);
                        }
                    }
                }
            }
        }
    }

private:
    float cellSize;
    unordered_map<size_t, vector<Entity*>> grid;

    inline pair<int,int> getCell(Vector2& p) {
        return {
            int(floor(p.x / cellSize)),
            int(floor(p.y / cellSize))
        };
    }

    inline size_t hashCell(int x, int y) {
        const size_t p1 = 73856093;
        const size_t p2 = 19349663;
        return size_t(x) * p1 ^ size_t(y) * p2;
    }
};

#endif