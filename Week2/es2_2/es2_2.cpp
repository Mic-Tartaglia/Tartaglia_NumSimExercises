
#include <iostream>
#include <fstream>
#include <cmath>
#include "random.h"

using namespace std;

double StdDev(double mean, double sq_mean, int n){

	return sqrt (( sq_mean - mean*mean )/n);

}

int main (int argc, char *argv[]){

/***************************************************************///GENERATOR SETUP

   Random rnd;
   int seed[4];
   int p1, p2;
   ifstream Primes("Primes");
   if (Primes.is_open()){
      Primes >> p1 >> p2 ;
   } else cerr << "PROBLEM: Unable to open Primes" << endl;
   Primes.close();

   ifstream input("seed.in");
   string property;
   if (input.is_open()){
      while ( !input.eof() ){
         input >> property;
         if( property == "RANDOMSEED" ){
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            rnd.SetRandom(seed,p1,p2);
         }
      }
      input.close();
   } else cerr << "PROBLEM: Unable to open seed.in" << endl;

/****************************************************************/

	int const steps = 1e2;
	int const M = 1e4;
	int const N = 100;
	
	int L = M/N;
	double ran;
	double r[M];

	double sum=0;
	
	double position[3];
	double distance[M][steps];

	for(int i=0;i<M;i++){
		position[0]=0;
		position[1]=0;
		position[2]=0;

		for(int k=0;k<steps;k++){
	
			ran = 6*rnd.Rannyu();
			if ( ran < 1)
				position[0]+=1;
			if ( ran >= 1 and ran < 2)
				position[0]-=1;	
			if ( ran >= 2 and ran < 3)
				position[1]+=1;
			if ( ran >= 3 and ran < 4)
				position[1]-=1;
			if ( ran >= 4 and ran < 5)
				position[2]+=1;	
			if ( ran >= 5 and ran < 6)
				position[2]-=1;

			distance[i][k] = sqrt( position[0]*position[0] + position[1]*position[1] + position[2]* position[2]);

		}


		cout << "\n" << distance[i][steps-1] << "\n";  //print single distance
	}

	double ave[N][steps], prog_sum[N][steps], prog_sq[N][steps], prog_err[N][steps]; // first index: BLOCK, second index: STEP
	
	for(int s=0;s<steps;s++){
		for(int i=0;i<N;i++){
			ave[i][s]=0;
			prog_sq[i][s]=0;
			prog_sum[i][s]=0;		

			for(int k=0;k<L;k++)
				ave[i][s]+=distance[i*L+k][s];
			
			ave[i][s]/=L;
	
			for(int j=0;j<=i;j++){
				prog_sum[i][s]+= ave[j][s];
				prog_sq[i][s]+= ave[j][s]*ave[j][s];
			}

			prog_sum[i][s]/=(i+1);		
			prog_sq[i][s]/=(i+1);
			prog_err[i][s]= StdDev(prog_sum[i][s], prog_sq[i][s] , i+1);

		}
	}	
	
	ofstream res_out("results.dat");
	ofstream err_out("errors.dat");

	for (int s=0;s<steps;s++){
		res_out << prog_sum[N-1][s] << "\n";
		err_out << prog_err[N-1][s] << "\n";
	}

	res_out.close();
	err_out.close();

	//rnd.SaveSeed();
	return 0;
}

