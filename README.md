1. Project Summary

The project consists of an evolutionary simulation developed in C++ using the Raylib library. The objective is to evolve spaceships capable of navigating through a environment containing asteroids. 
Unlike traditional approaches, this system implements an Adaptive Genetic Algorithm, where selection parameters adjust dynamically based on the population's performance over generations.

2. Population Initialization

The population consists of fixed number of individuals (INDV_NUM), each controlled by a real-valued vector genome of size 32, the genes represent neural network weights and thresholds. The first generation is initialized with random positive values.

3. Population Evaluation

The fitness function evaluates each ship's performance frame by frame. The base score is the distance traveled (distancemoved), but there are some modifiers:

Stability Bonus: The ship receives 10 extra points if it accelerates forward without rotating simultaneously, incentivizing rectilinear movement.

Penalties: Penalties are applied based on the amount of rotation (rot_counter) and the number of collisions (coll_count), significantly subtracting from the total score.

Sensors: Evaluation depends on 7 sensory inputs: Speed, Alignment Coefficient, and 5 proximity sensors (Raycasts).

4. Selection and Reproduction

The selection mechanism uses an Adaptive K-Tournament. The tournament size (k) is not fixed; it is dynamically adjusted by the updateK function based on the variance of the scores of the best individuals from the last 5 generations. 
If the variance is low , the value of k decreases to reduce selection pressure and increase diversity. If the variance is high, k increases to focus on exploiting the best individuals. Reproduction uses Arithmetic Crossover, where the child's genes are the simple average of the two selected parents' genes.

5. Mutation

Mutation occurs after crossover and modifies genes by adding a random value proportional to mut_percent. An important technical aspect of this implementation is the sign constraint: the code includes a while loop that ensures the mutation does not flip the gene's sign to negative, forcing the weights to remain predominantly positive.

7. Neural Network Architecture

The bot implements a neural network. It maps 7 inputs to 4 outputs. Unlike traditional networks with Sigmoid or Tanh activation functions, this implementation uses an Active Threshold logic. The genome contains 28 weights for connections and 4 extra genes serving as activation thresholds. 
For each output, the weighted sum of inputs is calculated; if this value exceeds the specific threshold for that output, the output is activated (1), otherwise, it remains deactivated (0).
