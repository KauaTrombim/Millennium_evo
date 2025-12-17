1. Project Summary

The project consists of an evolutionary simulation developed in C++ using the Raylib library. The objective is to evolve spaceships capable of navigating through a environment containing asteroids. 
Unlike traditional approaches, this system implements an Adaptive Genetic Algorithm, where selection parameters adjust dynamically based on the population's performance over generations.

2. Population Initialization

The population consists of fixed number of individuals (INDV_NUM), each controlled by a real-valued vector genome of size 32, the genes represent neural network weights and thresholds. The first generation is initialized with random positive values.
