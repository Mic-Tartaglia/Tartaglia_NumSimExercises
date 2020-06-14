#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "random.h"

using namespace std;

//-------------classes------------//

class City{

   public:
	double x,y;

	City(double,double);
	City();	

	void Set(double,double);
	void Print();
};

class Pool{

   public:

	City * cities;
	int size;

	Pool(int);

	void SetCity(City,int);
	void SetCity(double,double,int);
};


class Path{

   public:
	
	int * path;
	Pool * pool;

	int size;
	double length;

	Path(Pool*);//creates a default ordered path 1,2,3,4....
	Path(); //Not to use alone, combine with SetPool
	void SetPool(Pool*);
	
	void Scramble(int, Random*);
	void Swap(int, int);
	void Measure_L2();
	void Measure_L1();

	//Mutation operators
	void Mutate_swap(Random*);
	void Mutate_shift(Random*);
	void Mutate_invert(Random*);

	void Print();
	void OutLength(int);

};

class Population{

   public:

	Path * paths;
	int size;

	Population(int,Pool*,int,Random*);

	void FullPrint();
	void LengthPrint();
	void Order();
	void Select(Random *);
	void Crossover(Random *);

	void BestAve();
	void BestPath();

	void OutPrint(int);

	int parent1 ;
	int parent2 ;


};

//-------------functions------------//

double Distance(City,City);
int Pbc(int,int);


//-------------definitions------------//

Population::Population(int N, Pool * Input_pool, int N_Scrambles, Random * rnd){

	size = N;

	paths = new Path[N];
	for(int i = 0;i<N;i++){
		paths[i].SetPool( Input_pool);
		paths[i].Scramble( N_Scrambles, rnd); //does N_scrambles permutations of two elements
	}
}

void Population::FullPrint(){
	
	for(int i = 0;i<size;i++){
		cout << endl;
		paths[i].Print();
	}

}

void Population::Order(){

	for(int i=0;i<size;i++){
		//paths[i].Measure_L1();   //can use either
		paths[i].Measure_L2();
	}

	Path min_path(paths[0].pool);
	int min_index = 0;
	//int len = paths[0].size;

	for(int i = 0; i<size-1 ;i++){ 	//selection sort
		min_index = i;
		for(int j = i+1;j<size;j++){
			if( paths[j].length < paths[min_index].length ){
				min_index = j;
			}
		}

		min_path = paths[min_index];
		paths[min_index] = paths[i];
		paths[i] = min_path;
		//for(int k=0;k<len;k++)
		//	min_path.path[k] = paths[min_index].path[k];
		//for(int k=0;k<len;k++)
		//	paths[min_index].path[k] = paths[i].path[k];
		//for(int k=0;k<len;k++)
		//	paths[i].path[k] = min_path.path[k];
	}
	
}

void Population::LengthPrint(){
	cout << endl << "----------------" << endl;
//	for(int i=0;i<size/2;i++){
	for(int i=0;i<10;i++){  //use to print just the 10 shortest paths
		//paths[i].Measure_L1();
		paths[i].Measure_L2();
		cout << endl << "Path " << i+1 << " length= " << paths[i].length;
	}


}

void Population::Select(Random * rnd){

	double p = 1.75;
	double r1 = pow(rnd->Rannyu(),p);

	parent1 = (int)(r1*size); // index of first parent
	do{
		double r2 = pow(rnd->Rannyu(),p);
		parent2 = (int)(r2*size); //index of second parent	
	}while(parent2 == parent1);

}

void Population::Crossover(Random * rnd){

	int len = paths[0].size;
	int icross = 1 + (int)(  (len-2)* rnd->Rannyu() ); // random index between 1 and len-1
	

	Path Son1(paths[parent1].pool), Son2(paths[parent1].pool);
	Path Parent1 = paths[parent1]; //lower case is index, upper is path
	Path Parent2 = paths[parent2];


	for (int i = 0;i <= icross;i++){ 
		Son1.path[i] = Parent1.path[i]; //copy first part as is
		Son2.path[i] = Parent2.path[i];
	}
	/* //UNCOMMENT TO CHECK PARENTS
	cout << endl << "parents:" << endl;
	for(int i =0;i< len;i++)
		cout << paths[parent1].path[i] << " ";
	cout << endl;
	for(int i =0;i< len;i++)
		cout << paths[parent2].path[i] << " ";
	*/

	if(icross < len-1){
		int icomplete = icross +1;
		while(icomplete <= len-1){
			for(int i = 0;i<len;i++){ //scan first parent
				int counter = 0;
				for(int j = 0;j < icomplete;j++){
					if(paths[parent1].path[i] == Son2.path[j]) {counter+=1;}//if the element is already in the second son do nothing...
				}					  		    	//
				if(counter == 0){						//... else insert in the first available spot
					Son2.path[icomplete] = paths[parent1].path[i];		  
					icomplete +=1;
				}
			}
			
		}
		
		icomplete = icross +1;
		while(icomplete <= len-1){		//same as above, with roles inverted
			for(int i = 0;i<len;i++){
				int counter = 0;
				for(int j = 0;j < icomplete;j++){
					if(paths[parent2].path[i] == Son1.path[j]) {counter+=1;} 
				}					  
				if(counter == 0){
					Son1.path[icomplete] = paths[parent2].path[i];		  
					icomplete +=1;
				}
			}
			
		}
	
	}
	
	/* //UNCOMMENT TO CHECK SONS
	cout << endl << "sons" << endl;
	for(int i =0;i< len;i++)
		cout << Son1.path[i] << " ";
	cout << endl;
	for(int i =0;i< len;i++)
		cout << Son2.path[i] << " ";
	*/
}

void Population::OutPrint(int rank){
	ofstream Path_out;


	Path_out.open("out.path"+to_string(rank)+".final");
		
	
	int len = paths[0].size;
	cout << endl << "Print out best path" << endl;
	for(int i=0;i < len; i++){
		//Path_out <<  paths[0].path[i] << "   ";
		Path_out << paths[0].pool->cities[paths[0].path[i]-1].x << "   " << paths[0].pool->cities[paths[0].path[i]-1].y << endl;

	}



}

void Population::BestAve(){

	ofstream Ave_out("best.ave.out",ios::app);

	int N = size/2;
	double ave=0, ave2=0, err;

	for(int i=0; i<N;i++){
		ave+= paths[i].length;
		ave2+= pow(paths[i].length,2);
	}

	ave /= (double)N;
	ave2 /= (double)N;

	err = sqrt( ave2-ave*ave)/sqrt(N);

	Ave_out << ave << "  " << err << endl;

}

void Population::BestPath(){

	ofstream Path_out("best.path.out",ios::app);

	Path_out << paths[0].length << endl;

}




Pool::Pool(int N){
	
	size = N;
	cities = new City[N];

}


void Pool::SetCity(City Input, int index){

	cities[index] = Input;

}

void Pool::SetCity(double x,double y, int index){

	City appo(x,y);
	cities[index] = appo;

}

Path::Path(){
}

Path::Path(Pool * Input){

	pool = Input;
	int N = Input->size;
	path = new int[N];
	for(int i=0;i<N;i++){ //creates a default ordered path 1,2,3,4....
		path[i] = i+1;
	}

	size = N;
}

void Path::SetPool(Pool * Input){

	pool = Input;
	int N = Input->size;
	path = new int[N];
	for(int i=0;i<N;i++){ //creates a default ordered path 1,2,3,4....
		path[i] = i+1;
	}

	size = N;

}


void Path::Swap(int i1, int i2){

   if( i1<size and i2<size){
	int appo = path[i1];
	path[i1] = path[i2];
	path[i2] = appo;
   }else{ cerr << endl << endl << "EXCEEDED PATH SIZE" << endl << endl;}
}

void Path::Scramble( int n_times, Random * rnd){

	int a,b;

	for(int i=1; i<= n_times;i++){
		do{
			a = (int)(1 + (size-1) * rnd->Rannyu()); // select two random indexes...
			b = (int)(1 + (size-1) * rnd->Rannyu()); // ...but not the first, which is fixed
			Swap(a,b);
		}while(a==b);
	}
}

void Path::Measure_L2(){

	double l = 0;
	for(int i=0;i<size-1;i++) // cycle the path, except for the last one, which needs Pbc
		l += pow(Distance(pool->cities[path[i]-1],pool->cities[path[i+1]-1]),2); 	// this goes to the pool to get the city corresponding to the index
	l+= pow(Distance(pool->cities[path[size-1]-1],pool->cities[path[0]-1]),2);	  	 // indicated by the path, then computes distance (in L2 is squared)

	length = l;

}

void Path::Measure_L1(){
	
	double l = 0;
	for(int i=0;i<size-1;i++) //same as above, just not squared
		l += Distance(pool->cities[path[i]-1],pool->cities[path[i+1]-1]);   
	l+= Distance(pool->cities[path[size-1]-1],pool->cities[path[0]-1]);	//(could have used Pbc)		

	length = l;
}

void Path::Print(){
	for (int i=0;i<size;i++)
		cout << path[i] << " ";
}

void Path::Mutate_swap(Random* rnd){
	Scramble(1,rnd);
}

void Path::Mutate_shift(Random* rnd){
	int shift = (int)( (size - 2)*rnd->Rannyu() ); //the maximum non trivial shift is N-2

	int appo1[size-1], appo2[size-1]; //create vectors of size N-1, to make Pbc easier

	for(int i = 1;i<size;i++){
		appo1[i-1] = path[i];
	}
	for(int i = 0;i<size;i++){
		appo2[i] = appo1[Pbc(i+shift,size-1)];// shifts to the left, shifting to the right needs change in Pbc
	}

	for(int i = 1;i<size;i++){
		path[i] = appo2[i-1];
	}

}

void Path::Mutate_invert(Random* rnd){

	int istart = 1 + (int)( (size - 2)*rnd->Rannyu() ); // first city is not included;

	int len =  (int)( 2+ (size-3)*rnd->Rannyu() ); // inverts 2 to N-1 cells

	int appo1[len], appo2[len]; //create vectors of size N-1, to make Pbc easier



	if (len < size - istart){
		for(int i = 0;i< len;i++){
			appo1[i] = path[istart+i];   // transcribe last (N-istart) indexes	
		}
	}

	if(len > size - istart){

		for(int i = 0;i< (size-istart);i++){
			appo1[i] = path[istart+i];   // transcribe last (N-istart) indexes	
		}

		for(int i = 0; i < (len- (size-istart));i++){ //if needed, transcribe remaining at the start of path (except 1)
			appo1[size - istart + i] = path[1+i];
		}

	}
	
	for(int i = 0;i<len;i++){
		appo2[i]=appo1[len-1-i]; 	//flip indexes
	}
	
	if(len < size - istart){
		for(int i = 0;i< len;i++){		// rewrite in starting vector
			path[istart+i] = appo2[i];
		}
	}

	if(len > size - istart){

		for(int i = 0;i< (size-istart);i++){		// rewrite in starting vector
			path[istart+i] = appo2[i];
		}

		for(int i = 0; i < (len- (size-istart));i++){
			path[1+i] = appo2[size - istart + i];
		}
	}
	

}

void Path::OutLength(int rank){

	ofstream Length_out("out.best.paths."+to_string(rank),ios::app);

	Length_out << length << endl;

}

City::City(){
	x = 0;
	y = 0;
}

City::City(double a, double b){

	x = a;
	y = b;
}

void City::Set(double a, double b){

	x = a;
	y = b;
}

void City::Print(){
	cout << " X = " << x << "\t Y = " << y; 
}

double Distance(City A,City B){

	double l = sqrt( pow((A.x - B.x),2) + pow((A.y - B.y),2));

	return l;

}

int Pbc(int i,int N){

	if (i<N){ return i;}
	else{
		i = i-N;
		return Pbc(i,N);
	}
}




