// append exact result from 1 job to N in an output file

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

	int M,tot_job,j;
	
	// read number of station from file
	FILE *fd_stations;
	fd_stations = fopen("../TEMP/station_num.txt","r");
	fscanf(fd_stations,"%d",&M);
	fclose(fd_stations);
	
	// number_length is string length of file output name
	int number_length = 1;
	int div = 10;
	while(M/div > 0) {
		++number_length;
		div *= 10;
	}

	// indices variable
	double T[M];
	double N[M];
	double Ue[M];
	double Xe[M];
	double pb[M];
	
	// Input indices file
    FILE *fd_n[M];
	FILE *fd_t[M];
	FILE *fd_X[M];
	FILE *fd_U[M];
	FILE *fd_pb[M];
	
	// Output indices file
    FILE *fd_n_out[M];
	FILE *fd_t_out[M];
	FILE *fd_X_out[M];
	FILE *fd_U_out[M];
	FILE *fd_pb_out[M];
	
	// file input name
	char *fn_n = (char*)calloc(1+number_length+25+1,sizeof(char));
	char *fn_t = (char*)calloc(1+number_length+25+1,sizeof(char));
	char *fn_X = (char*)calloc(1+number_length+25+1,sizeof(char));
	char *fn_U = (char*)calloc(1+number_length+25+1,sizeof(char));
	char *fn_pb = (char*)calloc(1+number_length+25+1,sizeof(char));
	
	// file output name
	char *fn_n_out = (char*)calloc(1+number_length+25+1,sizeof(char));
	char *fn_t_out = (char*)calloc(1+number_length+25+1,sizeof(char));
	char *fn_X_out = (char*)calloc(1+number_length+25+1,sizeof(char));
	char *fn_U_out = (char*)calloc(1+number_length+25+1,sizeof(char));
	char *fn_pb_out = (char*)calloc(1+number_length+25+1,sizeof(char));
	
	// open file input in TEMP directory
	for(j=0; j<M; ++j) {
		sprintf(fn_n,"../TEMP/n%d_EXACT.dat",j);
		fd_n[j] = fopen(fn_n,"r");
		sprintf(fn_t,"../TEMP/t%d_EXACT.dat",j);
		fd_t[j] = fopen(fn_t,"r");
		sprintf(fn_X,"../TEMP/X%d_EXACT.dat",j);
		fd_X[j] = fopen(fn_X,"r");
		sprintf(fn_U,"../TEMP/U%d_EXACT.dat",j);
		fd_U[j] = fopen(fn_U,"r");	
		sprintf(fn_pb,"../TEMP/PB%d_EXACT.dat",j);
		fd_pb[j] = fopen(fn_pb,"r");	
	}
	
	// open file output in TEMP_JOBS directory
	for(j=0; j<M; ++j) {
		sprintf(fn_n_out,"n%d_EXACT.dat",j);
		fd_n_out[j] = fopen(fn_n_out,"a");
		sprintf(fn_t_out,"t%d_EXACT.dat",j);
		fd_t_out[j] = fopen(fn_t_out,"a");
		sprintf(fn_X_out,"X%d_EXACT.dat",j);
		fd_X_out[j] = fopen(fn_X_out,"a");
		sprintf(fn_U_out,"U%d_EXACT.dat",j);
		fd_U_out[j] = fopen(fn_U_out,"a");	
		sprintf(fn_pb_out,"PB%d_EXACT.dat",j);
		fd_pb_out[j] = fopen(fn_pb_out,"a");
	}


	// get indices from qnsolver output
	for(j=0;j<M;j++) {
		fscanf(fd_n[j],"%d %lf\n",&tot_job,&N[j]);
		fscanf(fd_t[j],"%d %lf\n",&tot_job,&T[j]);
		fscanf(fd_X[j],"%d %lf\n",&tot_job,&Xe[j]);
		fscanf(fd_U[j],"%d %lf\n",&tot_job,&Ue[j]);
		fscanf(fd_pb[j],"%d %lf\n",&tot_job,&pb[j]);
	}
	
	
	// write indices in TEMP_JOBS directory
	for(j=0;j<M;j++) {
		fprintf(fd_n_out[j],"%d %lf\n",tot_job,N[j]);
		fprintf(fd_t_out[j],"%d %lf\n",tot_job,T[j]);
		fprintf(fd_X_out[j],"%d %lf\n",tot_job,Xe[j]);
		fprintf(fd_U_out[j],"%d %lf\n",tot_job,Ue[j]);
		fprintf(fd_pb_out[j],"%d %lf\n",tot_job,pb[j]);
	}
	
	
	// close file and free variables
	for(j=0; j<M; ++j) {
		fclose(fd_n[j]);
		fclose(fd_t[j]);
		fclose(fd_X[j]);
		fclose(fd_U[j]);
		fclose(fd_pb[j]);
	}
	
	// close output file and free variables
	for(j=0; j<M; ++j) {
		fclose(fd_n_out[j]);
		fclose(fd_t_out[j]);
		fclose(fd_X_out[j]);
		fclose(fd_U_out[j]);
		fclose(fd_pb_out[j]);
	}
	
	// free strings
	free(fn_n_out);
	free(fn_t_out); 
	free(fn_X_out); 
	free(fn_U_out);
	free(fn_pb_out);
	
	// free strings
	free(fn_n);
	free(fn_t); 
	free(fn_X); 
	free(fn_U);
	free(fn_pb);
	
	
	
	
	
	return 0;
}