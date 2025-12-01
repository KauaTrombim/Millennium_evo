#include <cmath>
#include <vector>
#include "raylib.h"
#include "asteroid.cpp"
#include "ship.cpp"
#include "bot_ship.cpp"
#include "entity.cpp"
#include "world.cpp"
#include "shinytriangle.cpp"
//#include "./IA/AG.cpp"

#define nIndv 10
#define nAsteroids 10

int main() {
    // init ------------------------------------------------------------------------------------

    const int screenWidth = 1800;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Drift da Nave");
    SetTargetFPS(60);

    //------------------------------------------------------------------------------------------

    // load textures ---------------------------------------------------------------------------

    vector<Texture2D> textures;
    textures.emplace_back(LoadTexture("../assets/millenium.png"));
    textures.emplace_back(LoadTexture("../assets/asteroid.png"));
    textures.emplace_back(LoadTexture("../assets/xwing.png"));

    //------------------------------------------------------------------------------------------

    // create world ----------------------------------------------------------------------------

    World world(25,screenWidth,screenHeight,textures);

    //------------------------------------------------------------------------------------------

    // spawn entities --------------------------------------------------------------------------

    world.Spawn_entity(0);                                 // jogador
    for(int i=0; i<nAsteroids; i++) world.Spawn_entity(1); // asteroides
    //for(int i=0; i<nIndv; i++) world.Spawn_entity(2);    // bots

    //------------------------------------------------------------------------------------------



    // main loop -------------------------------------------------------------------------------
    while (!WindowShouldClose()) {
        
        // update world
        world.update();

        BeginDrawing();

            ClearBackground(BLACK);

            // draw world entities
            world.Draw();
            world.DrawExtra();

            // show fps
            DrawFPS(GetScreenWidth() - 95, 10);

        EndDrawing();
    }
    //------------------------------------------------------------------------------------------

    CloseWindow();

    return 0;
}