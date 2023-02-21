/*
 Author: Thanakit Yuenyongphisit
 Ref original (kasiditchanchio@gmail.com)
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

#include <time.h>
#define SERV_IP		"127.0.0.1"
#define SERV_PORT 	13000

#define	MAXLINE	80

int main(int argc, char *argv[]){

	//Set of FD
	fd_set base_rfds;
	fd_set rfds; 
	char line[MAXLINE];
	char ID[6];
	int client_shutdown_flag = 0;
	
	//Define Data structure and Socket
	int conn_fd = socket(AF_INET, SOCK_STREAM, 0); 
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	
	serv_addr.sin_addr.s_addr = inet_addr(SERV_IP);
	
	
	//Connect
        if (connect(conn_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0) { 
            perror("Problem in connecting to the server");
            exit(3);
        }
        
        FD_ZERO(&base_rfds);
	FD_ZERO(&rfds);
	//Signal from keyboard
	FD_SET(fileno(stdin), &base_rfds);
	FD_SET(conn_fd, &base_rfds);
	
	//Form ID
	srand(time(NULL));
	sprintf(ID, "%d%d%d%d",rand()%10,rand()%10,rand()%10,rand()%10);
	
	int n = write(conn_fd, ID, 6);
	
	while(1){
	  //Display owner client
	  fflush(stdout);
          printf("cli-%s:>",ID);
          fflush(stdout);
	
	
	  //Refresh FD_SET from base FD
	  memcpy(&rfds, &base_rfds, sizeof(fd_set));
	  
	  if (select(conn_fd+1, &rfds, NULL, NULL, NULL) == -1) {
            printf("Error in select\n");
            fflush(stdout);
            exit(1);
          }
          //Signal from keyboard
          if(FD_ISSET(fileno(stdin), &rfds)){
            if(fgets(line, MAXLINE, stdin) == NULL){
	      printf("Shutdown writing to TCP connection\n");
	      shutdown(conn_fd, SHUT_WR);
	      client_shutdown_flag = 1;
	    }
	    else{
              n = write(conn_fd, line, MAXLINE);
              
	    }
	  }
	  
	  if(FD_ISSET(conn_fd, &rfds)){
            if(read(conn_fd, line, MAXLINE) == 0){
	      if(client_shutdown_flag){
	        printf("TCP connection closed after client shutdown\n");
	        close(conn_fd);
	        break;
	      }
	      else{
	        printf("Error: TCP connection closed unexpectedly\n");
	        exit(1);
	      }
	    }
	    else{//Incoming message
	     fputs("\n",stdout);
	    
              //printf("receive %s with m = %d characters\n", line, m);
              fputs(line, stdout);
	    }
	  }
	  
	  
	
	
	
	}
	
	



}

