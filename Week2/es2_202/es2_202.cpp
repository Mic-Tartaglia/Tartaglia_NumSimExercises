
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

	double leftbnd = 0, rightbnd = 1;
		
	int L = M/N;
	double theta, phi;
	double x,y,z;
	double r[M];

	double sum=0;
	
	double position[3];
	double distance[M][steps];

	//ofstream check("check.out");

	for(int i=0;i<M;i++){
		position[0]=0;
		position[1]=0;
		position[2]=0;
		
		for(int k=0;k<steps;k++){
			phi = rnd.Rannyu(0, 2*M_PI);
			theta = 2*asin(sqrt( rnd.Rannyu() ));
	
			//check << cos(theta) << "\n";

			x = sin(theta)*cos(phi);
			y = sin(theta)*sin(phi);
			z = cos(theta);			
	
			//cout << "x=" << x << "\ty=" << y << "\tz=" << z << "\tr="  <<sqrt( x*x + y*y + z*z) << "\n";

			position[0]+=x;
			position[1]+=y;
			position[2]+=z;

			distance[i][k] = sqrt( position[0]*position[0] + position[1]*position[1] + position[2]* position[2]);

		}
	}

	//	cout << "\n" << distance[i][steps-1] << "\n";  //print single distance


	double ave[N][steps], prog_sum[N][steps], prog_sq[N][steps], prog_err[N][steps]; // first index: BLOCK, second index: STEP
	
	//double sq_ave[N][steps];
	//sq_ave[1][1] = 0;

	for(int s=0;s<steps;s++){
		for(int i=0;i<N;i++){
			ave[i][s]=0;
	//		sq_ave[i][s]=0;
			prog_sq[i][s]=0;
			prog_sum[i][s]=0;		

			for(int k=0;k<L;k++)
				ave[i][s]+=distance[i*L+k][s];
			
			ave[i][s]/=L;
	//		sq_ave[i][s]= ave[i][s]*ave[i][s];

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

	/*for (int s=0;s<steps;s++){
		for (int i=0;i<N;i++){
			res_out << prog_sum[i][s] << " ";
			err_out << prog_err[i][s] << " ";
		}
		res_out << "\n";
		err_out << "\n";
	}
	*/

	for (int s=0;s<steps;s++){
		res_out << prog_sum[N-1][s] << "\n";
		err_out << prog_err[N-1][s] << "\n";
	}


	res_out.close();
	err_out.close();

	

//	*/




	//rnd.SaveSeed();
	return 0;
}

