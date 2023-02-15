#include <stdio.h>
#include <stdlib.h>
#define N 400

double A[N][N];
double B[N][N];
double C[N][N];
double D[N][N];

//Prototype
void plus(double x[N][N], double y[N][N], double z[N][N]);
void dotProduct(double x[N][N], double y[N][N], double z[N][N]);
void copyArray(double first[N][N], double second[N][N]);

void setValue(double x[N][N], double value){
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			x[i][j] = value;
	
}

int main(double argc, char *argv[]){

	for(int i = 0; i < N ; i++)
		for(int j = 0; j < N ; j++)
			A[i][j] = 4;
			
	setValue(A,4);
	setValue(B,3);
	setValue(C,8);
	setValue(D,0);
	
	
	dotProduct(A,B,D);
	plus(D,C,D);
	
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if(D[i][j] != 4808){
				printf("ERROR\n");
				return -1;
			}
	
	printf("OK\n");		
	
	
	return 0;
}

void plus(double first[N][N], double second[N][N], double z[N][N]){
	//Copy value
	double x[N][N];
	double y[N][N];
	
	copyArray(x,first);
	copyArray(y,second);

	for(int i = 0 ; i < N ; i++)
		for(int j = 0 ; j < N ; j++)
			z[i][j] = x[i][j] + y[i][j];
	
	
	
}
void copyArray(double first[N][N], double second[N][N]){
	for(int i = 0 ; i < N ; i++)
		for(int j = 0 ; j < N ; j++)
			first[i][j] = second[i][j];


}

void dotProduct(double first[N][N], double second[N][N], double z[N][N]){
	//Copy value
	double x[N][N];
	double y[N][N];
	
	copyArray(x,first);
	copyArray(y,second);
	
	for(int i = 0 ; i < N ; i++)
		for(int j = 0 ; j < N ; j++)
			for(int each = 0; each < N; each++){
				// Loop for sum each row and each col
				z[i][j] += x[i][each] * y[each][j];
			}
				
				
		
		
		
}




