
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"
#include "es10.2.h"

#include "mpi.h"

using namespace std;


int main (int argc, char *argv[]){

   int size,rank;

   MPI_Init(&argc,&argv);

   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Status stat;

   /**************************/

   Random rnd;
   int seed[4];
   int p1, p2;
   ifstream Primes("Primes");
   if (Primes.is_open()){
	int count =0;
	do{
		Primes >> p1 >> p2;   //differentiate seed for different cores
		count+=1;
	}while(count<rank);
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
	
	ifstream City_in("cities.dat");

	for (int i = 0;i<len;i++){
		double x;
		double y;
	
		City_in >> x >> y;

		P1.SetCity(x,y,i);
	}
	City_in.close();

	double Pc = 0.6;
	double Pm = 0.2;

	int Popsize = 1000; // use even number
	int Nsteps = 1000;
	int Nmigr = 50;

	if(size != 4){
		cerr << "------------------" << endl << "WARNING: THIS PROGRAM NEEDS TO USE FOUR CORES" << endl << "------------------";
		return -1;
	}


	//cout << "Sono il nodo "<<rank<<" dei "<<size<<" che hai utilizzato" << endl;
	
	Population popol( Popsize, &P1, 200, &rnd);
	Path min_path(&P1);

	for(int i=0;i<popol.size;i++)
		popol.paths[i].Measure_L2();

	popol.Order();

	//popol.LengthPrint();  //useful to check the cores use actually different paths

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

		popol.Order(); 	 // order population with regard to length

		if ((istep +1 )%(Nsteps/4) == 0){
			cout << endl << "-------------" << endl;
			cout << "Core n°" << rank << ":iterated " << istep + 1 <<"/"<< Nsteps << endl;
			//popol.LengthPrint();
		}
		if ((istep+1)%(Nsteps/50) == 0){
			popol.BestAve();
			popol.BestPath();
		}
		
		if ((istep+1)%Nmigr==0){  	//migrate best path

			int appo[len];

			if (rank == 0){
				for(int k = 0; k< len; k++){
					MPI_Send(&popol.paths[0].path[k],1,MPI_INTEGER,1,0/*tag*/,MPI_COMM_WORLD); //send 0->1
					//cout << endl << "Sent to 1";
					MPI_Recv(&appo[k],1,MPI_INTEGER,3,3/*tag*/,MPI_COMM_WORLD,&stat);	   //receive 3->0
					//cout << endl << "Received from 3";
					}
			}			
			 if (rank == 1){
				for(int k = 0; k< len; k++){
					MPI_Recv(&appo[k],1,MPI_INTEGER,0,0/*tag*/,MPI_COMM_WORLD,&stat);	 //receive 0->1
					MPI_Send(&popol.paths[0].path[k],1,MPI_INTEGER,2,1/*tag*/,MPI_COMM_WORLD);//send 1->2
				}
			}
			if (rank == 2){
				for(int k = 0; k< len; k++){
					MPI_Recv(&appo[k],1,MPI_INTEGER,1,1/*tag*/,MPI_COMM_WORLD,&stat);	//receive 1->2
					MPI_Send(&popol.paths[0].path[k],1,MPI_INTEGER,3,2/*tag*/,MPI_COMM_WORLD);//send 2->3
				}
			}
			if (rank == 3){
				for(int k = 0; k< len; k++){
					MPI_Recv(&appo[k],1,MPI_INTEGER,2,2/*tag*/,MPI_COMM_WORLD,&stat);	//receive 2->3
					MPI_Send(&popol.paths[0].path[k],1,MPI_INTEGER,0,3/*tag*/,MPI_COMM_WORLD);//send 3->0
				}
			}
			for(int k = 0; k< len; k++){
				popol.paths[0].path[k] = appo[k];
			}
		}
	
		if(istep%10==0){

			min_path.Measure_L2();
			if(popol.paths[0].length < min_path.length){		//update best path found
				for(int icity = 0;icity<len;icity++){
					min_path.path[icity] = popol.paths[0].path[icity];
				}
			}
			min_path.Measure_L2();
			min_path.OutLength(rank);
		}

	}

	popol.LengthPrint();

	popol.OutPrint(rank);


   MPI_Finalize();

   rnd.SaveSeed();
   return 0;
}


