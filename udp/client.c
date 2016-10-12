#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>

#define MAXBUFSIZE 100

/* You will have to modify the program below */

int main (int argc, char * argv[])
{
	
	int bytesRead;
	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];
	unsigned int remote_length;
	struct sockaddr_in remote;              //"Internet socket address structure"

	if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}

	/******************
	  Here we populate a sockaddr_in struct with
	  information regarding where we'd like to send our packet 
	  i.e the Server.
	 ******************/
	bzero(&remote,sizeof(remote));               //zero the struct
	remote.sin_family = AF_INET;                 //address family
	remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
	remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address
	
	
	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) //this checks completion of socket since error returns -1
	{
		printf("Unable to create socket.");
	}
	
	remote_length = sizeof(remote);
	char command[MAXBUFSIZE];
	char fileName[MAXBUFSIZE];
	char msg[MAXBUFSIZE];
	/******************
	  sendto() sends immediately.  
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
	
	while(1) {
	bzero(command,sizeof(command));
	printf("Choose command(get [FILE_NAME],put [FILE_NAME],ls,exit): \n"); //asks for command and stores it in 'command'
	fgets(command, MAXBUFSIZE, stdin);
	int len;
	len = strlen(command);	
	nbytes = sendto(sock, command, len, 0, (struct sockaddr *) &remote, remote_length); //**** CALL SENDTO() HERE ****;
	if (nbytes < 0) {
		printf("Error in Send.");
	}
	
	// Blocks till bytes are received
	//struct sockaddr_in from_addr;
	//int addr_length = sizeof(struct sockaddr);
	bzero(buffer,sizeof(buffer));

	if (strncmp(command, "get", 3) == 0) {
		printf("Writing the file into 'temp' from server.\n");
		FILE *fp;
		fp = fopen("temp", "a");
		/*nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0,
				(struct sockaddr *) &remote, &remote_length);*/
		while(2) {
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0,
					(struct sockaddr *) &remote, &remote_length);
			if (strcmp(buffer, "done") == 0) {
				break;
			}
			printf("Data: %s\n", buffer);
			fwrite(buffer, sizeof(buffer) + 2, 1, fp);
			bzero(buffer, sizeof(buffer));
		}
		printf("Finished. Check file to see if it transferred properly.\n");
		fclose(fp);
	}
	else if (strncmp(command, "put", 3) == 0) {
		printf("Sending file to server.\n");
		strncpy(fileName, &command[4], 4);
		FILE *fp;
		fp = fopen(fileName, "r");
		if (fp == NULL) {
			printf("Can't find the file ayy lmao.\n");
		}
		while(2) {
			bytesRead = fread(msg, sizeof(msg) + 1, 1, fp);
			if (bytesRead == 0)
				break;
			printf("Data: %s\n", msg);
			nbytes = sendto(sock, msg, strlen(msg), 0, 
				(struct sockaddr *) &remote, remote_length);
			bzero(msg, sizeof(msg));
		}
		bzero(msg, sizeof(msg));
		strcpy(msg, "done");
		nbytes = sendto(sock, msg, strlen(msg), 0, 
			(struct sockaddr *) &remote, remote_length);
		fclose(fp);
		printf("Done sending that file.\n");
} 
	else {
	nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0,
			(struct sockaddr *) &remote, &remote_length);
	printf("Server says %s\n", buffer);
}
}
	close(sock);

}

