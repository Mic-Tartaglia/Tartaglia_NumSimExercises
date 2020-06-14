#include "random.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

double StdDev(double mean, double sq_mean, int n);

int main(){

	//**********************************************************************//
	//SETTING UP GENERATOR

	Random rnd;

	int seed[4];
	int p1, p2;

	//import seed and primes

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
 	         	rnd.SetRandom(seed,255,1561);
  	       }
  	    }
  	    input.close();
   	} else cerr << "PROBLEM: Unable to open seed.in" << endl;

	//**********************************************************************//	
	//FILLING ARRAY OF RANDOM NUMBERS IN [0,1]

	int M = 1e4; //number generated each cycle
	int N = 100; // # of bins
	int R = M/N; // expected numbers of elements in a bin
	double L = 1./N; // size of a bin

	double r[M];
	int counter[N];
	double chi_sq[N];

	ofstream chi_out("chi_squared.dat");
	for (int k=0;k<N;k++){
		chi_sq[k]=0;
		for (int i=0;i<M;i++){
			r[i]= rnd.Rannyu(); // fill up r with random numbers
		}
		for (int i=0;i<N;i++){
			counter[i]=0;
			for(int j=0;j<M;j++){
				if ( (r[j] > i*L) and (r[j] < (i+1)*L) )
					counter[i]+=1;
			}
			//cout << counter[i] << " ";             // check it is around R
			chi_sq[k]+= pow((counter[i] - R),2) / (double)R;
		}
	chi_out << chi_sq[k] << "\n";
	}	
	chi_out.close();

return 0;
}

double StdDev(double mean, double sq_mean, int n){

	return sqrt (( sq_mean - mean*mean )/n);

}



