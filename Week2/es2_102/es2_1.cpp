
#include <iostream>
#include <fstream>
#include <cmath>
#include "random.h"

using namespace std;
 
double Integrand (double r){	//defining the integrand function

	double K = 0.6;//2*sqrt(2)/M_PI;	
	double A = K * atan( 1./K);
	

	return  cos( M_PI/2. * r )* A * (1. + r*r/(K*K));

}

double Sample(double r){ 	// distribution for importance sampling

	double K = 0.6;//2*sqrt(2)/M_PI;	
	double A = K * atan( 1/K);

	return K * tan( A * r / K);
} 


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

	int const M = 1e4;
	int const N = 1e2;
		
	int L = M/N;
	double ran;
	double r[M];

	double sum=0;
	
	for(int k=0;k<M;k++){
		ran = rnd.Rannyu(leftbnd, rightbnd);

		ran = Sample(ran);

		r[k] = M_PI/2*Integrand(ran);
		sum+= r[k];
	}

	cout << "\n" << sum/M * (rightbnd - leftbnd) << "\n";  //print single integral

	double ave[N], sq_ave[N], prog_sum[N], prog_sq[N], prog_err[N];

	for(int i=0;i<N;i++){
		ave[i]=0;
		prog_sq[i]=0;
		prog_sum[i]=0;		

		for(int k=0;k<L;k++)
			ave[i]+=r[i*L+k];
		
		ave[i]/=L;
		sq_ave[i]= ave[i]*ave[i];

		for(int j=0;j<i;j++){
			prog_sum[i]+= ave[j];
			prog_sq[i]+= sq_ave[j];
		}
		prog_sum[i]/=(i+1);		
		prog_sq[i]/=(i+1);
		prog_err[i]= StdDev(prog_sum[i], prog_sq[i] , i+1);
	}

	ofstream res_out("results.dat");
	ofstream err_out("errors.dat");

	for (int i=0;i<N;i++){
		res_out << prog_sum[i] << "\n";
		err_out << prog_err[i] << "\n";
	}
	res_out.close();
	err_out.close();

	rnd.SaveSeed();
	return 0;
}

