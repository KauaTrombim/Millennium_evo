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
    vector<Bot> population;

    bool evoType;

    pair<int,int> best2;

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

    vector<double> crossover(Bot& pai1, Bot& pai2, int gen_size){
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

    int tournament_selection(){
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

    void classification(int pos_bot){
        population[pos_bot].Classification();
    }

    // MODIFIED REPOPULATION FUNCTION
    // Changed return type from vector<Bot> to vector<vector<double>> (only genomes).
    // This decouples evolution logic from physical Bot creation.
    vector<vector<double>> repopulation(vector<Bot>& current_population, int pop_size, int gen_size){
        
        // Update internal population copy so tournament_selection uses the latest data
        this->population = current_population;

        vector<vector<double>> new_genomes;
        new_genomes.reserve(pop_size);

        for(int i = 0; i < pop_size; i++){
            // 1. Selection
            int pos_pai1 = tournament_selection();
            int pos_pai2 = tournament_selection();

            Bot& pai1 = this->population[pos_pai1];
            Bot& pai2 = this->population[pos_pai2];

            // 2. Crossover & Mutation (Creates Child DNA)
            vector<double> child_dna = crossover(pai1, pai2, gen_size);

            // 3. Store only the genome
            new_genomes.push_back(child_dna);
        }

        return new_genomes;
    }

    void Save_best(Bot& best1, Bot& best2){
        fs::create_directories("data");
        ofstream file("data/best_individuals.txt");

        if(!file.is_open()){
            return;
        }

        file << "É O MELHOR DE TODOS RECEBA\n";
        file << "1\n";
        file << "PONTUACAO: " << best1.get_score() << "\n";
        file << "GENOMA: ";
        for(auto e : best1.get_genome()){
            file << e << " ";
        }

        file << "\n\n";

        file << "É O SEGUNDO MELHOR RECEBA\n";
        file << "2\n";
        file << "PONTUACAO: " << best2.get_score() << "\n";
        file << "GENOMA: ";
        for(auto e : best2.get_genome()){
            file << e << " ";
        }

        file << "\n\n";

        return;


    }

    bool Load_best(Bot &best1, Bot &best2) {
        ifstream file("data/best_individuals.txt");

        if(!file.is_open()) {
            return false;
        }

        string line;

        // ---------- PRIMEIRO INDIVÍDUO ----------
        // pula a linha "É O MELHOR DE TODOS RECEBA"
        getline(file, line);

        // linha com "1"
        getline(file, line);

        // linha com "PONTUACAO: X"
        getline(file, line);

        // linha com "GENOMA: ..."
        getline(file, line);
        {
            stringstream ss(line);
            string lixo;
            ss >> lixo; // "GENOMA:"
            vector<double> new_genome1;
            double gene;
            while (ss >> gene) {
                new_genome1.push_back(gene);
            }
            best1.set_genome(new_genome1);
        }

        // pula linha em branco
        getline(file, line);


        // ---------- SEGUNDO INDIVÍDUO ----------
        // pula a linha "É O SEGUNDO MELHOR RECEBA"
        getline(file, line);

        // linha com "2"
        getline(file, line);

        // linha com "PONTUACAO: X"
        getline(file, line);

        // linha com "GENOMA: ..."
        getline(file, line);
        {
            stringstream ss(line);
            string lixo;
            ss >> lixo; // "GENOMA:"
            vector<double> new_genome2;
            double gene;
            while (ss >> gene) {
                new_genome2.push_back(gene);
            }
            best2.set_genome(new_genome2);
        }

        return true;
    }
};
#endif