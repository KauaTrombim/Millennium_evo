#include "raylib.h"
#include "ship.cpp"
#include <vector>

#define nIndv 20

int main() {
    // Configuração(
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Nave X-Wing");
    SetTargetFPS(60);

    vector<Ship> population(nIndv);
    //Cria os indivíduos
    for(int i = 0; i < nIndv; i++){
        Ship ship = Ship(screenWidth, screenHeight, 1.0, 5.0);
        population[i] = ship;
        population[i].Draw();
    }

    // Loop principal
    while (!WindowShouldClose()) {
        
        // --- Lógica (Update) ---
        // (Aqui você atualizaria a posição x, y e o 'angle' da minhaNave)
        // ex: minhaNave.Update();
        
        // --- Desenho (Draw) ---
        for(auto& e : population){
            e.movement("SPEED_UP");
            e.update();
        }
        BeginDrawing();
        
            ClearBackground(BLACK); // Fundo preto

            for(auto& e : population){
                e.Draw();
                e.DrawExtra();
            }

            //draw info on screen
            DrawFPS(GetScreenWidth() - 95, 10);

        EndDrawing();
    }

    // Aniquilando a nave
    CloseWindow();

    return 0;
}