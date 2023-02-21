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
		
		cur = (struct clientData *) malloc(sizeof(struct clientData));
		cur->id = (char *) malloc(10 * sizeof(char));
		head = cur;
	}
		
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
		cur->id = (char *) malloc(10 * sizeof(char));

	}
	cur->fd = fd;
	strcpy(cur->id,id);
	cur->next = NULL;
	return fd;
}




int removeClient(int fd){
	//Loop search id
	struct clientData* cur = head;
	//Empty case prevent
	if(head == NULL)
		return -1;
	
	
	//Head remove
	if(cur->fd == fd && cur == head){
		head = head->next;
		
		free(cur->id);
		free(cur);
		return 1;
	}
	
	//Other remove
	struct clientData* prev = cur;
	cur = cur->next;
	for(cur = head ; cur != NULL; cur = cur->next){
		if(cur->fd == fd){
			prev->next = cur->next;
			free(cur->id);
			free(cur);
			return 1;
		}
	
		prev = cur;
		
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
	
	//Program loop
	int j = 0;
	while(1){
		j++;
		printf("%d\n",j);fflush(stdout);
		//Copy base to temporary Set of FD
		memcpy(&rfds, &base_rfds, sizeof(fd_set));
	
		//Listening to signal from active file descriptor
		if(select(fdmax+1, &rfds, NULL, NULL, NULL) < 0){
			printf("Error occured on select()\n");
			fflush(stdout);
			exit(1);//Exit with abnormal
		}
		
		//New connection to listening FD
		if(FD_ISSET(lis_fd, &rfds)){
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
			int n = read(conn_fd, line, 6);
			if (n <= 0){
			    if(n == 0){
				printf("read: close connection %d\n", conn_fd);
				FD_CLR(conn_fd, &base_rfds);
				close(conn_fd);
			    }
			    else{
				printf("read: Error occured\n");
				exit(1);
			    }
			    printf("Error on accepting connect\n");
			    fflush(stdout);
			    
			  }
			  
			//Established
			//Store client data
			addClient(conn_fd,line);
								
			printf("Cli-%s Connected\n",line);
			fflush(stdout);
			
			
			if(conn_fd > fdmax)
		   		fdmax = conn_fd;
		  	continue;
			
		}
		
		//loop check client fd
		for(struct clientData* cur = head; cur != NULL; cur = cur->next){
			
			//Client package came
			if(cur->fd > 0 && FD_ISSET(cur->fd, &rfds)){
				int n = read(cur->fd, line, MAXLINE);
				//Close or error case
				if (n <= 0){
				    if(n == 0){
					printf("read: close connection %d\n", cur->fd);
					FD_CLR(cur->fd, &base_rfds);
					close(cur->fd);
				    }
				    else{
					printf("read: Error occured\n");
					exit(1);
				    }
				    cur->fd = -1;
				    
				    continue;//Skip
				  }
				  
				//Normal case
				//Set string format
				char sendbuffer[MAXLINE*2];
				sprintf(sendbuffer, "cli-%s says: %s", cur->id, line);
				printf("From: %s msg: %s",cur->id,line);
				fflush(stdout);
				
				//Broad case to other
				for(struct clientData* toSend = head; toSend != NULL; toSend = toSend->next){
					//Skip owner
					if(toSend == cur)
						continue;
				
					//Send
					write(toSend->fd, sendbuffer, n);
					printf(" To %s",toSend->id);
					fflush(stdout);
				
				}
			
			
			}
		
		
		
		
		}
		
		
		
	
	
	
	
	
	
	
	}
	
	
	
	





close(lis_fd);
}

