#include <cmath>
#include <vector>
#include <ctime>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "Mechanics/asteroid.cpp"
#include "Mechanics/ship.cpp"
#include "Mechanics/bot_ship.cpp"
#include "Mechanics/entity.cpp"
#include "world.cpp"
#include "./Genetic Algorithm/evolution.cpp"   
#include "./Genetic Algorithm/bot.cpp"  

// --- CONSTANTES ---
#define nIndv 30
#define nAsteroids 10
#define cell_size 32

#define GENOME_SIZE   28     // Chromosome size

int main() {
    // init ------------------------------------------------------------------------------------

    SetRandomSeed((time(NULL)));
    const int screenWidth = 1800;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Spaceship Drift");
    SetTargetFPS(60);
    
    //------------------------------------------------------------------------------------------

    // GUI -------------------------------------------------------------------------------------

    bool draw_graphics = true;
    bool capped_fps = true;
    bool paused = false;
    
    //------------------------------------------------------------------------------------------
    
    // load textures ---------------------------------------------------------------------------
    
    vector<Texture2D> textures;
    textures.emplace_back(LoadTexture("../assets/millenium.png"));
    textures.emplace_back(LoadTexture("../assets/asteroid.png"));
    textures.emplace_back(LoadTexture("../assets/xwing.png"));

    //------------------------------------------------------------------------------------------

    // create world ----------------------------------------------------------------------------

    World world(25,screenWidth,screenHeight,cell_size, textures);

    //------------------------------------------------------------------------------------------

    // spawn entities --------------------------------------------------------------------------
    
    //world.Spawn_entity(0);  
    for(int i = 0; i < nAsteroids; i++) world.Spawn_entity(1);

    // Bot ships spawn
    Evo evolution = Evo();     
    vector<Bot> population;
    

    BotShip* ship_phys = world.SpawnBotShip();
    population.emplace_back(ship_phys, GENOME_SIZE, 0, 0, 0, 0, textures[2], 0);
    evolution.Load_best(population[0]);

    for(int i = 1; i < nIndv; i++) {
        BotShip* ship_phys = world.SpawnBotShip(); //cria a nave
        population.emplace_back(ship_phys, GENOME_SIZE, 0, 0, 0, 0, textures[2], i); //coloca o cerebro na nave
    }

    //------------------------------------------------------------------------------------------
    
    
    // main loop -------------------------------------------------------------------------------
    int generation = 1;
    float gen_duration = 500;
    int timer = 0;

    evolution.clear_gens();

    while (!WindowShouldClose()) {
        
        timer++;

        int alive = 0;
        for(auto& bot : population) {
            bot.movement();
            if(bot.get_alive_status()) alive++;
        }

        world.update();

        //Repopulation
        if (timer >= gen_duration || alive == 0) {
            std::cout << "--- End of generation " << generation << " ---" << std::endl;

            // Fitness Evaluation
            for(int i = 0; i < population.size(); i++) evolution.classification(i, population);

            // 1. Save
            cout << " best pos " << evolution.get_best_pos();
            if(evolution.get_best_pos() != -1){
                double bestScore = population[evolution.get_best_pos()].get_score();

                evolution.SaveBestGenScore(generation, bestScore);
                evolution.updateK(population.size());

                evolution.Save_gens(generation, evolution.get_best_bot(population));
                evolution.Save_best(evolution.get_best_bot(population));
            }

            // 2. Evolve
            vector<vector<double>> next_genomes = evolution.repopulation(population, nIndv, GENOME_SIZE);

            // 3. Reset World
            world.Reset(false); // true = keep astoroids, erase ships
            for(int i = 0; i < nAsteroids; i++) world.Spawn_entity(1);

            // 4. Recreate Player Ship
            //world.Spawn_entity(0);

            // 5. Recreate new population
            population.clear();
            for(int i = 0; i < nIndv; i++) {
                BotShip* ship_phys = world.SpawnBotShip();
                if(i == 0) ship_phys->is_best = true;
                population.emplace_back(ship_phys, next_genomes[i], 0, 0, 0, 0, textures[2], i);
            }

            timer = 0;
            generation++;
            std::cout << "Beginning generation " << generation << std::endl;
        }


        // Draw --------------------------------------------------------------------------------
        start:
        BeginDrawing();
            ClearBackground(BLACK);

            if(draw_graphics){
                world.Draw();
                world.DrawExtra();
            }
            
            // GUI ----------------------------------------------------------------------------
            DrawFPS(screenWidth - 100, 10);
            DrawText(TextFormat("GENERATION: %i", generation), 10, 50, 20, GREEN);
            DrawText(TextFormat("TIME: %i", timer), 10, 85, 20, GREEN);
            DrawText(TextFormat("Current K: %d", evolution.current_k), 10, screenHeight - 30, 20, GREEN);
            GuiSliderBar((Rectangle){120, 20, 200, 20}, "Generation Duration", TextFormat("%.0f", gen_duration), &gen_duration, 250, 2000);
            if (GuiButton((Rectangle){370, 20, 120, 30 }, "Toggle Draw Graphics")) draw_graphics = !draw_graphics;
            if (GuiButton((Rectangle){500, 20, 120, 30 }, "Toggle FPS cap ")){
                if(capped_fps){
                    SetTargetFPS(999);
                }else{
                    SetTargetFPS(60);
                }
                capped_fps = !capped_fps;
            }
            if (GuiButton((Rectangle){630, 20, 50, 30 }, "Pause")){
                paused = !paused;
            }
            //----------------------------------------------------------------------------------
        EndDrawing();
        if(paused) goto start;
        //--------------------------------------------------------------------------------------
    }
    //------------------------------------------------------------------------------------------

    CloseWindow();

    return 0;
}

