#include <random>
#include <string>
#include <iostream>
#include <raylib.h>

#define M_PI 3.14159265358979323846
using namespace std;


class ship {
private:
    double x, y;
    int width;
    int height; 
    double velocity;
    double angle;
    string imgPath = "../assets/xwing.png";
    Texture2D texture;
public:
    //contrutor da classe
    ship(int screen_width, int screen_height, int ship_width, int ship_height, double min_vel, double max_vel){
        //Gerando números aleatórios
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distrib_x(ship_width, screen_width - ship_width);
        uniform_int_distribution<> distrib_y(ship_height, screen_height - ship_height);
        uniform_real_distribution<> distrib_vel(min_vel, max_vel);
        uniform_real_distribution<> distrib_ang(0, 2*M_PI);

        x = distrib_x(gen);
        y = distrib_y(gen);
        width = ship_width;
        height = ship_height;
        velocity = distrib_vel(gen);
        angle = distrib_ang(gen);
        texture = LoadTexture(imgPath.c_str());
    }
    ~ship() {
        // Verifica se a textura é válida antes de tentar descarregar
        if (texture.id > 0) {
            UnloadTexture(texture);
        }
    }
    void Draw() {
        if (texture.id <= 0) return; // Não desenha se a textura não foi carregada

        Rectangle src = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
        
        // O destino é onde na tela ela vai (com o tamanho definido na classe)
        // O 'x' e 'y' da classe será o canto superior esquerdo
        Rectangle dest = { (float)x, (float)y, (float)width, (float)height };
        
        // A origem da rotação será o centro da nave
        Vector2 origin = { (float)width / 2.0f, (float)height / 2.0f };

        //Converte para graus
        float rotationInDegrees = (float)(angle * (180.0 / M_PI));

        DrawTexturePro(texture, src, dest, origin, rotationInDegrees, WHITE);
    }
};