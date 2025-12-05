#ifndef AG_H
#define AG_H

#include "../Mechanics/bot_ship.cpp"
#include "raylib.h"
#include <cmath>
#include <vector>
#include "../Utils/rng.cpp"

using namespace std;


class Bot{
private:
    BotShip* ship;
    double score;
    vector<double> genome;
    int pos_bot;

public:

    // constructors ----------------------------------------------------------------------------
    Bot(BotShip* new_ship, int genome_size, float x, float y, int window_w, int window_h, Texture2D& ship_texture, int id)
    {
        ship = new_ship;
        ship->facing_angle = GetRandomValue(0,359);
        score = 0;
        genome = random_double_vector(genome_size, 0, 50);
    }

    Bot(BotShip* new_ship, vector<double> new_genome, float x, float y, int window_w, int window_h, Texture2D& ship_texture, int id)
    {
        ship = new_ship;
        ship->facing_angle = GetRandomValue(0,359);
        score = 0;
        genome = new_genome;
    }

    // getters e setters ------------------------------------------------------------------------

    double get_score(){
        return(score);
    }

    vector<double> get_genome(){
        return(genome);
    }

    bool get_alive_status(){
        return ship->active;
    }

    float get_coll_radius(){
        return(ship->get_coll_radius());
    }

    void set_genome(vector<double> new_genome){
        genome.clear();
        genome = new_genome;
        return;
    }

    void set_position(int position){
        pos_bot = position;
    }

    void set_score(int new_score){
        score = new_score;
    }

    // methods ----------------------------------------------------------------------------------

    vector<double> movement_decision(){

        vector<double> sensors = ship->getSensors();
        vector<double> output(4);
        output = {0,0,0,0};
        int gsize = genome.size();
        double x = 0;

        for(int i = 0; i < sensors.size(); i++){
            x += sensors[i] * genome[i % gsize];
        }

        x = tanh(x);

        for(int a = 0; a < 4; a++){
            // Gera uma seed baseada no genoma para aplicar na funcao caotica 
            double g = genome[(a * 7) % gsize];

            // Funcao caotica trigonometrica
            double y = sin(x * g) 
                + 0.5 * cos(x * g * 0.7) 
                + 0.25 * sin((x + g) * g);

            // Normaliza o intervalo de y para -1 ate 1
            y = (tanh(y * 2.0) + 1.0) * 0.5;

            output[a] = y;
            
            // Inicializa o proximo x com um valor bagunçado
            x = y;
        }

        return(output); 
    }

    void Classification(){
        double time_score = ship->active ? 1000.0 : ship->distance_moved() * 0.5;
        score = ship->distance_moved(); 
    }    

    // Função que atualiza quem são os melhores pontuados. 
    // (Deve ser rodada a cada vez que um robo for classificado)
    vector<double> Check_best(double score_n1, double score_n2, int pos_bot){
        vector<double> output = {-1, -1, -1};
        //      Vetor de saída 
        //      [0] = pontuacao numero 1
        //      [1] = pontuacao numero 2
        //      [2] = posicao no vector do novo melhor (se for melhor)
        if(score > score_n1){
            output[0] = score;
            output[2] = pos_bot;
            return(output);
        }
        else if(score > score_n2){
            output[1] = score;
            output[2] = pos_bot;
            return(output);
        }
        else{
            return(output);
        }
    }

    // mut_percent deve ser um valor entre 0 e 1
    vector<double> gen_mutation(vector<double> original, double mut_percent, 
                                    int max_mut_genes, int min_mut_genes){
        int num_genes_mutados = GetRandomValue(min_mut_genes, max_mut_genes);

        vector<double> NOVO_genoma(original.size(), 0);
        vector<bool> control(original.size(), false);

        for(int i = 0; i < num_genes_mutados; i++){
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

            NOVO_genoma[pos_gene] = original[pos_gene] + mutation;
        }

        return(NOVO_genoma);

    }

    // FUNCAO QE TRANSA OS BIXO
    vector<double> sex(Bot& pai1, Bot& pai2, int gen_size){
        vector<double> genoma_pai1 = pai1.get_genome();
        vector<double> genoma_pai2 = pai2.get_genome();

        vector<double> genoma_out;

        // O TRANZAMENTO EH UMA MEDIA ARITMETICA DE CADA PAR DE GENE DOS DOIS PAIS
        for(int i = 0; i < gen_size; i++){
            genoma_out[i] = (genoma_pai1[i] + genoma_pai2[i])/2;
        }

        // HAMA A FUNCAO PRA MUTAR OS GENES
        gen_mutation(genoma_out, 0.1, 1, 0);

        return(genoma_out);
    }

    // =================== FUNCAO MAIS IMPORTANTE DO PLANETA NAO PULE DE JEITO NEMHUM !111!!!1!! =======================

    void Draw(){
        ship->Draw();
    }

    void DrawExtra(){
        ship->DrawExtra();
    }

    void update(){
        ship->update();
        
    }

    void movement(){
        vector<double> inputs = movement_decision();
        ship->movement(inputs);
    }


};

#endif