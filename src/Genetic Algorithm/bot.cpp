#ifndef AG_H
#define AG_H

#include "raylib.h"
#include <cmath>
#include <vector>
#include "../Utils/rng.cpp"

using namespace std;


class Bot{
private:
    Ship* ship;
    double score;
    vector<double> genome;
    int pos_bot;

public:

    // constructors ----------------------------------------------------------------------------

    Bot(Ship* body, vector<double> dna, float x, float y, int window_w, int window_h, Texture2D& ship_texture, int id)
    {
        ship = body;
        body->facing_angle = GetRandomValue(0,359);
        genome = dna;
    }

    // getters and setters ------------------------------------------------------------------------

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

    // methods ----------------------------------------------------------------------------------

    // --- NEURAL NETWORK IMPLEMENTATION ---
    vector<double> movement_decision(){

        vector<double> sensors = ship->getSensors(); // 8 inputs
        vector<double> output(4, 0.0);               // 4 outputs

        // if genome does not match network size, return stopped
        if(genome.size() != sensors.size() * 4) return {0,0,0,0};

        int gene_index = 0;

        // Neural Layer: Input * Weight
        for(int o = 0; o < 4; o++){ // For each output neuron
            for(int s = 0; s < sensors.size(); s++){ // For each sensory input
                output[o] += sensors[s] * genome[gene_index];
                gene_index++;
            }
            
            // Activation Function: Tanh (-1 to 1)
            // Allows negative decisions (e.g., do not accelerate, or turn the other way if configured so)
            output[o] = std::tanh(output[o]);
            
            // Normalize to 0 to 1 for the ship control system
            // (tanh + 1) / 2 maps the interval [-1, 1] to [0, 1]
            output[o] = (output[o] + 1.0) / 2.0;
        }

        return(output); 
    }

    void Classification(){
        score = ship->score; 
    }    

    // Function that updates who are the best scorers. 
    vector<double> Check_best(double score_n1, double score_n2, int pos_bot){
        vector<double> output = {-1, -1, -1};
        //      Output vector 
        //      [0] = score number 1
        //      [1] = score number 2
        //      [2] = position in vector of the new best (if better)
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

    // mut_percent must be a value between 0 and 1
    vector<double> gen_mutation(vector<double> original, double mut_percent, 
                                    int max_mut_genes, int min_mut_genes){
        int num_genes_mutados = GetRandomValue(min_mut_genes, max_mut_genes);

        vector<double> NOVO_genoma(original.size(), 0);
        vector<bool> control(original.size(), false);

        for(int i = 0; i < num_genes_mutados; i++){
            // Search for a random genome slot to mutate, if the slot has already been mutated before,
            // draw random slots until finding one that has not been modified
            int pos_gene = GetRandomValue(0, original.size() - 1);
            while(control[pos_gene] == 1){
                pos_gene = GetRandomValue(0, original.size() - 1);
            }
            // mark control at position with 1 to not access that slot again
            control[pos_gene] = 1;

            double mutation = random_double_sign(get_random_double(0, original[pos_gene]*mut_percent));

            // Avoids mutation that completely zeros the gene
            NOVO_genoma[pos_gene] = original[pos_gene] + mutation;
        }
        
        // Copy unmutated genes
        for(int i=0; i<original.size(); i++){
            if(!control[i]) NOVO_genoma[i] = original[i];
        }

        return(NOVO_genoma);

    }
    
    vector<double> sex(Bot& pai1, Bot& pai2, int gen_size){
        vector<double> genoma_pai1 = pai1.get_genome();
        vector<double> genoma_pai2 = pai2.get_genome();

        vector<double> genoma_out(gen_size);

        for(int i = 0; i < gen_size; i++){
            genoma_out[i] = (genoma_pai1[i] + genoma_pai2[i])/2;
        }

        gen_mutation(genoma_out, 0.1, 1, 0);

        return(genoma_out);
    }

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