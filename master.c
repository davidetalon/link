/**
* @master.c
* @master functions
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/

#include "linkutils.h"
#include "master.h"

int openUdpSrv(const int udpSrvSock, const struct sockaddr_in *udpClntSockAddr, int udpClntSockAddrLen, const char *name) {

	char buffer[SERVICE_BUFFER_SIZE];

	//need to insert EXIT CHAR to interrupt master mode
	while (1) {

		//cleaning buffer
		bzero(buffer, SERVICE_BUFFER_SIZE);

		//testing
		printf("\nWaiting for sleave...");
		fflush(stdout);

		//checking received message
		if (recvfrom(udpSrvSock, buffer, SERVICE_BUFFER_SIZE, 0, (struct sockaddr *) udpClntSockAddr, &udpClntSockAddrLen) < 0) {
			perror("\nError receiving message from sleave: ");
			return -1;
		}
		
		//testing
		printf("UDP request: %s", buffer);
		fflush(stdout);

		//validating message from sleave
		if (strncmp(buffer, VALID_CLIENT_REQUEST, 22) == 0) {
			memset(buffer, 0, SERVICE_BUFFER_SIZE);
			
			strncpy(buffer, VALID_SERVER_ON, 14);
			strcat(buffer, name);
			//testing
			printf("\nServer response: %s", buffer);
			fflush(stdout);	

			//responding to sleave SRVON
			if (sendto(udpSrvSock, buffer, sizeof(buffer), 0, (struct sockaddr *) udpClntSockAddr, udpClntSockAddrLen) < 0) {
				perror("\nCannot send SRVON to sleave: ");
				return -1;
			}
			return 0;

			
		} else {
			printf("\nInvalid sleave request");
		}
	}

	return 0;
}

int openTcpSrv(const int tcpSrvSock, struct sockaddr_in *tcpSrvSockAddr, const int tcpSrvSockAddrLen, 
	struct sockaddr_in *tcpClntSockAddr, int tcpClntSockAddrLen, int *connectionSock) {

	printf("\nStarting TCP server...\n");
	fflush(stdout);
	//start listening with queue size of 5
	if(listen(tcpSrvSock, 5) < 0) {
		printf("Error starting listening\n");
		// exit(1);
	}

	//search for a connection request
	int request;
	while (1) {

		printf("ConnectionSock: %d\n", connectionSock);
		//accept connection with sleave
		request = accept(tcpSrvSock, (struct sockaddr *) &tcpClntSockAddr, &tcpClntSockAddrLen);

		printf("ConnectionSock after accept: %d\n", request);

		*connectionSock = request;
		
		
		if(connectionSock < 0) {
			perror("Connection with sleave not accepted: ");
			return -1;
		}

		return 1;
	}
	
}

int receiveFile (const int *connectionSock, const char *fileToReceive) {

	char buffer[DATA_BUFFER_SIZE];

	send(connectionSock, SEND_ACCEPTED, sizeof(SEND_ACCEPTED), 0);
	printf("Server response:%s\n", SEND_ACCEPTED);

	//creating service buffer for receiving file size
	char serviceBuffer[SERVICE_BUFFER_SIZE];
	memset(&serviceBuffer, 0, sizeof(serviceBuffer));

	//receiving buffer size
	recv(connectionSock, serviceBuffer, sizeof(serviceBuffer),  0);
	printf("buffer: %s\n", serviceBuffer);

	//transform buffer into a int
	int size = atoi(serviceBuffer);
	printf("File size to receive: %d\n", size);

	// opening file
	FILE *fpOutput;
	fpOutput = fopen(fileToReceive, "w");

	if(fpOutput == NULL) {
		printf("NONO\n");
	}

	//byte read
	int offset = 0;
	int n;

	// receiving message
	while( n = read(connectionSock, buffer, sizeof(buffer)) > 0 && offset < size) {
		fwrite(buffer, DATA_BUFFER_SIZE, 1, fpOutput);
		// printf("%s", buffer);
		offset = offset + n;
	}



	return 0;
}

int acceptFile(const int *connectionSock, char **fileToReceive, char **senderName) {


	//header will be the first HEADER_SIZE chars LINKAPP/SLVNAME/<sleave name>/FNAME/<file name.tar.gz>/


	//receiving filename and sender name
	char header[HEADER_SIZE];

	//variables used
	char sName[MAX_NAME_LENGTH + 1];
	char fName[MAX_FILE_NAME_SIZE + 1];
	
	recv(connectionSock, header, HEADER_SIZE, 0);

	if (strcmp(header, "") == 0) {
		printf("Connessione interrotta\n");
		return -1;
	}
	
	printf("Header: %s\n", header);
	strtok(header, "/");
	strtok(NULL, "/");
	strcat(sName, strtok(NULL, "/"));
	strcpy(*senderName, sName);

	strtok(NULL, "/");
	strcat(fName, strtok(NULL, "/"));
	strcpy(*fileToReceive, fName);


	printf("Accept file %s from %s? (Y/N) ", *fileToReceive, *senderName);

	char accepted = toupper(getchar());	

	if(accepted == 'Y') {
		return 0;
	} else {
		return -1;
	}

}