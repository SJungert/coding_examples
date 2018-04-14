#include "Simulated_Annealing.h"


int main(void){
    // Seed for random number generator.
    unsigned int iseed = 3;
    srand(iseed);

    // Input file has 20 entires, therefore n = 20
    // N = 200
    Anneal A;
    A.setE0("213546H7VHVa8V9HcVHgHibdHkVHfeHVlHVjHVH"); //Set the NPE
    A.setT0(-1);                                        //Initalize T0
    A.runAlgorithm();                                   //Run Annealing Algorithm
    A.getBest();                                        //Return result of Best (lowest cost)

    Anneal B;
    B.setE0("12V3V4V5V6V7V8V9VaVbVcVdVeVfVgViVjVkVlV"); //Set the NPE
    B.setT0(-1);                                        //Initalize T0
    B.runAlgorithm();                                   //Run Annealing Algorithm
    B.getBest();                                        //Return result of Best (lowest cost)

    Anneal C;
    C.setE0("12H3H4H5H6H7H8H9HaHbHcHdHeHfHgHiHjHkHlH"); //Set the NPE
    C.setT0(-1);                                        //Initalize T0
    C.runAlgorithm();                                   //Run Annealing Algorithm
    C.getBest();                                        //Return result of Best (lowest cost)

    // The NPE below was taken from Homework 3 Written portion
    // It requires a different input file
    /*
    Anneal D;
    D.setE0("12H34V56VHV");                             //Set the NPE
    D.setT0(-1);                                        //Initalize T0
    D.runAlgorithm();                                   //Run Annealing Algorithm
    D.getBest();                                        //Return result of Best (lowest cost)
    */

    return 0;
}