#include "setup.cpp"

#include "asteroid.cpp"
#include "shinyTriangle.cpp"
#include "./Genetic Algorithm/population.cpp"

#define INDIVIDUAL_NUM 20
#define ASTEROIDS_NUM 5
#define GENOME_SIZE 10

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


int main() {
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
        if(time_count >= GENERATION_DURATION){
            //Fitness de cada nave
            for(auto& ind: population){
                ind.classification(ind);
            }
            
            std::sort(population.begin(), population.end(), compareIndiv);
            population[0].save_pop(population[0], population[1]);

            cout << "--Fim da geracao: " << generation_count << "--" << endl;
            cout << "->Melhor pontuacao da geracao " << generation_count << ": " << population[0].get_score() << "<-" << endl;

            //Criação da nova geração
            vector<Indiv> new_population;
            new_population.reserve(INDIVIDUAL_NUM);
            elitism(population, new_population, ship_texture, SCREEN_WIDTH, SCREEN_HEIGHT); //Conserva o melhor de todos

            //Preenche a população
            while(new_population.size() < INDIVIDUAL_NUM){
                int aux = INDIVIDUAL_NUM-1;
                int d1 = GetRandomValue(0, aux);
                int d2 = GetRandomValue(0, aux);
                int m1 = GetRandomValue(0, aux);
                int m2 = GetRandomValue(0, aux);

                int dad_index = population[0].tournament_selection(population, d1, d2);
                int mom_index = population[0].tournament_selection(population, m1, m2);

                vector<double> son_gen = population[0].crossover(population[dad_index], population[mom_index], GENOME_SIZE);
                new_population.emplace_back(son_gen, SCREEN_WIDTH, SCREEN_HEIGHT, ship_texture);
            }
            
            population = new_population; //Atualiza a população
            time_count = 0;
            generation_count++;
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