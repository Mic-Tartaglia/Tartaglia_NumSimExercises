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
  	         	input >> seed[2] >> seed[1] >> seed[3] >> seed[0];
 	         	rnd.SetRandom(seed,255,1561);
  	       }
  	    }
  	    input.close();
   	} else cerr << "PROBLEM: Unable to open seed.in" << endl;

	//**********************************************************************//	
	//FILLING ARRAY OF RANDOM NUMBERS IN [0,1]

	int const N_exp = 1e4;
	int const N_throws = 1e5;		
	int const N_blocks = 100;
	int blk_len = N_exp/N_blocks;

	double x, theta;
	double pi_estim[N_exp];

	double const d= 1.2; 
	double const L= 1;

	int counter= 0;

	ofstream pi_out("pi.dat");
	ofstream err_out("errors.dat");

	double ave[N_blocks];
	double sq_ave[N_blocks];
	double prog_est[N_blocks];
	double prog_sq[N_blocks];
	double prog_err[N_blocks];

	for (int k=0;k<N_exp;k++){

		counter=0;

		for(int thr=0;thr<N_throws;thr++){
			//x = rnd.Rannyu(0, d) // USE THIS WITH COMMENTED OPTION BELOW	
			x = rnd.Rannyu( -d/2, d/2);// OR EQUIVALENTLY, THIS WITH THE SECOND ONE BELOW
			theta = rnd.Rannyu( 0, 2*M_PI);
			
			//if   ( ( x + L/2*cos(theta)<0) or (x + L/2*cos(theta) >d)  or ( x - L/2*cos(theta)<0) or (x - L/2*cos(theta) >d) )       //THIS ONE WORKS WITH RANDOMS ABOVE
			if ( (x-L/2*cos(theta))*(x+L/2*cos(theta)) < 0) //
				counter+=1;
	
		}
		pi_estim[k] = (double)N_throws/counter  * 2*L/d;
		//pi_out << pi_estim[k] << "\n";
		//cout << "hits: " << counter << "\thit/total: " << (double)counter/N_throws << "\tshould be: " << 2*L/(M_PI*d) <<"\n"; 
	}

	for (int k=0;k<N_blocks;k++){  //block averaginfg

		ave[k]=0;
		prog_est[k]=0;
		prog_sq[k]=0;
			
		for(int i=0;i<blk_len;i++)
			ave[k]+= pi_estim[k*blk_len + i] /blk_len;

		sq_ave[k]= ave[k]*ave[k];

		for(int j=0;j<=k;j++){
			prog_est[k]+=ave[j];
			prog_sq[k]+=sq_ave[j];
		}

		prog_est[k]/= (double)(k+1);
		prog_sq[k]/= (double)(k+1);

		prog_err[k]= StdDev( prog_est[k], prog_sq[k], k+1);

		pi_out << prog_est[k] << "\n";
		err_out << prog_err[k] << "\n";

	}
return 0;
}

double StdDev(double mean, double sq_mean, int n){

	return sqrt (( sq_mean - mean*mean )/n);

}



