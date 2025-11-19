#include <cmath>
#include <vector>
#include "raylib.h"
#include "asteroid.cpp"
#include "shinytriangle.cpp"
#include "./IA/AG.cpp"

#define nIndv 20
#define nAsteroids 5

int main() {
    // init
    const int screenWidth = 1800;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Drift da Nave");
    SetTargetFPS(60);

    // nave do jogador
    Texture2D player_texture = LoadTexture("../assets/millenium.png");
    
    Ship player_ship(screenWidth/2,screenHeight/2,screenWidth,screenHeight,player_texture);

    // coisas das naves
    Texture2D ship_texture = LoadTexture("../assets/xwing.png");

    vector<BotNave> population;
    for(int i = 0; i < nIndv; i++){
        population.emplace_back(1, GetRandomValue(0,screenWidth), GetRandomValue(0,screenHeight), screenWidth, screenHeight, ship_texture);
    }

    // coisas dos asteroides
    Texture2D asteroid_texture = LoadTexture("../assets/asteroid.png");

    std::vector<Asteroid> asteroids;
    for(int i=0; i<nAsteroids; i++){
        asteroids.emplace_back(1.0f*GetRandomValue(0,screenWidth), 1.0f*GetRandomValue(0,screenHeight), GetRandomValue(50,150), screenWidth, screenHeight, asteroid_texture);
    }

    // Loop principal
    while (!WindowShouldClose()) {
        
        // atualizar naves
        player_ship.movement(player_ship.scan_inputs());
        player_ship.update();
        for(auto& e : population){
            e.movement();
            e.update();
        }

        //atualizar asteroides
        for(auto& a : asteroids){
           a.update();
        }

        BeginDrawing();
            ClearBackground(BLACK); // Fundo preto

            //desenhar asteroides
            for(auto& e: asteroids){
                e.Draw();
                e.DrawExtra();
            }

            //desenhar naves
            for(auto& e : population){
                e.Draw();
                e.DrawExtra();
            }

            player_ship.Draw();
            player_ship.DrawExtra();

            //draw info on screen
            DrawFPS(GetScreenWidth() - 95, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}