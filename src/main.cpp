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

    vector<ship> population(nIndv);
    //Cria os indivíduos
    for(int i = 0; i < nIndv; i++){
        population[i] = ship(screenWidth, screenHeight, 85, 95, 1.0, 5.0);
        population[i].Draw();
    }

    // Loop principal
    while (!WindowShouldClose()) {
        
        // --- Lógica (Update) ---
        // (Aqui você atualizaria a posição x, y e o 'angle' da minhaNave)
        // ex: minhaNave.Update();
        
        // --- Desenho (Draw) ---
        BeginDrawing();
        ClearBackground(BLACK); // Fundo preto

        EndDrawing();
    }

    // Aniquilando a nave
    CloseWindow();

    return 0;
}