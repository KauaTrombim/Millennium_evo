#ifndef WORLD
#define WORLD

#include "raylib.h"
#include "entity.cpp"
#include "asteroid.cpp"
#include "ship.cpp"
#include "bot_ship.cpp"
#include <cmath>
#include <vector>
#include <memory>
#include <string>

using namespace std;

class World{
    private:
    int Max_entities;             //entity cap
    int Entity_count;             //entity counter
    int World_width;              //width
    int World_height;             //heigth

    vector<Entity*> Entities;     //vector with pointers to all entities
    vector<Texture> Textures;     //texture dictionary

    public:
    // constructor -----------------------------------------------------------------------------
    
    World(int max_entities, int width, int height, vector<Texture2D>& textures)
    :Max_entities(max_entities),
    World_width(width),
    World_height(height),
    Textures(textures)
    {

    }
    
    // methods ----------------------------------------------------------------------------------

    //spawn a new entity. types: 0- player ship, 1- asteroid, 2- bot ship
    void Spawn_entity(int type){
        Entity* new_entity;

        switch(type){
            case 0:
                new_entity = new Ship(World_width/2, World_height/2, World_width, World_height, Textures[type], Entity_count++);
            break;

            case 1:
                new_entity = new Asteroid(World_width/2,World_height/2,World_width,World_height, Textures[type], Entity_count++);
            break;

            //case 2:
                //new_entity = new Bot_ship();
            //break;
        }

        while(check_valid_spawn(new_entity) == false){
            new_entity->randomize_position();
        }

        Entities.push_back(new_entity);
    }

    //update,check and respond to collision for all entities
    void update(){

        for(auto e : Entities){
            e->update();
        }

       //check collision between all entites (TEMPORARY!)
       for(size_t i = 0; i < Entities.size(); ++i){
            for(size_t j = i + 1; j < Entities.size(); ++j){
                    if (Entities[i]->active == false || Entities[j]-> active == false) continue;
                    if (Entities[i]->coll_check(Entities[j])) Entities[i]->coll_response(Entities[j]);
            }
        }

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

};

#endif
