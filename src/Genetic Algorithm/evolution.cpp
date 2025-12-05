#ifndef EVO_H
#define EVO_H

#include <vector>
#include "bot.cpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

class World;

using namespace std;
namespace fs = filesystem;

class Evo{
private:
    //vector<Bot> population; //It causes crash

    //bool evoType; //Future?
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

    //Essa função existe 2x
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

    int tournament_selection(vector<Bot>& current_pop){
        if(current_pop.empty()) return 0;

        int pos_n1 = GetRandomValue(0, current_pop.size() - 1);
        int pos_n2 = GetRandomValue(0, current_pop.size() - 1);
        
        // Tenta garantir pais diferentes se possível
        if(pos_n2 == pos_n1 && current_pop.size() > 1){
            pos_n2 = (pos_n1 + 1) % current_pop.size();
        }

        if(current_pop[pos_n1].get_score() > current_pop[pos_n2].get_score()){
            return(pos_n1);
        }
        else{
            return(pos_n2);
        }
    }


public:

    Evo() {};

    // --- NOVO: Função para criar a primeira população ---
    vector<Bot> Initial_Population(int pop_size, int gen_size, int window_w, int window_h,
         Texture2D& ship_texture, int& Entity_id_counter, World& world){
        vector<Bot> new_population;
        new_population.reserve(pop_size);

        for(int i = 0; i < pop_size; i++){
            // Cria nave
            BotShip* new_ship = new BotShip(window_w/2, window_h/2, window_w, window_h, ship_texture, Entity_id_counter++);
            // Cria bot com a nave
            Bot new_bot(new_ship, gen_size);
            
            // Adiciona a nave no mundo e o bot na lista local
            world.Spawn_entity(new_ship); // Usa o overload que criamos
            new_population.push_back(new_bot);
        }
        return new_population;
    }

    // void classification(int pos_bot){
    //     population[pos_bot].Classification();
    // }

    vector<Bot> repopulation(vector<Bot>& population, int pop_size, int gen_size,
         int window_w, int window_h, Texture2D& ship_texture, int& Entity_count, World& world){
        vector<Bot> new_population;
        new_population.reserve(pop_size);

        world.offWithTheirHeads(); //Depopulate old generation
        for(auto& bot : population){
            delete bot.get_ship(); 
        }//Clear old bot memory

        for(int i = 0; i < pop_size; i++){
            int pos_pai1 = tournament_selection(population);
            int pos_pai2 = tournament_selection(population);

            Bot& pai1 = population[pos_pai1];
            Bot& pai2 = population[pos_pai2];

            //Linhas antigas
            //float ship_radius = pai1.get_coll_radius();
            //float x = get_random_float(2*ship_radius, window_w - 2*ship_radius);
            //float y = get_random_float(2*ship_radius, window_h - 2*ship_radius);
            //Bot son = (crossover(pai1, pai,2, gen_size), x, y, window_w, window_h, ship_texture, Entity_count ++);
            
            vector<double> son_gen = crossover(pai1, pai2, gen_size); //the old line is above
            BotShip* new_ship = new BotShip(window_w/2, window_h/2, window_w, window_h, ship_texture, Entity_count++); //
            Bot son(new_ship, son_gen);

            world.Spawn_entity(new_ship);
            new_population.push_back(move(son));
        }

        return(new_population);
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