/*

 Editor: Thanakit Yuenyongphisit
 
 Source original: Kasidit Chanchio (kasiditchanchio@gmail.com)
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

//Circular linked list
struct hnode {
	char* line;
	char* id;
	struct hnode *next;
	
};
struct hnode *init_cir_list(){
	//Declare head
	struct hnode * head = (struct hnode *) malloc(sizeof(struct hnode));
	head->line = NULL;
	head->id   = NULL;
	head->next = NULL;
	
	struct hnode * cur = head;
	for(int i = 0 ; i < 9 ; i++){
		cur->next = (struct hnode *) malloc(sizeof(struct hnode));
		cur = cur->next;
		cur->line = NULL;
		cur->id   = NULL;
		cur->next = NULL;
	
	}
	//Link last to head
	cur->next = head;
	
	return head;

}
struct hnode* add_data(struct hnode * cur, char* line, char* id){

	//free prev data
	free(cur->line);
	free(cur->id);
	//Allocate new space
	cur->line = (char *) malloc(MAXLINE * sizeof(char));
	cur->id   = (char *) malloc(MAXLINE * sizeof(char));
	//Copy value
	strcpy(cur->line,line);
	strcpy(cur->id,id);
	
	
	
	return cur->next;
}
void printList(struct hnode *cur){
	int m = 10;
	for(int i =0 ; i < 10; i++){
		if(cur->id != NULL && cur->line != NULL)
			printf("id = %s : m = %s\n",cur->id,cur->line);
		fflush(stdout);
	
		cur = cur->next;	
	}


}




//Struct data
struct clientData{
	int fd;
	char id[MAXLINE];

};


int lis_fd;
int conn_fd;
struct sockaddr_in serv_addr;


#define MAXCONN 100

void cut_newline(char* s){
	for(int i = 0 ; i < strlen(s); i++){
		if(s[i] == '\n')
			s[i] = '\0';
	
	}
}


int main(int argc, char *argv[]){


	//Part 2
	struct hnode *head = init_cir_list();
	//=======

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
	    	//printf("i= %d\n",i);
	    	
	    	//When the signal trigger on lis_fd
	    	//Incoming connection
		if(i == lis_fd){
		  //Error case
		  if((conn_fd[cindex] = accept(lis_fd, NULL, NULL)) < 0){
		    printf("Accept: Error occured\n");
		    exit(1);
		  }
		  //Connect accepted
	          //printf("a new connection %d is made!\n", conn_fd[cindex]);
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
		  cut_newline(line);
		  strcpy(clientList[cindex].id, line);
		  clientList[cindex].fd = conn_fd[cindex];
		  printf(" Cli-%s Connected\n",line);
		  
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
                    //printf("line = %s with n = %d characters\n", line, n);
                    fflush(stdout);
                    
                  //Find sender
                  char sendbuffer[MAXLINE*3];
		 	//Find sender
		 	int k;
		 	for (k = 0 ; k < cindex; k++)
		 		if (conn_fd[k] == i)
		 			break;
		 //Prepare message to send
		 	cut_newline(line);
		 	sprintf(sendbuffer, "cli-%s says: %s\n", clientList[k].id, line);
		 //Save value to linkedlist
		 add_data(head, line, clientList[k].id);
                  
                  
		  //Broad cast the message to other client
	 	    for(j =0; j < cindex; j++){
	 	    
		      if(conn_fd[j] != -1 && conn_fd[j] != i){//Fixed to send to all but sender
		      
		      
		      /*
		      char sendbuffer[MAXLINE*3];
		 	//Find sender
		 	int k;
		 	for (k = 0 ; k < cindex; k++)
		 		if (conn_fd[k] == i)
		 			break;
		 			
		 	
		 	
		 	
		 	//Prepare message to send
		 	cut_newline(line);
		 	sprintf(sendbuffer, "cli-%s says: %s\n", clientList[k].id, line);
		 	*/
		 	//Write to client	
		 	m = write(conn_fd[j], sendbuffer, n);

                        printf("From: %s Message: %s To %s\n",clientList[k].id, line, clientList[j].id);
                        
                        fflush(stdout);
                       // printList(head);
                        
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
