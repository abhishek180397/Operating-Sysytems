#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 100


// Method designed for email query  from the client 
void serverside(int ssd) 
{ 
	char buff[MAXSIZE]; 
	int n; 
	
	char search[MAXSIZE];
	bzero(buff, sizeof(buff)); 
	
	// read the message from client and copy it in buffer 
	read(ssd, buff, sizeof(buff)); 
	strncpy(search,buff,sizeof(search));

	// print buffer which contains the email address from client 
	printf("Received request for public key of : %s\t \n", buff);
	bzero(buff, sizeof(buff)); 
	FILE *file;
	file = fopen("keys20.txt", "r"); // open file
	if (file == NULL) {
    	printf("Failed to open file.");
		}
	else {
    char id[MAXSIZE];
	char pkey[MAXSIZE];
    char test[MAXSIZE];

    // traverse file for each record in file
    while(fscanf(file, "%s %s", id, pkey) != EOF)
		{
		strncpy(test,id,sizeof(test));
		// if id is equal to search than key is found return to main
		if (strcmp(test, search) == 0) 
		{
        printf("Key found! \n"); //true cause key is found
			
		// copy server message in the buffer 
		bzero(buff, MAXSIZE); 
		strncpy(buff,pkey,MAXSIZE);
		//Send the Public key to the client 
		write(ssd, buff, sizeof(buff)); 
		return;
		}
    	}

    	//Error meassage when key is not found
    	char errbuff[MAXSIZE]="was not found in database ";//false cause key not found
		write(ssd,errbuff,sizeof(errbuff));
    	}
	
}

int main(){

	int ssd;
	struct sockaddr_in servaddr;
	int newsd;
	struct sockaddr_in newaddr;
	socklen_t addr_size;
	pid_t childpid;

	//socket creation on server side and base case checkinh
	ssd = socket(AF_INET, SOCK_STREAM, 0);
	if(ssd < 0){
		printf("Error in connection.\n");
		exit(1);
		}
	printf("Server Socket is created.\n");

	//Setting the ervaddr block to zero
    memset(&servaddr, '\0', sizeof(servaddr));

	//getting portnumber from User
    int portno;
    char portnumber[20];
	printf("Enter portnumber :\n");
    fgets(portnumber,20,stdin);
    portno = atoi(portnumber);

	//Assigning portnumber and IP to the socket
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(portno);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 

	//binding the address to the socket
	if(bind(ssd, (struct sockaddr*)&servaddr, sizeof(servaddr))<0){
		printf("Error in binding.\n");
		exit(1);
		}
	printf("Binded to port\n");

	//Server listening for the client connections
	if(listen(ssd, 3) == 0){
		printf("server Listening....\n");}
	else{
		printf("Error in binding.\n");
		exit(0);
		}

	//Accepting the client connection
	while(1){
	newsd = accept(ssd, (struct sockaddr*)&newaddr, &addr_size);
	if(newsd < 0){
	exit(1);
	}
	//Printing the address and port of the client connected
	printf("Connection accepted from %s:%d\n", inet_ntoa(newaddr.sin_addr), ntohs(newaddr.sin_port));
	
	//forking to accpet Multiple Client Connections
	if((childpid = fork()) == 0){
		close(ssd);
		while(1){
			//Function call to get the primary key 
			serverside(newsd);
			}
        }
	}//end of while loop

	//closing the socket connection
	close(newsd);
	return 0;
}