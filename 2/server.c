/*
 A simple echo server program 
 Kasidit Chanchio (kasiditchanchio@gmail.com)
*/
#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_IP		"127.0.0.1"
#define SERV_PORT 	18800
#define MAXLINE	100


int lis_fd;
int conn_fd;
struct sockaddr_in serv_addr;

//Server2
#define SERV_PORT2 	18801
int conn_fd2;
struct sockaddr_in serv_addr2;


int main(int argc, char *argv[]){

        int m, n;
        char line[MAXLINE];

	lis_fd = socket(AF_INET, SOCK_STREAM, 0); 
	//Clear mem
	memset(&serv_addr, 0, sizeof(serv_addr));
	//Set value
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	//Bind port to being a server
	bind(lis_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)); 
	//Wait for connection
	listen(lis_fd, 5);

	if((conn_fd = accept(lis_fd, NULL, NULL)) < 0){
		printf("Accept: Error occured\n");
		exit(1);
	}

	//Connect to Server2
		conn_fd2 = socket(AF_INET, SOCK_STREAM, 0); 

		memset(&serv_addr2, 0, sizeof(serv_addr2));
		//Config value for server 2 client connection
		serv_addr2.sin_family = AF_INET;
		serv_addr2.sin_port = htons(SERV_PORT2);
		serv_addr2.sin_addr.s_addr = inet_addr(SERV_IP);

			if (connect(conn_fd2, (struct sockaddr *) &serv_addr2, sizeof(serv_addr2))<0) { 
				perror("Problem in connecting to the server");
				exit(3);
			}



	while ((n = read(conn_fd, line, MAXLINE)) != 0){
				//Get from client
                //printf("line = %s with n = %d characters\n", line, n);
				printf("Receive from Client: %s\n", line);
                fflush(stdout);
                

				//Send data to Server2
				n = write(conn_fd2, line, MAXLINE);
				//printf("send %s with n = %d characters\n", line, n);

				//Read from server2
				m = read(conn_fd2, line, MAXLINE);
				//printf("receive %s with m = %d characters\n", line, m);
				printf("Receive from Server2: %s\n", line);

				fputs(line, stdout);

				//Return to server
				m = write(conn_fd, line, n);
                printf("write line = %s for m = %d characters\n", line, m);
                fflush(stdout);


        }

	close(conn_fd);
	close(conn_fd2);
	close(lis_fd);

}
