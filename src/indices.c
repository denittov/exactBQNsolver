/*
 * indices.c
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
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

//#include "../include/meschach/sparse2.h"

#include "constants.h"

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



int main(int argc, char *argv[])
{
	if(argc < 5)
	{
		perror("[indices] Usage: indices num_job net_file states_file state_prob_file\n");
		exit(1);
	}

	// takes the number of jobs as first argument
	N = atoi(argv[1]);

	FILE *fd1, *fd2, *fd3;
	fd1 = fopen(argv[2],"r");
	fd2 = fopen(argv[3],"r");
    fd3 = fopen(argv[4],"r");
    
    
	int active_servers = 0;
	unsigned short i, j;

	/* read number of station from file */
	if(fscanf(fd1,"%hu\n",(unsigned short*)&M) < 1) {
		perror("[indices] Unable to read number of station from file.\n");
		exit(1);
	}
	
	// Output indices file
    FILE *fd_n[M];
	FILE *fd_t[M];
	FILE *fd_X[M];
	FILE *fd_U[M];
	FILE *fd_pb[M];
	// Output file number of station
	FILE *fd_stations;
	
	// number_length is string length of file output name
	int number_length = 1;
	int div = 10;
	while(M/div > 0) {
		++number_length;
		div *= 10;
	}
	
	// file output name
	char *fn_n = (char*)calloc(1+number_length+15+1,sizeof(char));
	char *fn_t = (char*)calloc(1+number_length+15+1,sizeof(char));
	char *fn_X = (char*)calloc(1+number_length+15+1,sizeof(char));
	char *fn_U = (char*)calloc(1+number_length+15+1,sizeof(char));
	char *fn_pb = (char*)calloc(1+number_length+15+1,sizeof(char));
	
	// open file output
	for(j=0; j<M; ++j) {
		sprintf(fn_n,"n%d_EXACT.dat",j);
		fd_n[j] = fopen(fn_n,"w");
		sprintf(fn_t,"t%d_EXACT.dat",j);
		fd_t[j] = fopen(fn_t,"w");
		sprintf(fn_X,"X%d_EXACT.dat",j);
		fd_X[j] = fopen(fn_X,"w");
		sprintf(fn_U,"U%d_EXACT.dat",j);
		fd_U[j] = fopen(fn_U,"w");	
		sprintf(fn_pb,"PB%d_EXACT.dat",j);
		fd_pb[j] = fopen(fn_pb,"w");	
	}
	
	// copy number of stations in file
	fd_stations = fopen("station_num.txt","w");
	fprintf(fd_stations,"%d",M);
	fclose(fd_stations);
	


	/* array allocation */
	blockType = (unsigned short*)calloc(M,sizeof(unsigned short));
	B = (unsigned short*)calloc(M, sizeof(unsigned short));
	m = (unsigned short*)calloc(M, sizeof(unsigned short));
	Ets = (double*)calloc(M, sizeof(double));
	mu = (double*)calloc(M, sizeof(double));
	P = (double**)calloc(M, sizeof(double*));

	unsigned short *n = (unsigned short *)calloc(M,sizeof(unsigned short));
	unsigned short *bs = (unsigned short *)calloc(M,sizeof(unsigned short));

	/* complete the allocation of matrix P */
	for(i=0; i<M; ++i) {
		P[i] = (double*)calloc(M, sizeof(double));
	}
	
	
	printf("---------------NET DESCRIPTION---------------\n");
	printf("ROUTING (source_station,destination_station,routing_probability)\n");

	double temp;
	while( fscanf(fd1,"(%hu,%hu,%lf)\n",&i,&j,&temp) == 3 ) {
		P[i][j] = temp;
		printf("  (%hu,%hu,%lf)\n",i,j,temp);
	}
	
	printf("\nSTATION PROPERTIES [blocking_type,queue_capacity,number_of_servers,mean_service_time]\n");

	/* read properties of the M stations from file */
	for(i = 0; i < M; ++i) {
		if(fscanf(fd1,"[%hu,%hu,%hu,%lf]\n", &(blockType[i]),&(B[i]),(unsigned short*)&(m[i]), &(Ets[i])) < 4) {
			perror("[indices] Unable to read station properties: wrong input file format.\n");
			exit(1);
		}
		mu[i] = 1.0 / Ets[i];
		if(blockType[i] == IS) { //IS
			B[i] = N+1;
		}
		
		printf("  Station %d: [",i);
		switch(blockType[i]) {
			case IS: printf("BAS,0,IS,"); break;
			case BAS: printf("BAS,%d,%d,",B[i],m[i]); break;
			case RS_RD: printf("RS-RD,%d,%d,",B[i],m[i]); break;
		}
		printf("%lf]\n",Ets[i]);
	}
	
	printf("\n---------------NUMBER OF JOB IN THE NET---------------\n %d\n",N);

	double ps_current = 0;
    i = 0;

    // INDICES
    // mean queue length
    double E_n[M];
	// classical utilization
    double U[M];
    // effective utilization
    double Ue[M];
	// classical throughput
    double X[M];
    // effective throughput
    double Xe[M];
    // mean response (residence) time
    double E_t[M];
	// blocking probability
	double bp[M];

    // indices initialization
    for(i = 0; i < M; i++) {
        E_n[i] = 0.0;
		U[i] = 0.0;
        Ue[i] = 0.0;
        E_t[i] = 0.0;
		bp[i] = 0.0;
    }
	i=0;
	int d, c, k;

	// read state configuration (queue length and blocking status for each station) from file
	for(d=0,j=0; j<M-1; ++j) {
		d += fscanf(fd2,"(%u,%u) ",(unsigned int*)&(n[j]),(unsigned int*)&(bs[j]));
	}
	d += fscanf(fd2,"(%u,%u) \n",(unsigned int*)&(n[j]),(unsigned int*)&(bs[j]));

	// while can read state configuration for each station
    while(d == 2*M)
	{
		// read state probability from file
        if(fscanf(fd3,"%le\n",&ps_current) < 1) {
			printf("[indices] Not enough probability values in the given file.\n");
			exit(1);
		}

		// for each station c
		for(c=0; c<M; ++c) {

			// if there are more than 0 jobs in the station
			if(n[c] > 0) {

				// compute partial mean queue length value
				E_n[c] += (double)n[c] * ps_current;

				// compute partial utilization and blocking probability for IS stations
				if(blockType[c] == IS) { //IS
					for(k=0; k<M; ++k) {
						U[c] += ps_current * n[c];
						if(P[c][k]) {
							if(n[k] < B[k]) {
								Ue[c] += ps_current * P[c][k] * n[c];
							}
							else {
								bp[c] += ps_current * P[c][k];
							}
						}
					}
				}

				// compute partial utilization and blocking probability for BAS stations
				if(blockType[c] == BAS) { //BAS
					//classical utilization
					active_servers = (n[c] <= m[c]) ? n[c] : m[c];
					U[c] += ps_current * ((double)active_servers/(double)m[c]);
					for(k=0; k<M; ++k) {
						if(P[c][k]) {
							//effective utilization
							active_servers = (n[c] <= m[c]) ? n[c]-bs[c] : m[c]-bs[c];
							Ue[c] += ((double)active_servers/(double)m[c]) * ps_current * P[c][k];
							bp[c] += bs[c] * ps_current * P[c][k];
						}
					}

				}

				// compute partial utilization and blocking probability for RS-RD stations
				if(blockType[c] == RS_RD) { //RS-RD
					//classical utilization
					active_servers = (n[c] <= m[c]) ? n[c] : m[c];
					U[c] += ps_current * ((double)active_servers/(double)m[c]);
					for(k=0; k<M; ++k) {
						if(P[c][k]) {
							if(k==c || n[k]<B[k]) {
								//effective utilization
								Ue[c] += ((double)active_servers/(double)m[c]) * ps_current * P[c][k];
							}
							else {
								bp[c] += ps_current * P[c][k];
							}
						}
					}

				}
			}

		}

		++i;

		// read state configuration (queue length and blocking status for each station) from file
        for(d=0,j=0; j<M-1; ++j) {
			d += fscanf(fd2,"(%u,%u) ",(unsigned int*)&(n[j]),(unsigned int*)&(bs[j]));
		}
		d += fscanf(fd2,"(%u,%u) \n",(unsigned int*)&(n[j]),(unsigned int*)&(bs[j]));

    
	}

    fclose(fd2);
	fclose(fd3);

	// for each station c: compute other performance metrics
	for(c=0; c<M; ++c) {

		if(blockType[c]==IS) {
			Xe[c] = Ue[c] * mu[c];
			X[c] = U[c] * mu[c];
			Ue[c] /= N;
			U[c] /= N;
		}
		else {
			Xe[c] = Ue[c] * m[c] * mu[c];
			X[c] = U[c] * m[c] * mu[c];
		}
	}

	for(i=0; i < M; i++) {
		// mean response (residence) time
		E_t[i] = E_n[i] / Xe[i];
	}
	
	printf("\n---------------PERFORMANCE METRICS---------------\n");
	printf("Mean Residence Time\n");
	for(j=0; j<M; ++j) {
		printf("  E[t_%d] = %lf\n",j,E_t[j]);
		// copy index to file
		fprintf(fd_t[j],"%d %lf\n",N,E_t[j]);
	}
	printf("\nMean Queue Length\n");
	for(j=0; j<M; ++j) {
		printf("  E[n_%d] = %lf\n",j,E_n[j]);
		// copy index to file
		fprintf(fd_n[j],"%d %lf\n",N,E_n[j]);
	}
	printf("\nThroughput (classic and effective)\n");
	for(j=0; j<M; ++j) {
		printf("  X_%d = %lf      Xeff_%d = %lf\n",j,X[j],j,Xe[j]);
		// copy index to file
		fprintf(fd_X[j],"%d %lf\n",N,Xe[j]);
	}
	printf("\nUtilization (classic and effective)\n");
	for(j=0; j<M; ++j) {
		printf("  U_%d = %lf      Ueff_%d = %lf\n",j,U[j],j,Ue[j]);
		// copy index to file
		fprintf(fd_U[j],"%d %lf\n",N,Ue[j]);
	}
	printf("\nBlocking probability\n");
	for(j=0; j<M; ++j) {
		printf("  bp_%d = %lf\n",j,bp[j]);
		// copy index to file
		fprintf(fd_pb[j],"%d %lf\n",N,bp[j]);
	}
	
	// close files
	for(j=0; j<M; ++j) {
		fclose(fd_n[j]);
		fclose(fd_t[j]);
		fclose(fd_X[j]);
		fclose(fd_U[j]);
		fclose(fd_pb[j]);
	}
	
	// free strings
	free(fn_n);
	free(fn_t); 
	free(fn_X); 
	free(fn_U);
	free(fn_pb);
	
	printf("\n\n\n(state space, transition frequency, state probabilities and indices (through. and util. effective) are available in TEMP directory)\n\n");

	return 0;
} 
