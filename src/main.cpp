#include <cmath>
#include <vector>
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

#define GENOME_SIZE   28     // Chromossome size

int main() {
    // init ------------------------------------------------------------------------------------

    const int screenWidth = 1800;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Drift da Nave");
    SetTargetFPS(60);
    bool show_debug = true; //Auxiliar para ativar/desativar o desenho do campo de visão das naves
    
    //------------------------------------------------------------------------------------------

    // GUI --------------------------------------------------------------------------------------

    bool draw_graphics = true;
    bool capped_fps = true;
    
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
    float gen_duration = 500;
    int timer = 0;

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

            if(draw_graphics){
                world.Draw();
                world.DrawExtra();
            }
            
            // GUI ----------------------------------------------------------------------------
            DrawFPS(screenWidth - 100, 10);
            DrawText(TextFormat("GERACAO: %i", generation), 10, 50, 20, GREEN);
            DrawText(TextFormat("TEMPO: %i", timer), 10, 85, 20, GREEN);
            DrawText("Setas para controlar a Millennium Falcon", 10, screenHeight - 30, 20, GRAY);
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
            //----------------------------------------------------------------------------------

        EndDrawing();
        //--------------------------------------------------------------------------------------
    }
    //------------------------------------------------------------------------------------------

    CloseWindow();

    return 0;
}

