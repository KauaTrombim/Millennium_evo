#ifndef POP_MANAGER
#define POP_MANAGER

#include <vector>
#include <iostream>
#include "raylib.h"
#include "world.cpp"
#include "./Genetic Algorithm/evolution.cpp"
#include "./Genetic Algorithm/bot.cpp"

class PopulationManager {
private:
    World* world;            
    Evo evolution;           
    vector<Bot> population;    
    int generation;
    int pop_size;
    int genome_size;
    int max_frames;    
    int current_frame;
    Texture2D ship_texture;  

public:
    PopulationManager(World* world_ref, int size, int g_size, int frames_limit, Texture2D tex)
        : world(world_ref), pop_size(size), genome_size(g_size), max_frames(frames_limit), ship_texture(tex) 
    {
        generation = 1;
        current_frame = 0;
    }

    //Gen1 Creation------------------------------------------------------------------------------------------
    void Initialize() {
        std::cout << "Iniciando Geracao 1..." << std::endl;
        population.clear();
        for(int i = 0; i < pop_size; i++) {
            BotShip* ship_phys = world->SpawnBotShip();
            Bot new_bot(ship_phys, genome_size, 0, 0, 0, 0, ship_texture, i);
            population.push_back(new_bot);
        }
    }
    //------------------------------------------------------------------------------------------
    
    //main loop------------------------------------------------------------------------------------------
    void Update() {
        bool someone_alive = false;
        current_frame++;

        for(auto& bot : population) {
            bot.movement(); 
        }

        // Time out or everybody died (TODO: implement everybody died check)
        if (current_frame >= max_frames) {
            NextGeneration();
        }
    }
    //------------------------------------------------------------------------------------------

    // Next Gen Spawn------------------------------------------------------------------------------------------
    void NextGeneration() {
        std::cout << "--- Fim da Geracao " << generation << " ---" << std::endl;
        double best_score_gen = -1.0;
        
        // 1. Fitness Evaluation
        for(auto& bot : population) {
            bot.Classification(); 
            
            if(bot.get_score() > best_score_gen){
                best_score_gen = bot.get_score();
            }
        }
        std::cout << "Melhor Fitness da Geracao: " << best_score_gen << std::endl;

        // 2. Get new genomes
        vector<vector<double>> next_genomes = evolution.repopulation(population, pop_size, genome_size);

        // 3. Save Best of ALL
        if(evolution.get_best_pos() != -1){
            evolution.Save_gens(generation, evolution.get_best_bot());
            evolution.Save_best(evolution.get_best_bot());
        }


        // 4. World Reset
        // Keep the asteroids, remove ships
        world->Reset(true); 

        // 5. New Pop Creation (Bound DNA with new bodies)
        population.clear(); // Clear old list

        for(int i = 0; i < pop_size; i++) {
            // Create new body in a clean world
            BotShip* ship_phys = world->SpawnBotShip();
            
            // Create new brains
            Bot new_bot(ship_phys, next_genomes[i], 0, 0, 0, 0, ship_texture, i);
            
            // C.Save in the list
            population.push_back(new_bot);
        }

        //Reset timers
        current_frame = 0;
        generation++;
        std::cout << "Iniciando Geracao " << generation << std::endl;
    }
    
    // Getters to draw stats------------------------------------------------------------------------------------------
    int getGen() { return generation; }
    int getTime() { return current_frame; }
    int getLiveCount() { 
        // Alive counter
        int count = 0;
        // TODO: Iterate to check active.
        // TODO: add bool isAlive() in bot.cpp.
        return pop_size; 
    }
    //------------------------------------------------------------------------------------------
};

#endif