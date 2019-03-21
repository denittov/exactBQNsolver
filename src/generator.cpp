/*
 * generator.cpp
 * 
 * Last modify: 2010/11/02
 * 
 * Author: Massimiliano Macchia
 * E-mail: massimiliano.macchia@gmail.com
 * 
 * 
 * Copyright (C) 2010 Massimiliano Macchia.
 * 
 * This file is part of qnsolver.
 * 
 * qnsolver is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include <errno.h>


using namespace std;

unsigned short *B;

// number of stations
unsigned short M;
// number of job in the net
unsigned char N;

// blocking type for each station
unsigned short *blockType;
// number of servers for each station
unsigned short *m;
// mean service time for each station (mean service time of the single server)
double *Ets;
// mean service rate for each station (1/E[ts])
double *mu;
// routing matrix
double **P;


#include "constants.h"
#include "state.h"


int main(int argc, char *argv[])
{
	if( argc < 5 ) {
		cerr << "[generator] Usage: generator num_job net_file states_file transitions_file\n" << endl;
		exit(1);
	}

	// takes the number of jobs as first argument 
	N = atoi(argv[1]);

	FILE *fd;
	fd = fopen(argv[2],"r");
	if(fd == NULL) {
		cout << "[generator] Unable to open file '"<<argv[2]<<"' (Errno: "<<errno<<").\n" << endl;
		exit(1);
	}

	/* read number of station from file */
	if(fscanf(fd,"%hu\n",(unsigned short*)&M) < 1) {
		cerr << "[generator] Unable to read number of station from file.\n" << endl;
		exit(1);
	}

	/* array allocation */
	blockType = (unsigned short*)calloc(M,sizeof(unsigned short));
	B = (unsigned short*)calloc(M, sizeof(unsigned short));
	m = (unsigned short*)calloc(M, sizeof(unsigned short));
	Ets = (double*)calloc(M, sizeof(double));
	mu = (double*)calloc(M, sizeof(double));
	P = (double**)calloc(M, sizeof(double*));

	unsigned short i, j;
	/* complete the allocation of matrix P */
	for(i=0; i<M; ++i) {
		P[i] = (double*)calloc(M, sizeof(double));
	}

	double temp;
	unsigned short x, y;
	/* matrix P initialization */
	while( fscanf(fd,"(%hu,%hu,%lf)\n",&x,&y,&temp) == 3 ) {
		P[x][y] = temp;
	}

	/* read properties of the M stations from file */
	for(i = 0; i < M; ++i) {
		if(fscanf(fd,"[%hu,%hu,%hu,%lf]\n", &(blockType[i]),&(B[i]),(unsigned short*)&(m[i]), &(Ets[i])) < 4) {
			cerr << "[generator] Unable to read station properties: wrong input file format.\n" << endl;
			exit(1);
		}
		if(blockType[i]==0) { //IS
			B[i] = N;
			m[i] = N;
		}
	}

	fclose(fd);

	// compute mu for each station
	for(i = 0; i < M; ++i) {
		mu[i] = 1.0 / Ets[i];
	}

	states_list sl;

	sl.init();

	/* print the list of states to file */
	sl.print_to_file(argv[3]);
	/* print the list of transitions to file */
	sl.tl.print_to_file(argv[4], sl.size());

	//cerr << "\nN. stati: " << ls.size() << endl;
	//cerr << "N. transizioni: " << ls.lt.size() << endl;
	//long double E_quadro = (long double)ls.size()*(long double)ls.size();
	//cerr << "Sparsita': " << ((long double)ls.lt.size()/E_quadro) << endl << endl;

	return 0;
}

