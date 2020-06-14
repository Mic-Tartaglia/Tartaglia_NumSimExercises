
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"
#include "es10.1.h"

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
	
	for (int i = 0;i<len;i++){
		double x = rnd.Rannyu(-1,1);
		double y = rnd.Rannyu(-1,1);
		P1.SetCity(x,y,i);
	}

	double Pc = 0.7;
	double Pm = 0.2;

	int Nsteps = 10000;
	int iblk;

	double T_start = 1.;

	double T = T_start;

	Path path(&P1);
	path.Scramble(100,&rnd);
	
	Path min_path(&P1);
	for(int icity = 0; icity < path.size; icity++){
				min_path.path[icity] = path.path[icity]; //generate a path variable holding the minimum, in case the "walk" escapes from it
	}

	
	path.Measure_L2();
	cout << "First Path Length:  " << path.length << endl;

	iblk = 1;

	do{

		for(int istep = 0; istep < Nsteps; istep++){
				
			Path new_path(&P1); 
			for(int icity = 0; icity < path.size; icity++){
				new_path.path[icity] = path.path[icity]; //generate a copy of the path
			}

			double p_mutate = 3.*rnd.Rannyu();

			if(p_mutate <= 1)			//mutate: one of the three has to occur
				new_path.Mutate_swap(&rnd);
			if(p_mutate > 1 and p_mutate <=2)
				new_path.Mutate_shift(&rnd);
			if(p_mutate > 2)
				new_path.Mutate_invert(&rnd);

			double p_move = min( 1., new_path.Boltzmann(T)/path.Boltzmann(T) ); //metropolis acceptance probability

			if(rnd.Rannyu() < p_move){
				
				//cout << endl << "Moved with probability " << p_move;

				for(int icity = 0; icity < path.size; icity++){		//if accepted, replace old path with new
					path.path[icity] = new_path.path[icity];
				}
			}else{}//cout << endl << "Rejected despite probability " << p_move;}
				
			min_path.Measure_L2(); //store length for 'min_path'...
					       //...for 'path' it has already been done in "path.Boltzmann(T)"
			if (path.length < min_path.length){
				for(int icity = 0; icity < path.size; icity++){
					min_path.path[icity] = path.path[icity]; //store the minimum if it is a new one
				}

			}
		}

		//cout << iblk << "th Path Length:  " << path.length << endl;
		iblk+=1;	

		min_path.Measure_L2();
		min_path.OutLength();

		T-=0.002;
		//cout << "Cooling to T= " << T << endl;

	}while(T>0.006);

	min_path.OutFinal();

	//cout << "First Path Length:  " << path.length;




   rnd.SaveSeed();
   return 0;
}


