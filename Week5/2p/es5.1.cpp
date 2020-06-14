
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"

using namespace std;

void SetupGen(Random);
double Radius(double *);
double Theta(double *);
double Psi_100(double *);
double Psi_210(double *);
double Psi_320(double *);
double StdDev(double,double,int);


int main(){

	/************************/ //SETUP RANDOM GEN

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

	/************************/

		
	int nsteps = 1e7;
	
	double x0[3] = { 0., 1., 1.}; //initial position
	double x[3];	//position

	double L = 3.; //cube size for delta

	double alpha; // probability of rejection
	double a;

	double delta[3];//displacement
	double xnew[3];

	int nblocks = 250;
	int blk_len = nsteps / nblocks;
	double prog_sum[nblocks];
	double prog_sq[nblocks];
	double prog_err[nblocks];

	ofstream Ave_out, Err_out;

	ofstream X_out, Y_out, Z_out;

	double ave[nblocks];

	for (int iblock=0;iblock<nblocks;iblock++){ //clearing summation vectors
		ave[iblock]=0;
		prog_sum[iblock]=0;
		prog_sq[iblock]=0;
	}

	for(int idim = 0; idim<3;idim++)  // start from initial position
		x[idim] = x0[idim];

	X_out.open("points/x.out");
	Y_out.open("points/y.out");
	Z_out.open("points/z.out");

	for(int ieq = 0; ieq< 500; ieq++){  //measureless steps for equilibration

		for(int idim=0;idim<3;idim++){
			delta[idim] = rnd.Rannyu( -L,L);
			//delta[idim] = rnd.Gauss(0,L*0.7); // 0.7 is to make acceptance about 50%, could have changed L instead
			xnew[idim] = x[idim] + delta[idim];
		}
		alpha = min ( 1., Psi_100( xnew )/Psi_100( x ));

		double ran = rnd.Rannyu();
		if (ran <= alpha){
			for(int idim=0;idim<3;idim++){
				x[idim]=xnew[idim];
			}
		}
	}


	for(int istep=0; istep<nsteps; istep++){

		if(istep%(nsteps/10) == 0)
			cout << endl << "Processing " << istep << "th step";
		
		if(istep%100 ==0){
			X_out << x[0]<< endl;
			Y_out << x[1]<< endl;
			Z_out << x[2]<< endl;
		}

		for(int idim=0;idim<3;idim++){
			delta[idim] =rnd.Rannyu( -L,L);
			xnew[idim] = x[idim] + delta[idim];
		}
		
		alpha = min ( 1., Psi_210( xnew )/Psi_210( x ));

		double ran = rnd.Rannyu();
		if (ran <= alpha){
			for(int idim=0;idim<3;idim++){
				x[idim]=xnew[idim];
			}
		}
		
	
		ave[istep/blk_len] += Radius(x);

		a+= alpha;

		//cout << endl<< "probability: " << alpha << "\tave[" << istep/blk_len << "]= " << ave[istep/blk_len];

		

	}

	Ave_out.open("average.out");
	Err_out.open("errors.out");

	for (int iblock=0;iblock<nblocks;iblock++){

		ave[iblock] /=blk_len;
		for(int i=0;i<=iblock;i++){
			prog_sum[iblock] += ave[i];
			prog_sq[iblock] += ave[i]*ave[i];
		}
		prog_sum[iblock]/=(iblock+1);
		prog_sq[iblock]/=(iblock+1);


		prog_err[iblock] = StdDev(prog_sum[iblock],prog_sq[iblock],iblock+1);

		Ave_out << prog_sum[iblock] << endl;
		Err_out << prog_err[iblock] << endl;
	}

	Ave_out.close();
	Err_out.close();

	

	cout << endl << "Average acceptance probability: " << a/nsteps; //check, should be around 0.5

	//cout <<endl << "Average Radius:  " << prog_sum[0];
	//cout <<endl << "Average Radius:  " << prog_sum[99];

	


	rnd.SaveSeed();
   return 0;



}








/*************************************************/
 
double Radius( double * x){

	return sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);

}

double Theta( double * pos){

	double x = pos[0];
	double y = pos[1];
	double z = pos[2];

	return  atan( sqrt(x*x + y*y)/z );

}


double Psi_100(double * x){

	double r= Radius(x);

	return pow(  exp(-r)/sqrt(M_PI), 2 )  ;
}

double Psi_210(double * x){

	double r=Radius(x);
	double theta= Theta(x);

	return 1./(32.*M_PI)* r*r *exp(-r) * pow(cos(theta),2); // already square of wave function

}

double Psi_320(double *x){

	double r = Radius(x);
	double theta = Theta(x);

	return 1/(6561.*M_PI) * pow(r,4) * exp( -2./3. * r) * pow( cos(theta)*sin(theta),2);

}



double StdDev(double mean, double sq_mean, int n){

	return sqrt (( sq_mean - mean*mean )/n);

}




