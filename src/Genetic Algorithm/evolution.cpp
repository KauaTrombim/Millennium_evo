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

    vector<double> last_best_scores;
    int current_k;
    int best_pos;

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

    double variance(){
        double mean = 0;
        for(double x : last_best_scores){
            mean += x;
        }
        mean /= last_best_scores.size();
        double variance = 0;
        for(double x : last_best_scores){
            variance += (x - mean)*(x - mean);
        }
        variance /= last_best_scores.size();

        return variance;

        }

    vector<double> crossover(Bot& pai1, Bot& pai2, int gen_size){
        vector<double> genoma_pai1 = pai1.get_genome();
        vector<double> genoma_pai2 = pai2.get_genome();

        vector<double> genoma_out(gen_size);

        //O cruzamento é uma média aritmética
        for(int i = 0; i < gen_size; i++){
            genoma_out[i] = (genoma_pai1[i] + genoma_pai2[i])/2;
        }

        genoma_out = gen_mutation(genoma_out, 0.25, gen_size, 1);

        return(genoma_out);
    }

    int k_tournament_selection(vector<Bot> &population, int k){
        int best = GetRandomValue(0, population.size() - 1);

        for(int i = 1; i < k; i++){
            int challenger = GetRandomValue(0, population.size() - 1);
            if(population[challenger].get_score() > population[best].get_score()) {
                best = challenger;
            }
        }
        return best;
    }


public:

    // constructor -----------------------------------------------------------------------------
    Evo(){
        last_best_scores = {0,0,0,0,0};
        current_k = 2;
        best_pos = -1;
    }

    // getters e setters -----------------------------------------------------------------------

    int get_best_pos(){
        return(best_pos);
    }

    Bot& get_best_bot(vector<Bot> &population){
        return(population[best_pos]);
    }

    // methods ---------------------------------------------------------------------------------

    void classification(int pos_bot, vector<Bot> &population){
        cout << "size = " << population.size() 
     << " | pos_bot = " << pos_bot << "\n";
        population[pos_bot].Classification();
        cout << "classificou bot " << pos_bot;
        if(best_pos != -1){
            if(population[pos_bot].get_score() > population[best_pos].get_score()){
                best_pos = pos_bot;
            }
        }
        else{
            best_pos = pos_bot;
        }
        return;
    }

    int updateK(int pop_size){
        double VAR_THRESHOLD = 1e-3;
        double var = variance();

        if(var < VAR_THRESHOLD) {
            // stagnated
            current_k = max(2, current_k - 1);
        } else {
            // evolving
            current_k = min(pop_size, current_k + 1);
        }
        return current_k;
    }

    void set_best_pos(int pos){
        best_pos = pos;
    }

    // MODIFIED REPOPULATION FUNCTION
    // Changed return type from vector<Bot> to vector<vector<double>> (only genomes).
    // This decouples evolution logic from physical Bot creation.
    vector<vector<double>> repopulation(vector<Bot>& population, int pop_size, int gen_size){

        vector<vector<double>> new_genomes;
        new_genomes.reserve(pop_size);

        vector<double> melhor_de_todos = get_best_bot(population).get_genome();
        new_genomes.push_back(melhor_de_todos);

        for(int i = 0; i < pop_size-1; i++){
            // 1. Selection
            int pos_pai1 = k_tournament_selection(population, current_k);
            int pos_pai2 = k_tournament_selection(population, current_k);

            Bot& pai1 = population[pos_pai1];
            Bot& pai2 = population[pos_pai2];

            // 2. Crossover & Mutation (Creates Child DNA)
            vector<double> child_dna = crossover(pai1, pai2, gen_size);
// methods ----------------------------------------------------------------------------------
            // 3. Store only the genome
            new_genomes.push_back(child_dna);
        }

        return new_genomes;
    }

    void Save_best(Bot& best1){
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
// methods ----------------------------------------------------------------------------------
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

    void clear_gens(){
        ofstream file("gens.txt", std::ios::trunc);
        file.close();
    }

    void Save_gens(int gen, Bot& best){
    ofstream file("gens.txt", ios::app);

    if (!file.is_open()) {
        cerr << "Erro ao abrir gens.txt\n";
        return;
    }

    file << gen << " " << best.get_score() << "\n";
    }
};
#endif