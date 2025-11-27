#include "setup.h"

void startSys(const char* title) {
    // Inicia a janela usando as constantes do .h
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, title);
    SetTargetFPS(TARGET_FPS);
    
    // Configurações extras (se quiser deixar tela cheia ou logar algo)
    // ToggleFullscreen(); 
    cout << "Sistema iniciado: " << title << endl;
}

void exitSys() {
    CloseWindow();
    cout << "Sistema encerrado." << endl;
}