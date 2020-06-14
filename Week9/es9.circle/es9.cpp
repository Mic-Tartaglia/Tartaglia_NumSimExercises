
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"
#include "es9.h"

using namespace std;


int main (int argc, char *argv[]){

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

   rnd.Rannyu();

   /*************************************************/

	int len = 32;   

	Pool P1(len);
	
	
	double R = 1;
	double x = 0;

	for (int i = 0;i<len;i++){
		double r = 2.*M_PI*rnd.Rannyu();
		P1.SetCity(R*cos(r),R*sin(r),i);
	}




	double Pc = 0.5;
	double Pm = 0.2;

	int Popsize = 1000; // use even number
	int Nsteps = 1000;

	Population popol( Popsize, &P1, 200, &rnd);

	//popol.FullPrint();

	for(int i=0;i<popol.size;i++)
		popol.paths[i].Measure_L2();

	//popol.LengthPrint();
	
	cout << endl << "---------------" << endl;

	popol.Order();

	//popol.LengthPrint();

	//popol.FullPrint();

	Population newpopol( Popsize, &P1, 1, &rnd);

	for(int istep=0; istep < Nsteps; istep++){
		popol.Order();
		for(int ifill = 0; ifill< Popsize -1; ifill+=2){
			popol.Select(&rnd);		//select two parents for "breeding"
			if(rnd.Rannyu() < Pc)
				popol.Crossover(&rnd);
			for(int k = 0; k< len;k++){
				newpopol.paths[ifill].path[k] = popol.paths[popol.parent1].path[k]; 	//although the indexes are called "parent", after the crossover
				newpopol.paths[ifill+1].path[k] = popol.paths[popol.parent2].path[k];	//these are the two sons
			}
		}
		newpopol.Order();
		
		for(int irepl = 0; irepl <Popsize; irepl++){		//after new population is created, replace the old one
			for(int k = 0; k< len;k++){
				popol.paths[irepl].path[k] = newpopol.paths[irepl].path[k];
			}
		}
		for(int imut = 0; imut < Popsize; imut++){ 		//give each individual the chance to mutate
			double r1 = rnd.Rannyu();
			if(r1 < Pm){
				popol.paths[imut].Mutate_swap(&rnd);
				//cout << endl << "Path " << imut + 1 << "has mutated: swap";
			}
			double r2 = rnd.Rannyu();
			if(r2 < Pm){
				popol.paths[imut].Mutate_invert(&rnd);
				//cout << endl << "Path " << imut + 1 << "has mutated: invert";
			}
			double r3 = rnd.Rannyu();
			if(r3 < Pm){
				popol.paths[imut].Mutate_shift(&rnd);
				//cout << endl << "Path " << imut + 1 << "has mutated: shift";
			}
		}
		popol.Order();
		if ((istep +1 )%(Nsteps/10) == 0){
			cout << endl << "-------------" << endl;
			cout << "Iterated " << istep + 1 <<"/"<< Nsteps << endl;
			//popol.LengthPrint();
		}
		if ((istep+1)%(Nsteps/50) == 0){
			popol.BestAve();
			popol.BestPath();
		}	
	}

	//popol.LengthPrint();

	popol.OutPrint();

   rnd.SaveSeed();
   return 0;
}


