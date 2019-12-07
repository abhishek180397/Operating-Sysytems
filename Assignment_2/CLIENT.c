#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<netdb.h>

#define MAXSIZE 100


//method to send email and receive public key

void clientside(int sockfd) 
{ 
	char buff[MAXSIZE]; 
	char output[MAXSIZE];

	//setting buffer with zero's
	bzero(buff, sizeof(buff)); 
	printf("Enter the email address of a user : \n"); 
	int n = 0; 
    char c;
	while((c = getchar()) != '\n'){
		buff[n] = c;
		n++;
		}
		
	//sending email address to the server
	write(sockfd, buff, sizeof(buff)); 
		
	//readind the public key from the server
	read(sockfd, output, sizeof(output));

	if(strcmp(buff,output) == 0)
		printf("Key for %s not found in database",buff);
	else
		printf("The public key for %s :\n",buff);
		printf("%s",output);
		printf("\n");
		
}


int main(){

	int csd;
	struct sockaddr_in serverAddr;
    struct hostent *server;


	//socket creation and base cases
	csd = socket(AF_INET, SOCK_STREAM, 0);
	if(csd < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	//printf("Client Socket is created.\n");

	//Server Name from user input
    char servername[MAXSIZE];
	printf("Enter server name :\n");
    fgets(servername,80,stdin);

    servername[strlen(servername) -1] = '\0';
    server = gethostbyname(servername);
   
   	if (server == NULL) {
      	fprintf(stderr,"ERROR, no such host\n");
      	exit(0);
   	} 

   //set the block of servAddr to zero
	memset(&serverAddr, '\0', sizeof(serverAddr));

	//Port number from  user input
	char portnumber[20];
	int portno;
	printf("Enter port number: \n");
    fgets(portnumber,20,stdin);
    portno = atoi(portnumber);

	
	// assign IP, PORT 
	serverAddr.sin_family = AF_INET; 
	memcpy((char *)server->h_addr, (char *)&serverAddr.sin_addr.s_addr, server->h_length);
	serverAddr.sin_port = htons(portno); 

	//connect function to establish a connection with server
	if(connect(csd, (struct sockaddr*)&serverAddr, sizeof(serverAddr))<0){
		printf("Error in connection.\n");
		exit(1);
		}
	printf("Connected to Server.\n");

	// An infinte loop of clinet requests to the server
	while(1){
		clientside(csd);
	}

	close(csd);
}