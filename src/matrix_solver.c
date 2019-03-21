/*
 * matrix_solver.c
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

#include <stdio.h>
#include <stdlib.h>

#include "../include/meschach/matrix2.h"
#include "../include/meschach/sparse2.h"
#include "../include/meschach/iter.h"

int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		perror("[matrix_solver] Usage: matrix_solver transitions_file state_prob_file\n");
		exit(1);
	}

	// open the transitions file
	FILE *fd;
	fd = fopen(argv[1],"r");

	int E;
	// read the number of states from file
	if( fscanf(fd,"%d\n",&E) != 1 )
	{
		perror("[matrix_solver] Error: cannot read states number.\n");
		exit(1);
	}

	SPMAT *A;
	VEC *x, *b;
	int num_steps;
	
	int i, j;
	double f;
	unsigned int num[E];
	for(i=0; i<E; ++i) {
		num[i]=0;
	}
	int offset = ftell(fd);
	// compute the number of non-zero values over each row of the transition matrix
	while( fscanf(fd,"[%d,%d,%le]\n",&i,&j,&f) == 3 ) {
		++(num[j]);
	}
	// compute the maximum number (max) of non-zero values over rows
	unsigned int max = 0;
	for(i=0; i<E; ++i) {
		if(num[i] > max)
			max = num[i];
	}

	// allocation of a sparse matrix with ExE (max non-zero elements over rows)
	A = sp_get(E,E,max);
	
	// back to the first transition
	fseek(fd,offset,SEEK_SET);
	// read transitions from file and initialize the transposed Q matrix (Q^T=A)
	while( fscanf(fd,"[%d,%d,%le]\n",&i,&j,&f) == 3 ) {
		sp_set_val(A,j,i,f); // transposed Q matrix
		sp_set_val(A,i,i, sp_get_val(A,i,i) - f );
	}
	fclose(fd);
	
	// known terms vector (b) initialization [1 0 0 ... 0 0]
	b = v_get(E);
	v_set_val(b,0,1.0); // normalization condition (variables sums to 1)
	int k;
	for(k=1; k<E; ++k)
	{
		v_set_val(b,k,0.0);
	}

	// first row substitution with normalization condition (variables sums to 1)
	for(k=0; k<E; ++k)
	{
		sp_set_val(A,0,k,1.0);
	}

	// solution vector initialization
	x = v_get(E);
	int ok;
	int fg = 0;
	double prec = 1e-14;
	do {
		ok = 1;
		// solve the sparse linear system with Modified Gradient Conjugate Residual method
		x = iter_spmgcr(A,NULL,b,prec,NULL,1000,1000,&num_steps);
		// check if solution vector contains negative values (invalid values)
		for(k=0; k<E; ++k)
		{
			if(v_get_val(x,k) < 0.0) {
				// if vector contains negative probability decrease precision (increase permissible error)...
				prec *= 10.0;
				// ... and try again
				ok = 0;
				break;
			}
		}
			
	} while(!ok && prec<1e-3);

	double tot = 0.0;
	double temp;
	// open the state probabilities destination file
	fd = fopen(argv[2],"w");
	for(k=0; k<E; ++k)
	{
		temp = v_get_val(x,k);
		if(temp < 0.0) {
			perror("[matrix_solver] ERROR: negative state probability detected.\n");
			fclose(fd);
			exit(1);
		}
		tot += temp;
		// write state probability to file
		fprintf(fd,"%le\n",temp);
	}
	fclose(fd);
	//fprintf(stderr,"Probabilities sum to (should be 1) %f\n",tot);

	return 0;
} 
