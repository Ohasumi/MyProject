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

#include <sys/select.h>

//Struct data
struct node { // Coming soon part 2
	char name[MAXLINE];
	char text[MAXLINE];
	
	struct node *next;
	
};

struct clientData{
	int fd;
	char id[MAXLINE];

};


int lis_fd;
int conn_fd;
struct sockaddr_in serv_addr;


#define MAXCONN 100

int main(int argc, char *argv[]){

        int m, n, i, j;
        char line[MAXLINE];
	int conn_fd[MAXCONN]; 
	//Struct array the use conn_fd to determine
	struct clientData clientList[MAXCONN];
	
	
	int cindex = 0; 

	fd_set base_rfds; // base read fd set
	fd_set rfds; // read fd set to be passed as a parameter of select() 
	int fdmax;
	//Define listening file descriptor
	lis_fd = socket(AF_INET, SOCK_STREAM, 0); 
	
	//Flush data values in serv_addr
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	//Set address to that listener filedescriptor
	bind(lis_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)); 
	
	//Set to passive listener
	listen(lis_fd, 5);
	
	//Clear File descriptor set for using
	FD_ZERO(&base_rfds);
	FD_ZERO(&rfds);

	//Set listening fd to to base_rfds
	FD_SET(lis_fd, &base_rfds);
	//The lasted file_descriptor
	fdmax = lis_fd;
	
	//Loop for each signal
	for(;;){
	  memcpy(&rfds, &base_rfds, sizeof(fd_set));
	  //If any error occured
	  if(select(fdmax+1, &rfds, NULL, NULL, NULL) < 0){
	    printf("select error!\n");
	    fflush(stdout);
	    exit(1);
	  }
	  
	  
	  //Loop each file_descriptor
	  for(i = 0; i <= fdmax; i++){
	    
	    //Check if the signal in that specific occured
	    if(FD_ISSET(i, &rfds)){
	    	printf("i= %d\n",i);
	    	
	    	//When the signal trigger on lis_fd
	    	//Incoming connection
		if(i == lis_fd){
		  //Error case
		  if((conn_fd[cindex] = accept(lis_fd, NULL, NULL)) < 0){
		    printf("Accept: Error occured\n");
		    exit(1);
		  }
		  //Connect accepted
	          printf("a new connection %d is made!\n", conn_fd[cindex]);
	   	//____________________________________________________________
	   	//Set connect behavior for get client id or check existed id
	          
	          
		  FD_SET(conn_fd[cindex] , &base_rfds);
		  
		  //Read ID 
		  n = read(conn_fd[cindex], line, MAXLINE);
		  //printf("c=%d\n",cindex);
		  //Error handle
		  if (n <= 0){
		    if(n == 0){
		        printf("read: close connection %d\n", i);
			FD_CLR(i, &base_rfds);
			close(i);
			
			for(j =0; j <= cindex; j++){
				if (conn_fd[j] == i){
					conn_fd[j] = -1; 
				}
			}
		    }
		    else{
		        printf("read: Error occured\n");
		        exit(1);
		    }
		    printf("Error occured\n");
		    continue;//Skip
		  }
		  
		  //Insert Client data to struct array
		  strcpy(clientList[cindex].id, line);
		  clientList[cindex].fd = conn_fd[cindex];
		  
		  
		  if(conn_fd[cindex] > fdmax){
		    fdmax = conn_fd[cindex];
		  }

		  cindex++; //count of client
		  
		  
		//____________________________________________________________	  
  
		}
		
		//Signal from other connection
		else{
		  //Read string from client
		  //i determine the client fd number
		  n = read(i, line, MAXLINE);
		  if (n <= 0){
		    if(n == 0){
		        printf("read: close connection %d\n", i);
			FD_CLR(i, &base_rfds);
			close(i);
			
			for(j =0; j <= cindex; j++){
				if (conn_fd[j] == i){
					conn_fd[j] = -1; 
				}
			}
		    }
		    else{
		        printf("read: Error occured\n");
		        exit(1);
		    }
		  }
		  //Success Reading
		  else{
                    printf("line = %s with n = %d characters\n", line, n);
                    fflush(stdout);
                    
//_______________Broad cast the message to other client
		//Fixed to send to all but sender
	 	    for(j =0; j < cindex; j++){
	 	    
		      if(conn_fd[j] != -1 && conn_fd[j] != i){
		 	//Find sender
		 	int k;
		 	for (k = 0 ; k < cindex; k++)
		 		if (conn_fd[k] == i)
		 			break;
		 			
		 	m = write(conn_fd[j], clientList[k].id, n);
		 		
		 			
		      		
                        //m = write(conn_fd[j], line, n);
                        printf("From\n %swith %sto %s\n",clientList[k].id, line, clientList[j].id);
                        fflush(stdout);
                        
                 //---------------------------------------
		      }
		    }
        	  }
		}
	    }
	  }
	}
	close(lis_fd);

}
