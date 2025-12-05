/*

#include "asteroid.cpp"
#include "shinyTriangle.cpp"
//#include "./Genetic Algorithm/population.cpp"

#define INDIVIDUAL_NUM 20
#define ASTEROIDS_NUM 5
#define GENOME_SIZE 10
#define EVO_TYPE 0          // 0 para gerações, 1 para tempo real

bool compareIndiv(Indiv& a, Indiv& b){
    return a.get_score() > b.get_score();
}
void elitism(vector<Indiv>& old_pop, vector<Indiv>& new_pop, Texture2D texture, int screenW, int screenH){
    if(old_pop.empty()) return; //Verificação de segurança

    Indiv best = old_pop[0];
    //Insere na próxima geração criando um novo indivíduo idêntico
    new_pop.emplace_back(best.get_genome(), screenW, screenH, texture);

    //Por enquanto passamos apenas o melhor de todos para a próxima geração.
    //Uma vez que salvamos o 2° melhor, isso abre possibilidade dele ir para a próxima gen também.
}

int tournament_selection(vector<Indiv> &populacao){
    int pos_n1 = GetRandomValue(0, populacao.size() - 1);
    int pos_n2 = GetRandomValue(0, populacao.size() - 1);
    while(pos_n2 == pos_n1){
        pos_n2 = GetRandomValue(0, populacao.size() - 1);
    }
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

vector<Indiv> repopulation(vector<Indiv>& population, int pop_size, int gen_size, int windowWidth, int windowHeight, Texture2D& ship_texture){
    vector<Indiv> new_population;
    new_population.reserve(pop_size);

    for(int i = 0; i < pop_size; i++){
        int pos_pai1 = tournament_selection(population);
        int pos_pai2 = tournament_selection(population);

        Indiv& pai1 = population[pos_pai1];
        Indiv& pai2 = population[pos_pai2];

        Indiv filho = Indiv(crossover(pai1, pai2, gen_size), windowWidth, windowHeight, ship_texture);

        new_population.push_back(move(filho));
    }

    return(new_population);
}*/

#include <cmath>
#include <vector>
#include "raylib.h"
#include "Mechanics/asteroid.cpp"
#include "Mechanics/ship.cpp"
#include "Mechanics/bot_ship.cpp"
#include "Mechanics/entity.cpp"
#include "world.cpp"
#include "./Genetic Algorithm/evolution.cpp"   
#include "./Genetic Algorithm/bot.cpp"      

// --- CONSTANTES ---
#define nIndv 10
#define nAsteroids 10
#define cell_size 32

#define GENOME_SIZE   28     // Chromossome size
#define GEN_TIME      1200   // Generation time in frames(at 60 FPS, 1200 = 20 seconds)

int main() {
    // init ------------------------------------------------------------------------------------

    const int screenWidth = 1800;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "Drift da Nave");
    SetTargetFPS(60);
    bool show_debug = true; //Auxiliar para ativar/desativar o desenho do campo de visão das naves
    
    //------------------------------------------------------------------------------------------
    
    // load textures ---------------------------------------------------------------------------
    
    vector<Texture2D> textures;
    textures.emplace_back(LoadTexture("../assets/millenium.png"));
    textures.emplace_back(LoadTexture("../assets/asteroid.png"));
    textures.emplace_back(LoadTexture("../assets/xwing.png"));

    //------------------------------------------------------------------------------------------

    // create world ----------------------------------------------------------------------------

    World world(25,screenWidth,screenHeight,cell_size, textures);

    //------------------------------------------------------------------------------------------

    // spawn entities --------------------------------------------------------------------------
    
    world.Spawn_entity(0);  
    for(int i = 0; i < nAsteroids; i++) world.Spawn_entity(1);

    // Bot ships spwan
    Evo evolution;          
    vector<Bot> population;
    
    for(int i = 0; i < nIndv; i++) {
        BotShip* ship_phys = world.SpawnBotShip(); //cria a nave
        population.emplace_back(ship_phys, GENOME_SIZE, 0, 0, 0, 0, textures[2], i); //coloca o cerebro na nave
    }

    //------------------------------------------------------------------------------------------
    
    
    // main loop --------------------------------------------------------------------------------
    int generation = 1;
    int timer = 0;

    while (!WindowShouldClose()) {
        
        timer++;

        for(auto& bot : population) {
            bot.movement();
        }

        world.update();

        //Repopulation
        if (timer >= GEN_TIME) {
            std::cout << "--- Fim da Geracao " << generation << " ---" << std::endl;

            // Fitness Evaluation
            for(auto& bot : population) bot.Classification();

            // 2. Evolve
            vector<vector<double>> next_genomes = evolution.repopulation(population, nIndv, GENOME_SIZE);

            // 3. Reset World
            world.Reset(true); // true = keep astoroids, erase ships

            // 4. Recreate Player Ship
            world.Spawn_entity(0);

            // 5. Recreate new population
            population.clear();
            for(int i = 0; i < nIndv; i++) {
                BotShip* ship_phys = world.SpawnBotShip();
                population.emplace_back(ship_phys, next_genomes[i], 0, 0, 0, 0, textures[2], i);
            }

            timer = 0;
            generation++;
            std::cout << "Iniciando Geracao " << generation << std::endl;
        }


        // Draw --------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(BLACK);

            world.Draw();
            world.DrawExtra();

            DrawFPS(screenWidth - 100, 10);
            DrawText(TextFormat("GERACAO: %i", generation), 10, 10, 20, GREEN);
            DrawText(TextFormat("TEMPO: %i", timer), 10, 35, 20, GREEN);
            DrawText("Setas para controlar a Millennium Falcon", 10, screenHeight - 30, 20, GRAY);

        EndDrawing();
        //------------------------------------------------------------------------------------------
    }
    //------------------------------------------------------------------------------------------

    CloseWindow();

    return 0;
}

/*
    //Inicia a janela
    startSys("Em busca da Millennium Falcom automática");
    bool show_debug = true; //Auxiliar para ativar/desativar o desenho do campo de visão das naves

    //Iniciando nave de testes
    Texture2D player_texture = LoadTexture("../assets/millenium.png");
    Ship player_ship(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,SCREEN_WIDTH,SCREEN_HEIGHT,player_texture);

    //Iniciando população de naves
    Texture2D ship_texture = LoadTexture("../assets/xwing.png");
    vector<Indiv> population;
    for(int i = 0; i < INDIVIDUAL_NUM; i++){
        population.emplace_back(random_double_vector(GENOME_SIZE, 0, 100), SCREEN_WIDTH, SCREEN_HEIGHT, ship_texture);
    }

    //Iniciando população de asteroides
    Texture2D asteroid_texture = LoadTexture("../assets/asteroid.png");
    std::vector<Asteroid> asteroids;
    for(int i=0; i<ASTEROIDS_NUM; i++){
        asteroids.emplace_back(1.0f*GetRandomValue(0,SCREEN_WIDTH), 1.0f*GetRandomValue(0,SCREEN_HEIGHT), GetRandomValue(50,150), SCREEN_WIDTH, SCREEN_HEIGHT, asteroid_texture);
    }


    //--Loop principal------
    float time_count = 0.0f;
    int generation_count = 1;
    while (!WindowShouldClose()) {
        
        time_count += GetFrameTime(); //Recebe o tempo que passou desde o ultimo frame
        if (IsKeyPressed(KEY_H)) show_debug = !show_debug; //Ativa/desativa o desenho do campo de visão
        
        //Lógica de evolução

        // É interessante calcular o Fitness a cada tick porque podemos ter parametros atualizados em tempo real
        for(auto& ind: population){
            ind.classification();
        }
        // Se for por geração, atualiza a cada contagem de tempo
        if(simulation_mode == 0){
            if(time_count >= GENERATION_DURATION){
                
                sort(population.begin(), population.end(), compareIndiv);
                population[0].save_pop(population[0], population[1]);

                cout << "--Fim da geracao: " << generation_count << "--" << endl;
                cout << "->Melhor pontuacao da geracao " << generation_count << ": " << population[0].get_score() << "<-" << endl;

                //Criação da nova geração
                cout << "NOVA POPULACAO \n";
                vector<Indiv> new_population = repopulation(population,INDIVIDUAL_NUM, GENOME_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT, ship_texture);
                population = new_population; //Atualiza a população
                cout << "CONSEGUIU \n";
                time_count = 0;
                generation_count++;
            }
        }
        // Se for em tempo real, faz filhos novos quando alguem morre
        /*else if(simulation_mode == 1){
            if()

        }
        //Atualização da física
        player_ship.movement(player_ship.scan_inputs());
        player_ship.update();
        for(auto& e : population){
            e.movement();
            e.update();
        }
        for(auto& a : asteroids){
           a.update();
        }

        BeginDrawing();
            ClearBackground(BLACK);

            for(auto& e: asteroids){
                e.Draw();
                e.DrawExtra();
            }

            for(auto& e : population){
                e.Draw();
                if(show_debug)e.DrawExtra();
            }

            player_ship.Draw();
            player_ship.DrawExtra();

            DrawFPS(GetScreenWidth() - 95, 10);
            DrawText(TextFormat("Geracao: %i", generation_count), 10, 10, 20, WHITE);
            DrawText(TextFormat("Tempo: %.1f", time_count), 10, 30, 20, WHITE);

        EndDrawing();
    }

    exitSys();

    return 0;
}
*/