#ifndef SETUP_H
#define SETUP_H

// --- Bibliotecas Comuns ---
// Ao incluir o setup.h, todos esses arquivos vêm junto
#include "raylib.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <fstream>

using namespace std;

// Constantes
const int GENERATION_DURATION = 20.0f;
const int SCREEN_WIDTH = 1800;
const int SCREEN_HEIGHT = 1000;
const int TARGET_FPS = 60;

// Funções de Setup
void startSys(const char* title);
void exitSys();

#endif