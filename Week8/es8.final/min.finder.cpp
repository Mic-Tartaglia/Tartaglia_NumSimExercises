#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"
#include "es8.2.h"

using namespace std;
 
int main (int argc, char *argv[]){

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

/*****************************************/

   double x0 = 0.5;

   ofstream Insta_out("insta.out");

   x = x0;

   sigma = 0.605;

   do{
	mu = 0.795;
	   do{

		cout << endl << "------------" << endl;
		cout << "mu = " << mu << "  ;  sigma = "  << sigma << endl;
		cout << "------------" << endl;

		for(int iblock = 1; iblock <=nblocks;iblock++){ //produce data
			
			Blk_Reset(iblock);
			
			for(int istep = 0; istep <nsteps;istep++){
				Move();
				Measure(iblock,istep);
				//Insta_out << H[istep] << endl;
			}

			Average(iblock);

			if(iblock%10 == 0){
				cout << endl << "Completed " << iblock << "/" << nblocks;
			}
		}

		Blocking_ave();
		Print_ave();

		mu+=0.01;


	   }while(mu <= 0.825);

	sigma+=0.001;
   
   }while(sigma<=0.615);


   rnd.SaveSeed();
   return 0;

}


/************************************************************************************/

void Blk_Reset(int iblock){

	H_ave[iblock-1] = 0;
	prog_sum[iblock-1] = 0;
	prog_sum2[iblock-1] = 0;
	
}


void Move(){

	double L = 1.;

	xnew = x + rnd.Rannyu(-L,L);

	double alpha = min( 1., Psi_square(xnew,mu,sigma)/Psi_square(x,mu,sigma) );

	if(rnd.Rannyu() < alpha){
		x = xnew;
	}

}

void Measure(int iblock,int istep){

	H[istep] = H_Psi(x,mu,sigma)/Psi_trial(x,mu,sigma);

}

void Blocking_ave(){

	for(int iblock = 0; iblock < nblocks;iblock++){
		
		for (int j =0; j<=iblock;j++){
			prog_sum[iblock]+= H_ave[j];
			prog_sum2[iblock]+= H_ave[j]*H_ave[j];
		}

		prog_sum[iblock]/= (double)(iblock+1.);
		prog_sum2[iblock]/= (double)(iblock+1.);	
		prog_err[iblock] = sqrt( prog_sum2[iblock] - pow(prog_sum[iblock],2)) / sqrt(iblock+1);
	
   }
}

void Print_ave(){

	ofstream H_out;

	H_out.open("H.out",ios::app);

	H_out << mu << " " << sigma << " " << prog_sum[nblocks-1] << " " << prog_err[nblocks-1] << endl;

	//for(int i =0;i<nblocks;i++)
	//	H_out << prog_sum[i] << " " << prog_err[i] << endl;

}

void Average(int iblock){

	for(int i = 0;i <nsteps;i++){
		H_ave[iblock-1]+= H[i];
	}
	H_ave[iblock-1]/=nsteps;

}






double Potential(double x){
	return pow(x,4) - 5./2.*pow(x,2);
   }

   double Potential_first(double x){
	return 4.*pow(x,3) - 5.*x;
   }

   double Potential_second(double x){
	return 12.*pow(x,2);
   }


   double Psi_trial(double x, double mu, double sigma){

	double e_plus = exp(-(x+mu)*(x+mu)/(2.*sigma*sigma));
	double e_minus = exp(-(x-mu)*(x-mu)/(2.*sigma*sigma));

	return e_minus + e_plus;
   }
  
   double Psi_square(double x, double mu, double sigma){
	return Psi_trial(x,mu,sigma)*Psi_trial(x,mu,sigma);
   }



   double Psi_first(double x, double mu, double sigma){

	double e_plus = exp(-(x+mu)*(x+mu)/(2.*sigma*sigma));
	double e_minus = exp(-(x-mu)*(x-mu)/(2.*sigma*sigma));

	return -(x-mu)/pow(sigma,2)*e_minus - (x+mu)/pow(sigma,2)*e_plus;
   }
   double Psi_second(double x, double mu, double sigma){

	double e_plus = exp(-(x+mu)*(x+mu)/(2.*sigma*sigma));
	double e_minus = exp(-(x-mu)*(x-mu)/(2.*sigma*sigma));

	return (pow(x-mu,2) - pow(sigma,2))/pow(sigma,4) * e_minus + (pow(x+mu,2) - pow(sigma,2))/pow(sigma,4) * e_plus;
   }

   double H_Psi(double x,double mu,double sigma){
 	return - hbar/(2.*mass) * Psi_second(x,mu,sigma) + Potential(x)*Psi_trial(x,mu,sigma);
   }

