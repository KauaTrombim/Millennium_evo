#include <random>

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

};