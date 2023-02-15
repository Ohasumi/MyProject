#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // Time library

#define N 400

double A[N][N];
double B[N][N];
double C[N][N];
double D[N][N];

//Struct to pass as parameter
struct thread_data{
	double (*first)[N];
	double (*second)[N];
	double (*result)[N];
	int start;
	int size;

};

//Prototype

//Thread
	void *operationThread(void *args);
	void *plusThread(void *args);
	void *dotProductThread(void *args);
	void setData(struct thread_data *data, double x[][N], double y[][N],double z[][N], int start, int size);
	
//Function
	void plus(double first[N][N], double second[N][N], double z[N][N], int start, int size);
	void dotProduct(double first[N][N], double second[N][N], double z[N][N], int start, int size);
	void copyArray(double first[N][N], double second[N][N]);
	void setValue(double x[N][N], double value);

int main(double argc, char *argv[]){		
	setValue(A,4);
	setValue(B,3);
	setValue(C,8);
	setValue(D,0);
	
	//Multithread
	pthread_t T1, T2, T3, T4;
	struct thread_data data1;
	struct thread_data data2;
	struct thread_data data3;
	struct thread_data data4;
	
	setData(&data1, A, B, D,   0, 100);
	setData(&data2, A, B, D, 100, 100);
	setData(&data3, A, B, D, 200, 100);
	setData(&data4, A, B, D, 300, 100);
	
	pthread_create(&T1, NULL, operationThread, (void *) &data1);
	
	pthread_create(&T2, NULL, operationThread, (void *) &data2);
	
	pthread_create(&T3, NULL, operationThread, (void *) &data3);
	
	pthread_create(&T4, NULL, operationThread, (void *) &data4);
	
	pthread_join(T1, NULL);
	pthread_join(T2, NULL);
	pthread_join(T3, NULL);
	pthread_join(T4, NULL);
	
	
	
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if(D[i][j] != 4808){
				printf("ERROR\n");
				return -1;
			}
	
	printf("OK\n");	
	
	return 0;
}
//To set data on thread_data struct
//Thread_data using for input multiple values to thread
void setData(struct thread_data *data, double x[][N], double y[][N],double z[][N], int start, int size){
	
	data->first  = x;
	data->second = y;
	data->result = z;
	data->start  = start;
	data->size   = size;
}
void *operationThread(void *args){
	struct thread_data *data;
	data = (struct thread_data *) args;
	
	dotProduct(A,B,D,data->start,data->size);
	plus(D,C,D,data->start,data->size);
	
}
void *plusThread(void *args){
	struct thread_data *data;
	data = (struct thread_data *) args;
	plus(data->first,data->second,data->result,data->start,data->size);
	
}
void *dotProductThread(void *args){
	struct thread_data *data;
	data = (struct thread_data *) args;
	
	dotProduct(data->first,data->second,data->result,data->start,data->size);
	
}




void setValue(double x[N][N], double value){
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			x[i][j] = value;
	
}

void plus(double first[N][N], double second[N][N], double z[N][N], int start, int size){
	//Copy value
	double x[N][N];
	double y[N][N];
	
	copyArray(x,first);
	copyArray(y,second);

	for(int i = start ; i < size+start ; i++)
		for(int j = 0 ; j < N ; j++)
			z[i][j] = x[i][j] + y[i][j];
	
	
	
}
void copyArray(double first[N][N], double second[N][N]){
	for(int i = 0 ; i < N ; i++)
		for(int j = 0 ; j < N ; j++)
			first[i][j] = second[i][j];


}

void dotProduct(double first[N][N], double second[N][N], double z[N][N], int start, int size){
	//Copy value
	double x[N][N];
	double y[N][N];
	
	copyArray(x,first);
	copyArray(y,second);
	
	for(int i = start ; i < size+start ; i++)
		for(int j = 0 ; j < N ; j++)
			for(int each = 0; each < N; each++){
				// Loop for sum each row and each col
				z[i][j] += x[i][each] * y[each][j];
			}
				
				
		
		
		
}




