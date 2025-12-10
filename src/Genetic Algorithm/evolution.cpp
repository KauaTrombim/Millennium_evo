#ifndef EVO_H
#define EVO_H

#include <vector>
#include "bot.cpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = filesystem;

class Evo{
private:
    int best_pos;

    vector<double> gen_mutation(vector<double> original, double mut_percent, 
                                int max_mut_genes, int min_mut_genes){
        int muted_gen_num = GetRandomValue(min_mut_genes, max_mut_genes);

        vector<double> new_genoma = original;
        vector<bool> control(original.size(), false);

        for(int i = 0; i < muted_gen_num; i++){
            int pos_gene = GetRandomValue(0, original.size() - 1);
            while(control[pos_gene] == 1){
                pos_gene = GetRandomValue(0, original.size() - 1);
            }
            control[pos_gene] = 1;

            // CHANGE: Ensures minimal perturbation (0.1) even if the original gene is zero.
            // This prevents genes from getting stuck at 0.0.
            double amplitude = abs(original[pos_gene]) * mut_percent + 0.2; 
            double mutation = random_double_sign(get_random_double(0, amplitude));

            new_genoma[pos_gene] = original[pos_gene] + mutation;
        }

        return(new_genoma);
    }

    vector<double> crossover(Bot& pai1, Bot& pai2, int gen_size){
        vector<double> genoma_pai1 = pai1.get_genome();
        vector<double> genoma_pai2 = pai2.get_genome();
        vector<double> genoma_out(gen_size);

        // Uniform Crossover (50/50 chance per gene)
        for(int i = 0; i < gen_size; i++){
            if(GetRandomValue(0, 100) < 50) genoma_out[i] = genoma_pai1[i];
            else genoma_out[i] = genoma_pai2[i];
        }

        // Mutation rate applied to 25% of genes with 30% intensity
        genoma_out = gen_mutation(genoma_out, 0.30, gen_size/4, 1);

        return(genoma_out);
    }

    int tournament_selection(vector<Bot> &population){
        int pos_n1 = GetRandomValue(0, population.size() - 1);
        int pos_n2 = GetRandomValue(0, population.size() - 1);
        
        while(pos_n2 == pos_n1){
            pos_n2 = GetRandomValue(0, population.size() - 1);
        }
        if(population[pos_n1].get_score() > population[pos_n2].get_score()){
            return(pos_n1);
        }
        else{
            return(pos_n2);
        }
    }

public:
    Evo(){ best_pos = -1; }

    int get_best_pos(){ return(best_pos); }
    Bot& get_best_bot(vector<Bot> &population){ return(population[best_pos]); }

    void classification(int pos_bot, vector<Bot> &population){
        population[pos_bot].Classification();
        if(best_pos != -1){
            if(population[pos_bot].get_score() > population[best_pos].get_score()){
                best_pos = pos_bot;
            }
        } else {
            best_pos = pos_bot;
        }
        return;
    }

    void set_best_pos(int pos){ best_pos = pos; }

    vector<vector<double>> repopulation(vector<Bot>& population, int pop_size, int gen_size){
        vector<vector<double>> new_genomes;
        new_genomes.reserve(pop_size);

        // Elitism: Keeps the absolute best
        if (best_pos != -1)
            new_genomes.push_back(get_best_bot(population).get_genome());
        else 
            new_genomes.push_back(population[0].get_genome());

        for(int i = 0; i < pop_size-1; i++){
            int pos_pai1 = tournament_selection(population);
            int pos_pai2 = tournament_selection(population);
            vector<double> child_dna = crossover(population[pos_pai1], population[pos_pai2], gen_size);
            new_genomes.push_back(child_dna);
        }
        return new_genomes;
    }

    void Save_best(Bot& best1){
        fs::create_directories("data");
        ofstream file("data/best_individuals.txt");
        if(!file.is_open()) return;
        file << "MELHOR INDIVIDUO\n1\nPONTUACAO: " << best1.get_score() << "\nGENOMA: ";
        for(auto e : best1.get_genome()) file << e << " ";
        file << "\n\n";
    }

    void clear_gens(){
        ofstream file("gens.txt", std::ios::trunc);
        file.close();
    }

    void Save_gens(int gen, Bot& best){
        ofstream file("gens.txt", ios::app);
        if (!file.is_open()) return;
        file << gen << " " << best.get_score() << "\n";
    }
};
#endif