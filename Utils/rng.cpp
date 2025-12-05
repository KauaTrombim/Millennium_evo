#ifndef RNG_H
#define RNG_H
#include <raylib.h>
#include <vector>
using namespace std;

double get_random_double(double min, double max) {
    // Generate a random integer in a safe range, e.g., 0 to 1,000,000
    int precision = 1000000; // adjust for desired fractional precision
    int randInt = GetRandomValue(0, precision);

    // Scale it to the desired range
    double result = min + (max - min) * ((double)randInt / precision);
    return result;
}

float get_random_float(float min, float max) {
    // Generate a random integer in a safe range, e.g., 0 to 1,000,000
    int precision = 1000000; // adjust for desired fractional precision
    int randInt = GetRandomValue(0, precision);

    // Scale it to the desired range
    float result = min + (max - min) * ((float)randInt / precision);
    return result;
}

vector<double> random_double_vector(int size, double min, double max){
    vector<double> output(size);
    for(int i = 0; i < size; i++){
        output[i] = get_random_double(min, max);
    }
    return output;
}

double random_double_sign(double num_original){
    int sign = GetRandomValue(0, 1);

    if(sign == 0){
        return(num_original);
    }
    else if(sign == 1){
        return(num_original * (-1));
    }

    return(num_original);
}

#endif