#ifndef AG_H
#define AG_H

#include "../ship.cpp"
#include "../rng.cpp"

using namespace std;

// Funcões auxiliares

// Função caotica


class BotNave{
private:
    Ship nave;
    double pontuacao;
    vector<double> genoma;

public:

    BotNave() = default;

    BotNave(int genome_size, int windowWidth, int windowHeight, float min_axis_speed, float max_axis_speed){

        Random rng = Random();

        nave = Ship(windowWidth, windowHeight, min_axis_speed, max_axis_speed);
        pontuacao = 0;
        genoma = rng.random_double_vector(genome_size, 0, 50);
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