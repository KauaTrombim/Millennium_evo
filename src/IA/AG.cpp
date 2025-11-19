#ifndef AG_H
#define AG_H

#include "../ship.cpp"
#include "raylib.h"
#include <cmath>
#include <vector>

using namespace std;

// Funcões auxiliares

// Função caotica

double get_random_double(double min, double max) {
    // Generate a random integer in a safe range, e.g., 0 to 1,000,000
    int precision = 1000000; // adjust for desired fractional precision
    int randInt = GetRandomValue(0, precision);

    // Scale it to the desired range
    double result = min + (max - min) * ((double)randInt / precision);
    return result;
}

vector<double> random_double_vector(int size, double min, double max){
    vector<double> output(size);
    for(int i = 0; i < size; i++){
        output[i] = get_random_double(min, max);
    }
    return output;
}


class BotNave{
private:
    Ship nave;
    double pontuacao;
    vector<double> genoma;

public:

    BotNave() = default;

    BotNave(int genome_size, float x, float y, int windowWidth, int windowHeight, Texture2D& ship_texture)
    : nave(GetRandomValue(0, windowWidth), GetRandomValue(0, windowHeight), windowWidth, windowHeight, ship_texture)
    {
        nave.facing_angle = GetRandomValue(0,359);
        pontuacao = 0;
        genoma = random_double_vector(genome_size, 0, 50);
    }

    vector<double> movement_decision(){
        vector<double> sensors = nave.getSensors();
        vector<double> output(4);
        output = {0,0,0,0};

        int gsize = genoma.size();

        double x = 0;

        for(int i = 0; i < sensors.size(); i++){
            x += sensors[i] * genoma[i % gsize];
        }

        x = tanh(x);

        for(int a = 0; a < 4; a++){
            // Gera uma seed baseada no genoma para aplicar na funcao caotica 
            double g = genoma[(a * 7) % gsize];

            // Funcao caotica trigonometrica
            double y = sin(x * g) 
                + 0.5 * cos(x * g * 0.7) 
                + 0.25 * sin((x + g) * g);

            // Normaliza o intervalo de y para -1 ate 1
            y = tanh(y * 2.0);

            output[a] = y;
            
            // Inicializa o proximo x com um valor bagunçado
            x = y;
        }

        return(output); 
    }

    double classificacao(BotNave bot){
        return nave.distancemoved;
    }

    void Draw(){
        nave.Draw();
    }

    void DrawExtra(){
        nave.DrawExtra();
    }

    void update(){
        nave.update();
    }

    void movement(){
        vector<double> inputs = movement_decision();
        nave.movement(inputs);
    }


};

#endif