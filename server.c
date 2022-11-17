#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/signal.h>
#include <sys/wait.h>

#define BUFLEN 100
#define peerLength 10
#define contentLength 10
#define maxContent 10

typedef struct PDU{
        char type;
        char data[BUFLEN];
}pud;

/*	reaper		*/
void	reaper(int sig)
{
	int	status;
	while(wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}

int main(int argc, char **argv){ 
    int downloadClient, client_len;     
    int i, j, k, s;
    int numberOfRegistedContent = 0;
    int TCP;
    int UDP;
    int alen;
    int IndexPort = 15000;
    int port;
    int numOfContent = 0;
    char input;
    char *charPort;
    struct sockaddr_in clientADDR;
    struct hostent *phe;
    
    struct sockaddr_in fsin;    //from address of a client
    struct sockaddr_in sin;     //endpoint address   
    char Addr[10];
    char Name[10];
    char contentList[contentLength][contentLength];
    char Content[10];
    char ErrorConflict[] = "Name conflict, please register with another name.";
    
    pud toClient;
    pud receivedData;
    port = atoi(argv[2]);
    peerName = argv[1];
    charPort= argv[2];
    bool registrationPassed;
    
    for(i = 0; i < 10;i++){
    	memset(contentList[i], '\0', sizeof(contentList[i]));
    }
    
    memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(port);
                                                                                                 
    /* Allocate a socket */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
		fprintf(stderr, "can't create socket\n");
                                                                                
    /* Bind the socket */
        if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		fprintf(stderr, "can't bind to %d port\n",port);
        listen(s, 5);	
  	alen = sizeof(fsin);
  	
  	while (1) {			
		//receive registration/content request/content de-registration/quit	
		 recvfrom(UDP, (struct PDU*) &receivedData, sizeof(receivedData) + 1, 0, (struct sockaddr*) &clientADDR, &alen);
		 printf("%s\n", receivedData.data);
		 
		 if(receivedData.type == 'R'){
		 
		    //Sort data into 10 bytes each
		    for(i = 0; i<30; i++){
		       if(i<10)
		        Name[i] = receivedData.data[i];  
		       else if(i<20)
		        Content[i] = receivedData.data[i];
		       else if(i<30)
		        Addr[i] = receivedData.data[i];
		    } 
		    
		    //check for already registered content
		    for(i = 0; i <= numberofRegisteredContent; i++){
		       if(strcmp(Name, contentList[i][1]) == 0){
		          if(strcmp(Content, contentList[i][2]) == 0){
		                toClient.type = 'E';
                    strcpy(toClient.data, ErrorConflict);
                    sendto(UDP, (struct PDU*) &toIClient, sizeof(toClient) + 1, 0, (struct sockaddr*) &clientADDR, alen); 
                    continue;
		          }
           } 
           else{
                    //Register content
                    for (i = 0; i<3; i++){
                      if(i == 1)
                        contentList[numberofRegisteredContent][i] = Name;
                      if(i == 2)
                        contentList[numberofRegisteredContent][i] = Content;
                      if(i==3)
                        contentList[numberofRegisteredContent][i] = Addr;
                      
                    }
                      
                    numberofRegiteredContent++;
                      
                    toClient.type = 'A';
                    strcat(toClient.data, "Success");
                    sendto(UDP, (struct PDU*) &toClient, sizeof(toClient) + 1, 0, (struct sockaddr*) &clientADDR, alen); 
                    continue;
           }
           
		    }
		 } 
		 
		 else if(receivedData.type == 'S'){
		    //sort data into 10 bytes each
		    for(i = 0; i<20; i++){
		       if(i<10)
		        Name[i] = receivedData.data[i];  
		       else if(i<20)
		        Content[i] = receivedData.data[i];
		    }
		  
		    for(i=0;i<numberofRegisteredContent;i++){
		      if(strcmp(Name, contentList[i][1]) == 0){
		          if(strcmp(Content, contentList[i][2]) == 0){
		                toClient.type = 'S';
		                strcpy(toClient.data, contentList[i][3]); //1 = Name, 2 = Content, 3 = Address - alredy registered with ' ' from client side
                    sendto(UDP, (struct PDU*) &toIClient, sizeof(toClient) + 1, 0, (struct sockaddr*) &clientADDR, alen); 
                    i = numberofRegisteredContent; //break out of for loop -> client only needs single peer to download from    
		          }
		      } 
		      //reached end of registered content array and no match
		      else if (i == numberofRegisteredContent-1){
		         toClient.type = 'E';
             strcat(toClient.data, "Please verify content request, deos not exist.");
             sendto(UDP, (struct PDU*) &toIClient, sizeof(toClient) + 1, 0, (struct sockaddr*) &clientADDR, alen); 
             continue;
		      }
		    }
		 } 
		 
		 //De-register Content
		 else if(receivedData.type == 'T'){
		   for(i=0;i<numberofRegisteredContent;i++){
		     if(strcmp(Name, contentList[i][1]) == 0){
		          if(strcmp(Content, contentList[i][2]) == 0){
		          
		            //adapted from client code
		            if(i == numberofRegisteredContent - 1){
                        memset(contentList[i], '\0', sizeof(contentList[i])); //check if deletion/initialization works with 1-D reference
                    }
                    else{
                        for(i; i < numberOfRegistedContent; i++){
                            strcpy(contentList[i], contentList[i + 1]);
                        }
                        memset(contentList[numberOfRegistedContent], '\0', sizeof(contentList[numberOfRegistedContent]));
                    }
                    numberOfRegistedContent--;
                    printf("%s\n", receivedData.data);
                }
                
                toClient.type = 'A';
                strcat(toClient.data, "Success");
                sendto(UDP, (struct PDU*) &toClient, sizeof(toClient) + 1, 0, (struct sockaddr*) &clientADDR, alen); 
                continue;
		          }
		     }
		     //reached end of registered content array and no match
		      else if (i == numberofRegisteredContent-1){
		         toClient.type = 'E';
             strcat(toClient.data, "Please verify content request, deos not exist.");
             sendto(UDP, (struct PDU*) &toIClient, sizeof(toClient) + 1, 0, (struct sockaddr*) &clientADDR, alen); 
             continue;
		      }
		   }
		 } 
		 
		 //else if(receivedData.type == 'Q'){ 
		      //Client Q sends type t multiple times. 
		 //}
		 
		 //List all registered content
		 else if(receivedData.type == 'O'){ 
		   for(i=0;i<numberofRegisteredContent;i++){
		   
		     if(i < numberofRegisteredContent-1)
		      toClient.type = 'O';
		     else
		      toClient.type = 'F';
		     
		     strcpy(toClient.data, contentList[i][2]);//copy content name to data field
		     sendto(UDP, (struct PDU*) &toIClient, sizeof(toClient) + 1, 0, (struct sockaddr*) &clientADDR, alen);
		   }
		 }
		  
		 else
		  printf(stderr, "Code Debugging Needed");
	}  
}
