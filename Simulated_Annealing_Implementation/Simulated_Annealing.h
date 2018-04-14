//Simulated Annealing header file
#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include <iostream>
#include <cmath>
#include <string>
#include <stack>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdlib.h> /* srand, rand */

using namespace std;

//Stores Node Height and Width
class Node{
public:
string name;
double height;
double width;
int nodeIndex;
Node();
};


//Stores Node Name, Area, and Aspect Ratio
class operandInfo{
public:
string name;
double area;
double aspectRatio;
int operandIndex;
operandInfo();
operandInfo(string, double, double);
};

//Simmulated Annealing class
class Anneal
{
private:
//*********Parameters**************************
// 1/chipratio < Final chip ratio < chipratio
// chipratio > 1.0 
double chipratio = 2.0; 

// Number of attempt moves per temperature iteration.
// Use 10 as the default value
// The total execution time of the program grows linearly with respect to nmoves.
int nmoves = 10;

// Ratio for decreasing temperatures.
// Use 0.85 as the default value.
// Larger values of ratios in general produce better results and have much longer execution time.
double ratio = 0.85;

// Inital Temperature.
// If the input t0 is negative (e.g. -1), the program will compute an inital temperature.
// In this case you will need to add a negative sin to make the inital temperature positive
// Otherwise the program will use the input positive t0 as the inital temperature
// Default value is -1.
double T0 = -1;                

// ratio = 0.1 when temperature falls below (lambdatf * t0).
// After ratio is set to 0.1, the program in general will terminate very quickly.
double lambdatf = 0.005;

// Inital probability of accepting uphill moves
double P = 0.99;
double epsilon = 0.001;

//Number of modules (this will depend on our input file)
int n = 20;
int N = nmoves * n;

double cost;
string E0 = " ";                 // inital solution
string NPE = E0;                 // "E" in example
string Best = E0;                // Best
int uphill = 0;                  // totalNumber of Uphil moves
int MT = 0;                      // total number of moves at a given temperature
double T = T0;                  
double Reject = 0;          

//******Private Functions*************************
void Complement(string &, int, int);
void BallotProp(string &, bool &);
void Nomralized(string &, bool &);
void M1(string &);
void M2(string &);
void M3(string &);
double deltaAverage();
void moveSelect(string &);
void returnBest();

public:
//******Public Functions**************************
double calcCost(string);
void moveM1(string &);
void moveM2(string &);
void moveM3(string &);
void setE0(string);
void setT0(double);
void runAlgorithm();
void getBest();

};
#endif