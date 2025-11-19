#ifndef RNG_H
#define RNG_H

#include <random>
#include <vector>

using namespace std;

class Random {
private:
        random_device rd;
        mt19937 gen;

public:
    Random() : gen(rd()){}

    int random_int(int min_range, int max_range){
        uniform_int_distribution<> distrib(min_range, max_range);
        return(distrib(gen));
    }

    double random_double(double min_range, double max_range){
        uniform_real_distribution<> distrib(min_range, max_range);
        return(distrib(gen));
    }

    vector<double> random_double_vector(int size, double min_double_range, double max_double_range){
        vector<double> lista_output(size);
        for(int i = 0; i < size; i++){
            lista_output[i] = random_double(min_double_range, max_double_range);
        }
        return(lista_output);
    }

};

#endif