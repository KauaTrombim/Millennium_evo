#include <cmath>
#include <vector>
#include <string>
#include "raylib.h"

#include "Mechanics/asteroid.cpp"
#include "Mechanics/ship.cpp"
#include "Mechanics/bot_ship.cpp"
#include "Mechanics/entity.cpp"

#include "world.cpp"

#include "Genetic Algorithm/evolution.cpp"

#define POPULATION_SIZE 20
#define GENOME_SIZE 10
#define NUM_ASTEROIDS 10
#define GENERATION_TIME 15.0f // Seconds per Generation

int main() {
    // init ------------------------------------------------------------------------------------

    const int screenWidth = 1800;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Drift da Nave");
    SetTargetFPS(60);
    bool show_debug = true; //Auxiliar para ativar/desativar o desenho do campo de visão das naves

    //------------------------------------------------------------------------------------------

    // load textures ---------------------------------------------------------------------------

    vector<Texture2D> textures;
    textures.emplace_back(LoadTexture("../assets/millenium.png"));
    textures.emplace_back(LoadTexture("../assets/asteroid.png"));
    textures.emplace_back(LoadTexture("../assets/xwing.png"));

    //------------------------------------------------------------------------------------------

    // create world ----------------------------------------------------------------------------

    //Anteriormente 25 e cell_size
    World world(100,screenWidth,screenHeight,35, textures);

    //------------------------------------------------------------------------------------------

    Evo evo;


    int entity_id_counter = 1000; // IDs para bots
    vector<Bot> population = evo.Initial_Population(POPULATION_SIZE, GENOME_SIZE, screenWidth, screenHeight, textures[2], entity_id_counter, world);

    // spawn entities --------------------------------------------------------------------------

    world.Spawn_entity(0);                                 // jogador
    for(int i=0; i<NUM_ASTEROIDS; i++) world.Spawn_entity(1); // asteroides
    //for(int i=0; i<nIndv; i++) world.Spawn_entity(2);    // bots

    //------------------------------------------------------------------------------------------
    //Controll settings
    float timer = 0.0f;

    // main loop -------------------------------------------------------------------------------
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        timer += dt;
        if (IsKeyPressed(KEY_H)) show_debug = !show_debug;
        
        // update world
        world.update();

        //Thinking
        for(auto& bot : population){
            bot.update(); 
        }

        //Generation End Detection
        if(timer >= GENERATION_TIME){
            //Fitnnes calculation
            population = evo.repopulation(population, POPULATION_SIZE, GENOME_SIZE, screenWidth, screenHeight, textures[2], entity_id_counter, world);
            timer = 0.0f;
        }

        BeginDrawing();

            ClearBackground(BLACK);

            // draw world entities
            world.Draw();
            if(show_debug) world.DrawExtra();

            // show fps
            DrawFPS(GetScreenWidth() - 95, 10);
            DrawText(TextFormat("Tempo: %.2f / %.2f", timer, GENERATION_TIME), 10, 10, 20, WHITE);
            DrawText(TextFormat("Bots Vivos: %lu", population.size()), 10, 50, 20, GRAY);
            DrawText(TextFormat("Pressione 'H' para desativar o debug"), 10, 30, 20, WHITE);

        EndDrawing();
    }
    //------------------------------------------------------------------------------------------

    CloseWindow();

    return 0;
}
