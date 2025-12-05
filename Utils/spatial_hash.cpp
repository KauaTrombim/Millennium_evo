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
        float cell_radius = ceil(radius/cellSize);
        for (int ox = -cell_radius; ox <= cell_radius; ++ox) {
            for (int oy = -cell_radius; oy <= cell_radius; ++oy) {
                size_t h = hashCell(cx + ox, cy + oy);
                grid[h].push_back(e);
            }
        }
    }

    // make hash from entity list
    void build(vector<Entity*>& entities) {
        clear();
        for (auto e : entities) {
            if (e->active)
                insert(e);
        }
    }

    // execute collisions by checing neighbour cells
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

                        if (other->id <= e->id) continue;

                        if (e->coll_check(other)) {
                            e->coll_response(other);
                        }
                    }
                }
            }
        }
    }

    void computeFOV(vector<Entity*>& entities){
        for (auto e : entities) {
            if(!e->active) continue;
            if(BotShip* s = dynamic_cast<BotShip*>(e)){
                auto [cx,cy] = getCell(s->position);
                s->set_fov_detect(0);

                float fov_radius = s->get_fov_radius();
                int hash_radius = ceil(fov_radius/cellSize);

                for (int ox = -hash_radius; ox <= hash_radius; ++ox) {
                    for (int oy = -hash_radius; oy <= hash_radius; ++oy) {
                        size_t h = hashCell(cx + ox, cy + oy);

                        auto it = grid.find(h);
                        if (it == grid.end()) continue;

                        for (Entity* other : it->second) {
                            if (other == s) continue;
                            if (!other->active) continue;

                            if (s->check_fov(other)) {
                                s->set_fov_detect(1);
                                break;
                            }
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