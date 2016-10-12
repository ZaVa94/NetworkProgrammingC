#include <time.h>
#include <sys/types.h>
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
#include <sys/dir.h>
#include <memory.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>

/* You will have to modify the program below */

#define MAXBUFSIZE 100

extern int alphasort();

int find_file(const struct direct *entry) {
	return 1;
}

int main (int argc, char * argv[] )
{


	int sock;
	int bytesRead;                           //This will be our socket
	struct sockaddr_in sin, remote;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             //a buffer to store our received message
	if (argc != 2)
	{
		printf ("USAGE:  <port>\n");
		exit(1);
	}

	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&sin,sizeof(sin));                    //zero the struct
	sin.sin_family = AF_INET;                   //address family
	sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
	sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("unable to create socket");
		exit(1);
	}


	/******************
	  Once we've created a socket, we must bind that socket to the 
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("unable to bind socket\n");
		exit(1);
	}
	
	printf("Listening for client on port %s\n", argv[1]);
	char msg[MAXBUFSIZE];
	char fileName[MAXBUFSIZE];
	char bufferS[MAXBUFSIZE];
	remote_length = sizeof(remote);
	while(1) {
		
	bzero(msg, sizeof(msg));//waits for an incoming message
	bzero(buffer,sizeof(buffer));
	bzero(fileName, sizeof(fileName));
	bzero(bufferS, sizeof(bufferS));
	nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0,
		(struct sockaddr *) &remote, &remote_length); //**** CALL RECVFROM() HERE ****;

	printf("The client says %s\n", buffer);
	if (strncmp(buffer, "ls", 2) == 0) {
		int i;
				  
		struct direct **file_name; //structure necessary for finding files in a directory
		int count = scandir(".", &file_name, find_file, alphasort); // count for files
		if (count <= 0){
			printf("No files found in the specified directory");
		}
		printf("Number of files found: %d\n", count);
		for (i = 0; i < count; ++i) {
			sprintf(msg + strlen(msg), "%s\n ", file_name[i]->d_name); // store all file names in directory
		}
		printf("Here's your list of files.\n");
		nbytes = sendto(sock, msg, strlen(msg), 0, 
			(struct sockaddr *) &remote, remote_length);
	}
	else if (strncmp(buffer, "exit", 4) == 0) {
		strcpy(msg, "'peace out!!' (server exited successfully)");
		nbytes = sendto(sock, msg, strlen(msg), 0, 
			(struct sockaddr *) &remote, remote_length);
			printf("Bye!!\n");
		exit(0);
	}
	else if (strncmp(buffer, "put", 3) == 0) {
		printf("Writing the file into 'temp' from client.\n");
		FILE *fp;
		fp = fopen("temp", "a");
		/*nbytes = recvfrom(sock, bufferS, MAXBUFSIZE, 0,
				(struct sockaddr *) &remote, &remote_length);*/
		while(2) {
			nbytes = recvfrom(sock, bufferS, MAXBUFSIZE, 0,
					(struct sockaddr *) &remote, &remote_length);
			if (strcmp(bufferS, "done") == 0) {
				break;
			}
			printf("Data: %s\n", bufferS);
			fwrite(bufferS, sizeof(bufferS) + 1, 1, fp);
			bzero(bufferS, sizeof(bufferS));
		}
		printf("Finished. Check file to see if it transferred properly.\n");
		fclose(fp);
	}
	else if (strncmp(buffer, "get", 3) == 0) {
			printf("Writing your bullshit file.\n");
			strncpy(fileName, &buffer[4], 4);
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
			printf("Done sending the file.\n");
			bzero(msg, sizeof(msg));
	}
	else {
		strncpy(msg, buffer, strlen(buffer));
		strcat(msg, "Error: no idea what that command means ayy lmao.\n");
		nbytes = sendto(sock, msg, strlen(msg), 0, 
			(struct sockaddr *) &remote, remote_length);
	} //**** CALL SENDTO() HERE ****;
}
	close(sock);
}

