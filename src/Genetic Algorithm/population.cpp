#ifndef AG_H
#define AG_H

#include "../ship.cpp"
#include "raylib.h"
#include <cmath>
#include <vector>
#include <fstream>
#include <iostream>

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

double random_double_sign(double num_original){
    int sign = GetRandomValue(0, 1);

    if(sign == 0){
        return(num_original);
    }
    else if(sign == 1){
        return(num_original * (-1));
    }

    return(num_original);
}


class Indiv{
private:
    Ship ship;
    double score;
    vector<double> genoma;

public:
    // ===== Construtores =====
    // Construtor vazio, necessário para iniciar a população
    Indiv(Texture2D& ship_texture) : ship(ship_texture) {};

    // Construtor padrão do bot
    Indiv(vector<double> new_genome, int windowWidth, int windowHeight, Texture2D& ship_texture)
    : ship(GetRandomValue(0, windowWidth), GetRandomValue(0, windowHeight), windowWidth, windowHeight, ship_texture)
    {
        ship.facing_angle = GetRandomValue(0,359);
        score = 0;
        genoma = new_genome; // Recebe o genoma passado por argumento
    }

    // Destrutor dos bots (util para death, por enquanto não usado)
    ~Indiv() = default;

    //Função de escolha de movimento
    // Essa função tem um comportamento caótico, o que desejamos para escolher o movimento, pois garante
    // alta variabilidade ao mudar levemente o genoma.
    vector<double> movement_decision(){
        vector<double> sensors = ship.getSensors();
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

    // ===== Getters e Setters =====

    double get_score(){
        return(score);
    }
    vector<double> get_genome(){
        return(genoma);
    }

    // ===== Funções de Evolução =====

    // Função que calcula pontuacao do individuo a cada frame
    // Mudança para o futuro: Colocar novas metricas de classificação
    //                              - Velocidade normalizada
    //                              - Desvios bem sucedidos
    double classification(Indiv bot){
        score = ship.distancemoved;
        return(score);
    }

    // Função que atualiza quem são os melhores pontuados. 
    // (Deve ser rodada a cada vez que um robo for classificado)
    vector<double> podium_check(double pontuacao_bot, double pontuacao_n1, double pontuacao_n2, int pos_bot){
        vector<double> output = {-1, -1, -1};
        //      Vetor de saída 
        //      [0] = pontuacao numero 1
        //      [1] = pontuacao numero 2
        //      [2] = posicao no vector do novo melhor (se for melhor)
        if(pontuacao_bot > pontuacao_n1){
            output[0] = pontuacao_bot;
            output[2] = pos_bot;
            return(output);
        }
        else if(pontuacao_bot > pontuacao_n2){
            output[1] = pontuacao_bot;
            output[2] = pos_bot;
            return(output);
        }
        else{
            return(output);
        }
    }

    // Função que seleciona com torneio de 2
    int tournament_selection(vector<Indiv> &populacao, int pos_n1, int pos_n2){
        if(populacao[pos_n1].get_score() > populacao[pos_n2].get_score()){
            return(pos_n1);
        }
        else{
            return(pos_n2);
        }
    }

    // mut_percent deve ser um valor entre 0 e 1
    vector<double> gen_mutation(vector<double> original, double mut_percent, 
                                    int max_mut_genes, int min_mut_genes){
        int muted_gen_num = GetRandomValue(min_mut_genes, max_mut_genes);

        vector<double> new_genoma = original;
        vector<bool> control(original.size(), false);

        for(int i = 0; i < muted_gen_num; i++){
            // Busca uma casa do genoma aleatoria para mutar, se a casa ja tiver sido mutada antes,
            // sorteia casas aleatorias até achar uma que não foi modificada
            int pos_gene = GetRandomValue(0, original.size() - 1);
            while(control[pos_gene] == 1){
                pos_gene = GetRandomValue(0, original.size() - 1);
            }
            // marca control na posicao com 1 para nao acessar mais aquela casa
            control[pos_gene] = 1;

            double mutation = random_double_sign(get_random_double(0, original[pos_gene]*mut_percent));

            while(mutation <= -1*original[pos_gene]){
                mutation = random_double_sign(get_random_double(0, original[pos_gene]*mut_percent));
            }

            new_genoma[pos_gene] = original[pos_gene] + mutation;
        }

        return(new_genoma);

    }

    // Função genética
    vector<double> crossover(Indiv& pai1, Indiv& pai2, int gen_size){
        vector<double> genoma_pai1 = pai1.get_genome();
        vector<double> genoma_pai2 = pai2.get_genome();

        vector<double> genoma_out(gen_size);

        //O cruzamento é uma média aritmética
        for(int i = 0; i < gen_size; i++){
            genoma_out[i] = (genoma_pai1[i] + genoma_pai2[i])/2;
        }

        genoma_out = gen_mutation(genoma_out, 0.1, 1, 0);

        return(genoma_out);
    }

    // ===== Funções auxiliares =====

    // IMPORTANTE: Não apagar essa função
    // Função que salva os dados resultantes da execução
    void save_pop(Indiv& best, Indiv secndBest){
        ofstream file("ship_podium.txt");

        if(!file.is_open()){
            return;
        }

        file << "É O MELHOR DE TODOS RECEBA\n";
        file << "1\n";
        file << "PONTUACAO: " << best.score << "\n";
        file << "GENOMA: ";
        for(auto e : best.genoma){
            file << e << " ";
        }

        file << "\n\n";

        file << "É O SEGUNDO MELHOR RECEBA\n";
        file << "2\n";
        file << "PONTUACAO: " << secndBest.score << "\n";
        file << "GENOMA: ";
        for(auto e : secndBest.genoma){
            file << e << " ";
        }

        file << "\n\n";

        return;


    }
    void Draw(){
        ship.Draw();
    }
    void DrawExtra(){
        ship.DrawExtra();
    }
    void update(){
        ship.update();
    }
    void movement(){
        vector<double> inputs = movement_decision();
        ship.movement(inputs);
    }


};

#endif