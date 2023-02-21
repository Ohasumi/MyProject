/*

 Author: Thanakit Yuenyongphisit
 
 Ref: Kasidit Chanchio (kasiditchanchio@gmail.com)
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

//Select signal
#include <sys/select.h>

#define SERV_IP		"127.0.0.1"
#define SERV_PORT	13000
#define	MAXLINE		80
#define MAXCONN		80

/*
	To map client name to file descriptor
	Linklist
	Use to manipulate the list of connect client
*/

struct clientData{
	// If fd are minus mean not active
	int fd;
	char *id;
	struct clientData *next;

};
struct clientData *head;
int addClient( int fd, char* id){

	struct clientData* cur = head;
	
	//Init case
	if(cur == NULL){
		cur = (struct clientData *) malloc(sizeof(struct
		cur->id = (char *) malloc(10 * sizeof(char));
	}
		 clientData));
	//Have existing node
	else{
		struct clientData* tmp;
		//Loop find tail
		for(tmp = cur; tmp->next != NULL; tmp = tmp->next){
			//Check if client already have data
			if(strcmp(id,tmp->id)==0){
				//Client used to connect
				//Update only fd
				tmp->fd = fd;
				return fd;
			
			}
		}
		//Client doesn't exist add new
		tmp->next = (struct clientData *) malloc(sizeof(struct clientData));
		cur = tmp->next;

	}
	cur->fd = fd;
	strcpy(cur->id,id);
	cur->next = NULL;
	return fd;
}

int getClient(char*id){
	//Loop search id
	for(struct clientData* cur = head; cur->next != NULL; cur = cur->next){
		if(strcmp(id,cur->id) == 0)
			return cur->fd;
	
	}
	
	return -1;

}






int main(int argc, char *argv[]){
	
	
	char line[MAXLINE];
	
	
	
	//Set of fd to be listening the signal
	fd_set base_rfds;
	fd_set rfds;
	int fdmax; // point to last file descriptor
	
	//Define socket
	int lis_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	//Define Data structure for server socket
	struct sockaddr_in serv_addr;
	
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family	  = AF_INET;
	serv_addr.sin_port	  = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	//Assigned Data structure to socket
	bind(lis_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	
	//Set socket to passive listener
	listen(lis_fd, 8);
	
	//Clear mem of Set of FD
	FD_ZERO(&base_rfds);
	FD_ZERO(&rfds);
	
	//Add listending file descriptor to Set of FD
	FD_SET(lis_fd, &base_rfds);//Base FD
	
	fdmax = lis_fd;//point to the last avaliable FD
	
	while(1){
		//Copy base to temporary Set of FD
		memcpy(&rfds, &base_rfds, sizeof(fd_set));
	
		//Listening to signal from active file descriptor
		if(select(fdmax+1, &rfds, NULL, NULL, NULL) < 0){
			printf("Error occured on select()\n");
			fflush(stdout);
			exit(1);//Exit with abnormal
		}
		
		
		//Loop each file descriptor
		for(int i = 0; i <= fdmax; i++){
			
			//If no signal in i FD then skip
			if(!FD_ISSET(i, &rfds))
				continue;
			
			//Have signal case
			
			//New connection to listening FD
			if(lis_fd == i){
				//Create new conn_fd for connection
				int conn_fd = accept(lis_fd, NULL, NULL);

				if(conn_fd < 0){
					printf("Error in accept\n");
					exit(1);
				
				}
				//Normal case
				
				//Add conn_fd to Set of FD
				FD_SET(conn_fd, &base_rfds);
				
				//Get client ID (Size are 4)
				int num_read = read(conn_fd, line, 6);
				
								
			
			
			
			}
			
		
		
		
		}
	
	
	
	
	
	
	
	}
	
	
	
	






}

