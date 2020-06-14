#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"

using namespace std;

//generator
Random rnd;

//variables

double hbar = 1;
double mass = 1;

int const nsteps = 1e5;
int const nblocks = 50;
   
double x, xnew;
//double L = 1000;

double mu=0;
double sigma=0;


double H[nsteps];

double H_ave[nblocks];
double prog_sum[nblocks];
double prog_sum2[nblocks];
double prog_err[nblocks];

//functions
double Potential(double);
double Potential_first(double);
double Potential_second(double);

double Psi_trial(double x, double mu, double sigma);
double Psi_square(double x, double mu, double sigma);
double Psi_first(double x, double mu, double sigma);
double Psi_second(double x, double mu, double sigma);
double H_Psi(double x, double mu, double sigma);

void Move();
void Measure(int istep, int iblock);


void Blk_Reset(int iblock);
void Accumulate();
void Average(int iblock);

void Blocking_ave();
void Print_ave();
