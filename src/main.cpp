#include <cmath>
#include <vector>
#include <ctime>
#include <iostream>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "Mechanics/asteroid.cpp"
#include "Mechanics/ship.cpp"
#include "Mechanics/entity.cpp"
#include "world.cpp"
#include "./Genetic Algorithm/evolution.cpp"   
#include "./Genetic Algorithm/bot.cpp"  

// --- CONSTANTS ---
#define nIndv 25
#define nAsteroids 10
#define cell_size 32

// UPDATED: 8 inputs * 4 outputs = 32 weights for the Neural Network
#define GENOME_SIZE   32     

using namespace std;

// Global variable for fitness history
std::vector<float> fitness_history;

// Function to draw the evolution graph
void DrawFitnessGraph(const std::vector<float>& history, int x, int y, int w, int h) {
    // Semi-transparent background
    DrawRectangle(x, y, w, h, Fade(BLACK, 0.6f));
    DrawRectangleLines(x, y, w, h, DARKGREEN);
    
    if (history.empty()) return;

    // Finds the maximum value for scaling
    float max_val = 0;
    for (float v : history) if (v > max_val) max_val = v;
    if (max_val < 10) max_val = 10; 

    // Draws lines connecting the points
    for (size_t i = 0; i < history.size() - 1; i++) {
        float val1 = history[i];
        float val2 = history[i+1];

        // X Coordinates
        int x1 = x + (int)((float)i / (history.size() - 1) * w);
        int x2 = x + (int)((float)(i + 1) / (history.size() - 1) * w);
        
        if(history.size() == 1) { x1 = x; x2 = x + w; }

        // Y Coordinates (Inverted because Y grows downwards on the screen)
        int y1 = y + h - (int)((val1 / max_val) * (h - 20)) - 10;
        int y2 = y + h - (int)((val2 / max_val) * (h - 20)) - 10;

        DrawLine(x1, y1, x2, y2, GREEN);
    }

    DrawText("FITNESS HISTORY", x + 5, y + 5, 10, GREEN);
    DrawText(TextFormat("Max: %.0f", max_val), x + 5, y + 20, 10, LIME);
}

int main() {
    // init ------------------------------------------------------------------------------------

    SetRandomSeed((time(NULL)));
    const int screenWidth = 1800;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Spaceship Drift - Evolution Mode");
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

    World world(nIndv + nAsteroids + 1,screenWidth,screenHeight,cell_size, textures);

    //------------------------------------------------------------------------------------------

    // spawn entities --------------------------------------------------------------------------
    
    world.Spawn_entity(0);  
    for(int i = 0; i < nAsteroids; i++) world.Spawn_entity(1);

    // Bot ships spawn
    vector<Bot> population;

    for(int i = 0; i < nIndv; i++) {
        Ship* ship = dynamic_cast<Ship*>(world.Spawn_entity(2)); //creates the ship
        vector<double> random_genome;
        for(int j = 0; j < GENOME_SIZE; j++){
            // Initialize weights between -5.0 and 5.0 to allow for strong inhibition and excitation
            random_genome.push_back(get_random_double(-5.0, 5.0));
        }
        population.emplace_back(ship, random_genome, 0, 0, screenHeight, screenWidth, textures[2], i); //puts the brain into the ship
    }
    
    
    //------------------------------------------------------------------------------------------
    
    
    // main loop -------------------------------------------------------------------------------
    int generation = 1;
    float gen_duration = 500;
    int timer = 0;
    Evo evolution = Evo();

    evolution.clear_gens();

    while (!WindowShouldClose()) {
        
        timer++;

        int alive = 0;
        for(auto& bot : population) {
            bot.movement();
            if(bot.get_alive_status()) alive++;
        }

        world.update();
        
        // EVOLUTION -------------------------------------------------------------------------------
        if (timer >= gen_duration || alive == 0) {
            std::cout << "--- End of generation " << generation << " ---" << std::endl;

            // Fitness Evaluation
            for(int i = 0; i < population.size(); i++) evolution.classification(i, population);

            // 1. Save & Record Graph Data
            cout << " | best pos = " << evolution.get_best_pos();
            if(evolution.get_best_pos() != -1){
                Bot& best_bot = evolution.get_best_bot(population);
                
                // Adds to graph history
                fitness_history.push_back(best_bot.get_score());

                evolution.Save_gens(generation, best_bot);
                evolution.Save_best(best_bot);
                cout << " SAVED\n";
            } else {
                // If no one scored (everyone died on frame 1), save 0
                fitness_history.push_back(0);
            }

            // 2. Evolve
            vector<vector<double>> next_genomes = evolution.repopulation(population, nIndv, GENOME_SIZE);

            // 3. Reset World
            world.Reset();
            for(int i = 0; i < nAsteroids; i++) world.Spawn_entity(1);

            // 4. Recreate Player Ship
            world.Spawn_entity(0);

            // 5. Recreate new population
            population.clear();
            for(int i = 0; i < nIndv; i++) {
                Ship* ship = dynamic_cast<Ship*>(world.Spawn_entity(2));;
                if(i == 0) ship->eh_o_melhor = true;
                population.emplace_back(ship, next_genomes[i], 0, 0, screenHeight, screenWidth, textures[2], i);
            }

            timer = 0;
            generation++;
            std::cout << "Beginning generation " << generation << std::endl;
        }
        //------------------------------------------------------------------------------------------
        

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
            DrawText("Use arrow keys to control the Millennium Falcon", 10, screenHeight - 30, 20, GRAY);
            
            // DRAW THE GRAPH HERE
            DrawFitnessGraph(fitness_history, 10, 130, 300, 100);

            GuiSliderBar((Rectangle){120, 20, 200, 20 }, "Generation Duration", TextFormat("%.0f", gen_duration), &gen_duration, 250, 2000);
            if (GuiButton((Rectangle){370, 20, 120, 30 }, "Toggle Draw Graphics")) draw_graphics = !draw_graphics;
            if (GuiButton((Rectangle){500, 20, 120, 30 }, "Toggle FPS cap ")){
                if(capped_fps){
                    SetTargetFPS(9999);
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