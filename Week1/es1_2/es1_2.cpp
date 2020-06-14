#include <iostream>
#include <cmath>
#include <fstream>
#include "random.h"

using namespace std;

double rand_exp(Random * gen, double lambda){

	double r= gen->Rannyu();
	return  -1./lambda * log( 1. - r);
}

double rand_cauchy(Random * gen, double Gamma, double mean){

	double r= gen->Rannyu();
	return  mean + Gamma*tan( M_PI * (r -0.5));
}

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

	double lambda=1;
	double Gamma=1;

	double ran_exp[1000];
	double ran_Cauchy[1000];	

	//ofstream exp_out("exp.dat");
	//ofstream Cau_out("Cauchy.dat");
	
	ofstream res_out("results.dat");

	int N=1e4;
	double sum_exp[N];
	double sum_Cau[N];
	
	int index[4];
	index[0]=1;
	index[1]=2;
	index[2]=10;
	index[3]=100;
	
	for(int j=0;j<4;j++){
		for (int i=0; i<N; i++){		//specifying how many numbers are to be summed
			sum_exp[i]= 0;
	
			for (int k=0; k<index[j]; k++){			//computing sums of random numbers 
				sum_exp[i]+= rand_exp( &rnd, 1) * (1./index[j]);
			}
	
		res_out << sum_exp[i] << " ";	 
		}
	res_out << "\n";
	}	

	for(int j=0;j<4;j++){
		for (int i=0; i<N; i++){		//specifying how many numbers are to be summed
			sum_Cau[i]= 0;
	
			for (int k=0;k< index[j]; k++){			//computing sums of random numbers 
				sum_Cau[i]+= rand_cauchy( &rnd, 1, 0) * (1./index[j]);
			}
	
		res_out << sum_Cau[i] << " ";	 
		}
	res_out << "\n";
	}


return 0;



}

/*                       // generating random sequences (exp and cauchy)

	for (int k=0;k<N;k++){

		r[k]= rnd.Rannyu();
		ran_exp[k]=-1/lambda * log( 1 - r[k]);
		ran_Cauchy[k] = Gamma*tan( M_PI * (r[k] -0.5));
		
		exp_out << ran_exp[k] << "\n";	
		Cau_out << ran_Cauchy[k] << "\n";
	 }
*/

/*			// same, just quicker

	for(int i=0;i<10000;i++){
		exp_out << rand_exp( &rnd, 1) << "  ";
		Cau_out << rand_cauchy( &rnd, 1) << "  ";
	}
*/

