#include <cmath>
#include <vector>
#include "raylib.h"
#include "asteroid.cpp"
#include "ship.cpp"
#include "bot_ship.cpp"
#include "entities.cpp"
#include "shinytriangle.cpp"
//#include "./IA/AG.cpp"

#define nIndv 10
#define nAsteroids 10

int main() {
    // init
    const int screenWidth = 1800;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Drift da Nave");
    SetTargetFPS(60);

    vector<Entity*> Entities;

    // texturas
    Texture2D player_texture = LoadTexture("../assets/millenium.png");
    Texture2D asteroid_texture = LoadTexture("../assets/asteroid.png");
    Texture2D bot_texture = LoadTexture("../assets/xwing.png");

    // spawnar entidades:
    int entitycounter = 0;

    //asteroides
    for(int i=0; i<nAsteroids; i++){
        Asteroid* candidate = new Asteroid(1.0f*GetRandomValue(0,screenWidth),1.0f*GetRandomValue(0,screenHeight),screenWidth,screenHeight,asteroid_texture, entitycounter++);
        
        Entities.push_back(candidate);
    }

    //nave do jogador
    Ship* player_ship = new Ship(screenWidth/2, screenHeight/2, screenWidth, screenHeight, player_texture, entitycounter++);
    Entities.push_back(player_ship);

    //naves dos bots *to-do*

    // Loop principal
    while (!WindowShouldClose()) {
        
        // atualizar entidades
        for(auto e : Entities){
            e->update();
        }

        BeginDrawing();
            ClearBackground(BLACK); // Fundo preto

            //desenhar entidades
            for(auto e: Entities){
               e->Draw();
               e->DrawExtra();
            }

            // verificar colisões entre entidades (TEMPORÁRIO!)
            for(size_t i = 0; i < Entities.size(); ++i){
                for(size_t j = i + 1; j < Entities.size(); ++j){
                    if (Entities[i]->coll_check(Entities[j])){
                        Entities[i]->coll_response(Entities[j]);
                    }
                }
            }
            //draw misc.info on screen
            DrawFPS(GetScreenWidth() - 95, 10);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}