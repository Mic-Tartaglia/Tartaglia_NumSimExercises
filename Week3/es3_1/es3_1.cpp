
#include <iostream>
#include <fstream>
#include <cmath>
#include "random.h"

using namespace std;
 
double StdDev(double mean, double sq_mean, int n){

	return sqrt (( sq_mean - mean*mean )/n);

}

double max( double x1, double x2){

	if (x1 >= x2)
		return x1;
	else
		return x2;
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

	int const M = 100000;
	int const N = 100;
	int L=M/N;


	double const s0 = 100;
	double const T= 1;
	double const K=100;
	double const r= 0.1;
	double const sigma = 0.25;

	//double GAUSS = 1;//PLACEHOLDER FOR GAUSSIAN

	double S[M];
	double profit_put[M];
	double profit_call[M];

	/**************************************/

	for (int k=0;k<M;k++){
		S[k] = s0 * exp( (r-sigma*sigma/2)* T + sigma* rnd.Gauss(0,T));
		profit_call[k]= max( S[k] - K, 0);
		profit_put[k] = max(-S[k] + K, 0);
	}

	double aveC[N], sq_aveC[N], prog_sumC[N], prog_sqC[N], prog_errC[N];
	double aveP[N], sq_aveP[N], prog_sumP[N], prog_sqP[N], prog_errP[N];

	for(int i=0;i<N;i++){ //blocking average
		aveC[i]=0;
		aveP[i]=0;
		prog_sqC[i]=0;
		prog_sqP[i]=0;
		prog_sumC[i]=0;
		prog_sumP[i]=0;	

		for(int k=0;k<L;k++){
			aveC[i]+=profit_call[i*L+k];
			aveP[i]+=profit_put[i*L+k];
		}
		aveC[i]/=L;
		aveP[i]/=L;
		sq_aveC[i]= aveC[i]*aveC[i];
		sq_aveP[i]= aveP[i]*aveP[i];

		for(int j=0;j<i;j++){
			prog_sumC[i]+= aveC[j];
			prog_sumP[i]+= aveP[j];
			prog_sqC[i]+= sq_aveC[j];
			prog_sqP[i]+= sq_aveP[j];
		}

		prog_sumC[i]/=(i+1);
		prog_sumP[i]/=(i+1);		
		prog_sqC[i]/=(i+1);
		prog_sqP[i]/=(i+1);
		prog_errC[i]= StdDev(prog_sumC[i], prog_sqC[i] , i+1);
		prog_errP[i]= StdDev(prog_sumP[i], prog_sqP[i] , i+1);

	}

	ofstream dir_out("direct.out");

	for (int i=0;i<N;i++){
		dir_out << prog_sumC[i] << "  " << prog_errC[i] << "  " << prog_sumP[i] << "  " << prog_errP[i] << endl;
		//cout << prog_sumC[i] << "  " << prog_errC[i] << "  " << prog_sumP[i] << "  " << prog_errP[i] << endl;
	}
	dir_out.close();

	/**************************************/
	
	int ntimes = 100;
	double dt = T/ntimes;

	for (int k=0;k<M;k++){
	
		S[k] = s0;

		for(int j = 0; j<ntimes; j++){
			S[k] = S[k] * exp( (r-sigma*sigma/2)*dt + sigma* rnd.Gauss(0,1)*sqrt(dt));
		}
		profit_call[k]= max(S[k] - K, 0);
		profit_put[k] = max(-S[k] + K, 0);

	}

	for(int i=0;i<N;i++){
		aveC[i]=0;
		aveP[i]=0;
		prog_sqC[i]=0;
		prog_sqP[i]=0;
		prog_sumC[i]=0;
		prog_sumP[i]=0;	

		for(int k=0;k<L;k++){
			aveC[i]+=profit_call[i*L+k];
			aveP[i]+=profit_put[i*L+k];
		}
		
		aveC[i]/=L;
		aveP[i]/=L;
		sq_aveC[i]= aveC[i]*aveC[i];
		sq_aveP[i]= aveP[i]*aveP[i];

		for(int j=0;j<i;j++){
			prog_sumC[i]+= aveC[j];
			prog_sumP[i]+= aveP[j];
			prog_sqC[i]+= sq_aveC[j];
			prog_sqP[i]+= sq_aveP[j];
		}

		prog_sumC[i]/=(i+1);
		prog_sumP[i]/=(i+1);		
		prog_sqC[i]/=(i+1);
		prog_sqP[i]/=(i+1);
		prog_errC[i]= StdDev(prog_sumC[i], prog_sqC[i] , i+1);
		prog_errP[i]= StdDev(prog_sumP[i], prog_sqP[i] , i+1);

	}

	ofstream dis_out("discrete.out");

	for (int i=0;i<N;i++){
		dis_out << prog_sumC[i] << "  " << prog_errC[i] << "  " << prog_sumP[i] << "  " << prog_errP[i] << endl;
	}
	dis_out.close();

	/******************************/

	rnd.SaveSeed();
	return 0;
}

