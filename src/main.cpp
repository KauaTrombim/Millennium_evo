#include <cmath>
#include <vector>
#include "raylib.h"
#include "Mechanics/asteroid.cpp"
#include "Mechanics/ship.cpp"
#include "Mechanics/bot_ship.cpp"
#include "Mechanics/entity.cpp"
#include "world.cpp"
#include "./Genetic Algorithm/evolution.cpp"   
#include "./Genetic Algorithm/bot.cpp"  

// --- CONSTANTES ---
#define nIndv 10
#define nAsteroids 10
#define cell_size 32

#define GENOME_SIZE   28     // Chromossome size
#define GEN_TIME      1000   // Generation time in frames(at 60 FPS, 1200 = 20 seconds)

int main() {
    // init ------------------------------------------------------------------------------------

    const int screenWidth = 1800;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Drift da Nave");
    SetTargetFPS(999);
    bool show_debug = true; //Auxiliar para ativar/desativar o desenho do campo de visão das naves
    
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
    
    world.Spawn_entity(0);  
    for(int i = 0; i < nAsteroids; i++) world.Spawn_entity(1);

    // Bot ships spawn
    Evo evolution;          
    vector<Bot> population;
    
    for(int i = 0; i < nIndv; i++) {
        BotShip* ship_phys = world.SpawnBotShip(); //cria a nave
        population.emplace_back(ship_phys, GENOME_SIZE, 0, 0, 0, 0, textures[2], i); //coloca o cerebro na nave
    }

    //------------------------------------------------------------------------------------------
    
    
    // main loop --------------------------------------------------------------------------------
    int generation = 1;
    int timer = 0;

    while (!WindowShouldClose()) {
        
        timer++;

        //int alive = 0;
        for(auto& bot : population) {
            bot.movement();
            //if(bot.active)
        }

        world.update();

        //Repopulation
        if (timer >= GEN_TIME) {
            std::cout << "--- Fim da Geracao " << generation << " ---" << std::endl;

            // Fitness Evaluation
            for(auto& bot : population) bot.Classification();

            // 2. Evolve
            vector<vector<double>> next_genomes = evolution.repopulation(population, nIndv, GENOME_SIZE);

            // 3. Reset World
            world.Reset(true); // true = keep astoroids, erase ships

            // 4. Recreate Player Ship
            world.Spawn_entity(0);

            // 5. Recreate new population
            population.clear();
            for(int i = 0; i < nIndv; i++) {
                BotShip* ship_phys = world.SpawnBotShip();
                population.emplace_back(ship_phys, next_genomes[i], 0, 0, 0, 0, textures[2], i);
            }

            timer = 0;
            generation++;
            std::cout << "Iniciando Geracao " << generation << std::endl;
        }


        // Draw --------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK);

            world.Draw();
            world.DrawExtra();

            DrawFPS(screenWidth - 100, 10);
            DrawText(TextFormat("GERACAO: %i", generation), 10, 10, 20, GREEN);
            DrawText(TextFormat("TEMPO: %i", timer), 10, 35, 20, GREEN);
            DrawText("Setas para controlar a Millennium Falcon", 10, screenHeight - 30, 20, GRAY);

        EndDrawing();
        //--------------------------------------------------------------------------------------
    }
    //------------------------------------------------------------------------------------------

    CloseWindow();

    return 0;
}

