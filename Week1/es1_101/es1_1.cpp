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

	int M = 1e5; // # of steps
	int N = 100; // # of blocks
	int L = M/N; // block length

	double r[M];
	for (int i=0;i<M;i++){
		r[i]= rnd.Rannyu(); // fill up r with random numbers
	}

	double ave[N];
	double sq_ave[N];
	double sum=0;

	for (int i=0;i<N;i++){
		sum=0;
		for(int j=0;j<M/N;j++)
			sum+= r[ i*(L) + j] /L;
		ave[i] = sum;
		sq_ave[i] = ave[i]*ave[i];

	}
		//cout << "\n" << ave[i] << "\t" << sq_ave[i];      // quick check
	//COMPUTING AND PRINTING MEAN VALUE

	double progr_sum[N];
	double progr_sqsum[N];
	double progr_error[N];

	for (int i=0;i<N;i++){
		progr_sum[i]=0;
		progr_sqsum[i]=0;
		for(int j=0;j<=i;j++){
			progr_sum[i]+=ave[j];
			progr_sqsum[i]+=sq_ave[j];
		}
		progr_sum[i] /= (double)(i+1);
		progr_sqsum[i] /= (double)(i+1);
		progr_error[i] = StdDev(progr_sum[i],progr_sqsum[i],i+1);
	}

	ofstream ave_out("averages.out");
	for (int i=0;i<N;i++){
		ave_out << progr_sum[i] << "  " << progr_error[i] << endl;
		//err_out << progr_error[i] << "\n";
	}
	ave_out.close();
	//err_out.close();

	//COMPUTING AND PRINTING VARIANCE
	for (int i=0;i<N;i++){
		sum=0;
		for(int j=0;j<M/N;j++)
			sum+= pow((r[i*L+j]-0.5),2)/L;
		ave[i] = sum;				//reusing vector ave[i], now is variance, not average
		sq_ave[i] = ave[i]*ave[i];		//same with square
	
	}
	for (int i=0;i<N;i++){
		progr_sum[i]=0;
		progr_sqsum[i]=0;
		for(int j=0;j<=i;j++){
			progr_sum[i]+=ave[j];
			progr_sqsum[i]+=sq_ave[j];
		}
		progr_sum[i] /= (double)(i+1);
		progr_sqsum[i] /= (double)(i+1);
		progr_error[i] = StdDev(progr_sum[i],progr_sqsum[i],i+1);
	}

	ave_out.open("variance.out");
	//err_out.open("var_error.dat");
	for (int i=0;i<N;i++){
		ave_out << progr_sum[i] << "  " << progr_error[i] << endl;
		//err_out << progr_error[i] << "\n";
	}
	ave_out.close();
	//err_out.close(); 
return 0;
}

//****************************************************************

double StdDev(double mean, double sq_mean, int n){

	return sqrt (( sq_mean - mean*mean )/n);

}







