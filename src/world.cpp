#ifndef WORLD
#define WORLD

#include "raylib.h"
#include "Mechanics/entity.cpp"
#include "Mechanics/asteroid.cpp"
#include "Mechanics/ship.cpp"
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include "Utils/spatial_hash.cpp"

using namespace std;

class World{
    private:
    int Max_entities;             //entity cap
    int Entity_count;             //entity counter
    int World_width;              //width
    int World_height;             //heigth
    const float Cell_size;        //size of unit square for collision

    vector<Entity*> Entities;     //vector with pointers to all entities
    vector<Texture> Textures;     //texture dictionary

    SpatialHashGrid grid;  //collision hash map

    public:
    // constructor -----------------------------------------------------------------------------
    
    World(int max_entities, int width, int height, float cell_size, vector<Texture2D>& textures)
    :Max_entities(max_entities),
    Entity_count(0),
    World_width(width),
    World_height(height),
    Textures(textures),
    Cell_size(cell_size),
    grid(cell_size)
    {

    }
    
    // methods ----------------------------------------------------------------------------------

    //spawn a new entity. types: 0- player ship, 1- asteroid, 2- bot ship
    Entity* Spawn_entity(int type){
        Entity* new_entity;

        switch(type){
            case 0:
                new_entity = new Ship(World_width/2, World_height/2, World_width, World_height, Textures[type], Entity_count++);
            break;

            case 1:
                new_entity = new Asteroid(World_width/2,World_height/2,World_width,World_height, Textures[type], Entity_count++);
            break;

            case 2:
                new_entity = new Ship(World_width/2,World_height/2,World_width,World_height, Textures[type], Entity_count++);
                new_entity->collisionradius = 18;
                new_entity->type = 2;
            break;
        }

        while(check_valid_spawn(new_entity) == false){
            new_entity->randomize_position();
        }

        Entities.push_back(new_entity);
        return new_entity;
    }

    //update,check and respond to collision for all entities
    void update(){
        if(!Entities.empty()){
            grid.computeFOV(Entities); 
        }

        for(auto& e : Entities){
            e->update();
        }

       //check collision between all entites (With Hash map)
       grid.build(Entities);
       grid.computeCollisions(Entities);
    }

    bool check_valid_spawn(Entity* candidate){

        for(size_t i = 0; i < Entities.size(); ++i){
            if (Entities[i]->active == false) continue;
            if (candidate->coll_check(Entities[i])) return false;
        }
        return true;

    }

    void Draw(){
        for(auto e: Entities) e->Draw();
    }

    void DrawExtra(){
        for(auto e: Entities) e->DrawExtra();
    }

    void Reset() {
        grid.clear();
        for (size_t i = 0; i < Entities.size(); ) {
            delete Entities[i];
            Entities.erase(Entities.begin() + i);
        }
    }

};

#endif
