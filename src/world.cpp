#ifndef WORLD
#define WORLD

#include "raylib.h"
#include "Mechanics/entity.cpp"
#include "Mechanics/asteroid.cpp"
#include "Mechanics/ship.cpp"
#include "Mechanics/bot_ship.cpp"
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
    World_width(width),
    World_height(height),
    Textures(textures),
    Cell_size(cell_size),
    grid(cell_size)
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
        for(auto& e : Entities){
            grid.computeFOV(Entities, World_width, World_height);
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

    // Em world.cpp, dentro de public:

void Reset(bool keep_asteroids) {
    // 1. Limpa o SpatialHash para evitar colisões fantasmas
    grid.clear();

    // 2. Remove as entidades
    for (size_t i = 0; i < Entities.size(); ) {
        // Se for Bot (type 2) ou Player (type 0), deleta
        if (Entities[i]->type == 2 || Entities[i]->type == 0) {
            delete Entities[i]; // Libera a memória RAM
            Entities.erase(Entities.begin() + i); // Remove do vetor
        }
        // Se for asteroide e não queremos manter, deleta
        else if (!keep_asteroids && Entities[i]->type == 1) {
            delete Entities[i];
            Entities.erase(Entities.begin() + i);
        }
        else {
            i++; // Se manteve o asteroide, avança o índice
        }
    }
    
    // Resetar contadores se necessário
    // Entity_count = ... (opcional, se usar IDs únicos globais)
}

// Método auxiliar para permitir que o Manager pegue o ponteiro da nave física
BotShip* SpawnBotShip() 
{
    BotShip* new_bot = new BotShip(World_width/2, World_height/2, World_width, World_height, Textures[2], Entity_count++); 
    
    // Tenta achar uma posição segura
    int attempts = 0;
    while(check_valid_spawn(new_bot) == false && attempts < 100){
        new_bot->randomize_position();
        attempts++;
    }
    Entities.push_back(new_bot);
    return new_bot;
}

};

#endif
