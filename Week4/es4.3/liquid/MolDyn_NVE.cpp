/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
#include <stdlib.h>     // srand, rand: to generate random number
#include <iostream>     // cin, cout: Standard Input/Output Streams Library
#include <fstream>      // Stream class to both read and write from/to files.
#include <cmath>        // rint, pow
#include "MolDyn_NVE.h"

using namespace std;

int main(){ 

  Input();             //Inizialization

  int nconf = 1;
  for(int istep=1; istep <= nstep - 1; ++istep){ //the last one is going to be done by ConfFinal
     Move();           //Move particles with Verlet algorithm
     if(istep%iprint == 0) cout << "Number of time-steps: " << istep << endl;
     if(istep%10 == 0){
        Measure();     //Properties measurement
	Sum();
        ConfXYZ(nconf);//Write actual configuration in XYZ format //Commented to avoid "filesystem full"! 
        nconf += 1;
     }
     if(istep%blk_len == 0){
	Average(istep/blk_len);
	}

  }
  ConfFinal();         //Write final configuration to restart
  Print_Ave();  


  return 0;
}


void Input(void){ //Prepare all stuff for the simulation
  ifstream ReadInput,ReadConf,ReadOld;
  double ep, ek, pr, et, vir;
  double scale_fac;

  cout << "Classic Lennard-Jones fluid        " << endl;
  cout << "Molecular dynamics simulation in NVE ensemble  " << endl << endl;
  cout << "Interatomic potential v(r) = 4 * [(1/r)^12 - (1/r)^6]" << endl << endl;
  cout << "The program uses Lennard-Jones units " << endl;

  seed = 1;    //Set seed for random numbers
  srand(seed); //Initialize random number generator
  
  ReadInput.open("input.dat"); //Read input

  ReadInput >> temp;

  ReadInput >> npart;
  cout << "Number of particles = " << npart << endl;

  ReadInput >> rho;
  cout << "Density of particles = " << rho << endl;
  vol = (double)npart/rho;
  cout << "Volume of the simulation box = " << vol << endl;
  box = pow(vol,1.0/3.0);
  cout << "Edge of the simulation box = " << box << endl;

  ReadInput >> rcut;
  ReadInput >> delta;
  ReadInput >> nstep;
  ReadInput >> iprint;
  ReadInput >> restart;
  ReadInput >> nblocks;
  
  blk_len = nstep/nblocks;
  ave = new double * [nblocks];
  for(int i=0;i<nblocks;i++) { ave[i] = new double[4];}

  cout << "The program integrates Newton equations with the Verlet method " << endl;
  cout << "Time step = " << delta << endl;
  cout << "Number of steps = " << nstep << endl << endl;

  //cout << "restart = " << restart;

  ReadInput.close();

//Prepare array for measurements
  iv = 0; //Potential energy
  ik = 1; //Kinetic energy
  ie = 2; //Total energy
  it = 3; //Temperature
  n_props = 4; //Number of observables

//Read initial configuration
  cout << "Read initial configuration from file config.0 " << endl << endl;
  ReadConf.open("config.0");
  for (int i=0; i<npart; ++i){
    ReadConf >> x[i] >> y[i] >> z[i];
    x[i] = x[i] * box;
    y[i] = y[i] * box;
    z[i] = z[i] * box;
  }
  ReadConf.close();


   if(!restart){
 

	//Prepare initial velocities
	   cout << "Prepare random velocities with center of mass velocity equal to zero " << endl << endl;
	   double sumv[3] = {0.0, 0.0, 0.0};
	 
	  
	   for (int i=0; i<npart; ++i){

	   	  vx[i] = rand()/double(RAND_MAX) - 0.5;
	  	  vy[i] = rand()/double(RAND_MAX) - 0.5;
	  	  vz[i] = rand()/double(RAND_MAX) - 0.5;


	     sumv[0] += vx[i];
	     sumv[1] += vy[i];
	     sumv[2] += vz[i];
	   }
	   for (int idim=0; idim<3; ++idim) sumv[idim] /= (double)npart;

	  // cout<< "\n\n The average velocity is: [" << sumv[0] << ", " << sumv[1] << ", " << sumv[2] << "]";

	   double sumv2 = 0.0, fs;
	   for (int i=0; i<npart; ++i){
	     vx[i] = vx[i] - sumv[0];
	     vy[i] = vy[i] - sumv[1];
	     vz[i] = vz[i] - sumv[2];

	     sumv2 += vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i];
	   }
	/*   CHECK IF THE MEAN VELOCITY IS ZERO
	double check[3]={0.,0.,0.};
	   for(int i=0; i<npart;i++){

		check[0] += vx[i];
		check[1] += vx[i];
		check[2] += vx[i];

	}

	for(int i=0;i<3;i++){check[i]/=npart;}

	cout<< "\n\n The average velocity is: [" << check[0] << ", " << check[1] << ", " << check[2] << "]";	   
	*/



	   sumv2 /= (double)npart;

	   fs = sqrt(3 * temp / sumv2);   // fs = velocity scale factor 
	   for (int i=0; i<npart; ++i){
	     vx[i] *= fs;
	     vy[i] *= fs;
	     vz[i] *= fs;

	     xold[i] = Pbc(x[i] - vx[i] * delta);
	     yold[i] = Pbc(y[i] - vy[i] * delta);
	     zold[i] = Pbc(z[i] - vz[i] * delta);
	   }

   }

   if(restart){

	ReadOld.open("old.0");

	for (int i=0; i<npart; ++i){
		ReadOld >> xold[i] >> yold[i] >> zold[i];
		xold[i]*=box;
		yold[i]*=box;
		zold[i]*=box;
	}
	
	ReadOld.close();
	

	Move();

	double sumv[3] = {0.0, 0.0, 0.0};

	for(int i=0; i<npart; i++){
		vx[i] = Pbc(x[i] - xold[i]) / delta;
	  	vy[i] = Pbc(y[i] - yold[i]) / delta;
	  	vz[i] = Pbc(z[i] - zold[i]) / delta;

		sumv[0] += vx[i];
		sumv[1] += vy[i];
		sumv[2] += vz[i];
	}
	
	sumv[0] /= npart;
	sumv[1] /= npart;
	sumv[2] /= npart;


	//cout<< "\n\n The average velocity is: [" << sumv[0] << ", " << sumv[1] << ", " << sumv[2] << "]";

	for(int i=0; i<npart;i++){

		vx[i] -= sumv[0];
		vy[i] -= sumv[1];
		vz[i] -= sumv[2];

	}

	/*double check[3]={0.,0.,0.};

	for(int i=0; i<npart;i++){

		check[0] += vx[i];
		check[1] += vx[i];
		check[2] += vx[i];

	}

	for(int i=0;i<3;i++){check[i]/=npart;}

	cout<< "\n\n The average velocity is: [" << check[0] << ", " << check[1] << ", " << check[2] << "]";

	*/

	Measure();

	cout << "The computed temperature is: " << stima_temp;

	scale_fac =  sqrt(temp/stima_temp);

	cout << endl << "To get to the desired: " << temp;
	cout << endl << "Scale factor: " << scale_fac << endl << endl;


	for(int i=0; i<npart; i++){   //rescaling velocities to obtain desired temp

		vx[i]*=scale_fac;
		vy[i]*=scale_fac;
		vz[i]*=scale_fac;
	}
	
	for(int i=0;i<npart; i++){  // estimate "old" position with rescaled velocities

		xold[i] = Pbc(x[i] - vx[i] * delta);
		yold[i] = Pbc(y[i] - vy[i] * delta);
		zold[i] = Pbc(z[i] - vz[i] * delta);
	}




   }

   ofstream Epot, Ekin, Etot, Temp;    

   Epot.open("output_epot.dat",ios::trunc); // clearing output files
   Ekin.open("output_ekin.dat",ios::trunc);
   Temp.open("output_temp.dat",ios::trunc);
   Etot.open("output_etot.dat",ios::trunc);

   Epot.close();
   Ekin.close();
   Temp.close();
   Etot.close();

   Epot.open("ave_epot.out",ios::trunc); // clearing output files
   Ekin.open("ave_ekin.out",ios::trunc);
   Temp.open("ave_temp.out",ios::trunc);
   Etot.open("ave_etot.out",ios::trunc);

   Epot.close();
   Ekin.close();
   Temp.close();
   Etot.close();



   return;
}


void Move(void){ //Move particles with Verlet algorithm
  double xnew, ynew, znew, fx[m_part], fy[m_part], fz[m_part];

  for(int i=0; i<npart; ++i){ //Force acting on particle i
    fx[i] = Force(i,0);
    fy[i] = Force(i,1);
    fz[i] = Force(i,2);
  }

  for(int i=0; i<npart; ++i){ //Verlet integration scheme

    xnew = Pbc( 2.0 * x[i] - xold[i] + fx[i] * pow(delta,2) );
    ynew = Pbc( 2.0 * y[i] - yold[i] + fy[i] * pow(delta,2) );
    znew = Pbc( 2.0 * z[i] - zold[i] + fz[i] * pow(delta,2) );

    vx[i] = Pbc(xnew - xold[i])/(2.0 * delta);
    vy[i] = Pbc(ynew - yold[i])/(2.0 * delta);
    vz[i] = Pbc(znew - zold[i])/(2.0 * delta);

    xold[i] = x[i];
    yold[i] = y[i];
    zold[i] = z[i];

    x[i] = xnew;
    y[i] = ynew;
    z[i] = znew;
  }
  return;
}

double Force(int ip, int idir){ //Compute forces as -Grad_ip V(r)
  double f=0.0;
  double dvec[3], dr;

  for (int i=0; i<npart; ++i){
    if(i != ip){
      dvec[0] = Pbc( x[ip] - x[i] );  // distance ip-i in pbc
      dvec[1] = Pbc( y[ip] - y[i] );
      dvec[2] = Pbc( z[ip] - z[i] );

      dr = dvec[0]*dvec[0] + dvec[1]*dvec[1] + dvec[2]*dvec[2];
      dr = sqrt(dr);

      if(dr < rcut){
        f += dvec[idir] * (48.0/pow(dr,14) - 24.0/pow(dr,8)); // -Grad_ip V(r)
      }
    }
  }
  
  return f;
}

void Measure(){ //Properties measurement
  int bin;
  double v, t, vij;
  double dx, dy, dz, dr;
  ofstream Epot, Ekin, Etot, Temp;

  Epot.open("output_epot.dat",ios::app);// (ios::app stands for append)
  Ekin.open("output_ekin.dat",ios::app);
  Temp.open("output_temp.dat",ios::app);
  Etot.open("output_etot.dat",ios::app);

  v = 0.0; //reset observables
  t = 0.0;

//cycle over pairs of particles
  for (int i=0; i<npart-1; ++i){
    for (int j=i+1; j<npart; ++j){

     dx = Pbc( x[i] - x[j] );
     dy = Pbc( y[i] - y[j] );
     dz = Pbc( z[i] - z[j] );

     dr = dx*dx + dy*dy + dz*dz;
     dr = sqrt(dr);

     if(dr < rcut){
       vij = 4.0/pow(dr,12) - 4.0/pow(dr,6);

//Potential energy
       v += vij;
     }
    }          
  }

//Kinetic energy
  for (int i=0; i<npart; ++i) t += 0.5 * (vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i]);
   
    stima_pot = v/(double)npart; //Potential energy per particle
    stima_kin = t/(double)npart; //Kinetic energy per particle
    stima_temp = (2.0 / 3.0) * t/(double)npart; //Temperature
    stima_etot = (t+v)/(double)npart; //Total energy per particle

    Epot << stima_pot  << endl;
    Ekin << stima_kin  << endl;
    Temp << stima_temp << endl;
    Etot << stima_etot << endl;

    Epot.close();
    Ekin.close();
    Temp.close();
    Etot.close();

    return;
}


void ConfFinal(void){ //Write final configuration
  ofstream WriteConf, WriteOld;

  cout << endl << "Print final two configurations to old.final and config.final";
 
  WriteOld.open("old.final");

  for (int i=0; i<npart; ++i){
    WriteOld << xold[i]/box << "   " <<  yold[i]/box << "   " << zold[i]/box << endl;
  }
  WriteOld.close();

  //Move();
  Measure();

  WriteConf.open("config.final");

  for (int i=0; i<npart; ++i){
    WriteConf << x[i]/box << "   " <<  y[i]/box << "   " << z[i]/box << endl;
  }
  WriteConf.close();




  return;
}

void ConfXYZ(int nconf){ //Write configuration in .xyz format
  ofstream WriteXYZ;

  WriteXYZ.open("frames/config_" + to_string(nconf) + ".xyz");
  WriteXYZ << npart << endl;
  WriteXYZ << "This is only a comment!" << endl;
  for (int i=0; i<npart; ++i){
    WriteXYZ << "LJ  " << Pbc(x[i]) << "   " <<  Pbc(y[i]) << "   " << Pbc(z[i]) << endl;
  }
  WriteXYZ.close();
}

double Pbc(double r){  //Algorithm for periodic boundary conditions with side L=box
    return r - box * rint(r/box);
}

void Sum(){

  Ekin_ave+= stima_kin;
  Epot_ave+= stima_pot;
  Etot_ave+= stima_etot;
  Temp_ave+= stima_temp;

}

void Average(int iblock){

  Epot_ave /= (blk_len/10);
  Ekin_ave /= (blk_len/10);
  Etot_ave /= (blk_len/10);
  Temp_ave /= (blk_len/10);

  ave[iblock-1][0] = Epot_ave;   // 0 = Potential
  ave[iblock-1][1] = Ekin_ave;   // 1 = Kinetic
  ave[iblock-1][2] = Etot_ave;   // 2 = Total Energy
  ave[iblock-1][3] = Temp_ave;   // 3 = Temperature

  Epot_ave=0;
  Ekin_ave=0;
  Etot_ave=0;
  Temp_ave=0;

}

void Print_Ave(){

	cout << endl << "Print average quantities to ave_$QUANTITY$.out" << endl;


	double prog_sum[4] = {0.,0.,0.,0.};
	double prog_sq[4] = {0.,0.,0.,0.};
	double error[4];
	ofstream Epot, Ekin, Etot, Temp;


	for(int q=0;q<4;q++) {    // q for quantity
		for(int k=0;k<nblocks;k++){
			prog_sum[q]+= ave[k][q];
			prog_sq[q]+= ave[k][q]*ave[k][q];
		}	
		prog_sum[q]/=nblocks;
		prog_sq[q]/=nblocks;
		

		error[q]=StdDev( prog_sum[q], prog_sq[q], nblocks);

	}

	Epot.open("ave_epot.out",ios::app);// (ios::app stands for append)
	Ekin.open("ave_ekin.out",ios::app);
	Temp.open("ave_etot.out",ios::app);
	Etot.open("ave_temp.out",ios::app);

	Epot << prog_sum[0] << endl << error[0];
	Ekin << prog_sum[1] << endl << error[1];
	Temp << prog_sum[2] << endl << error[2];
	Etot << prog_sum[3] << endl << error[3];

	Epot.close();
	Ekin.close();
	Temp.close();
	Etot.close();

}




double StdDev(double mean, double sq_mean, int n){

	return sqrt (( sq_mean - mean*mean )/n);

}



/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
