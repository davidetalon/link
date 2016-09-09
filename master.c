/**
* @file master.c
* @functions for master side
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


/**
* @brief starts master mode
*
* @param *userName a string with current username
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int masterMode(const char *userName) {

	// server UDP address
	struct sockaddr_in udpSrvSockAddr;
	memset(&udpSrvSockAddr, 0, sizeof(udpSrvSockAddr));

	udpSrvSockAddr.sin_family = AF_INET;
	udpSrvSockAddr.sin_port = htons(UDP_SERVER_PORT);
	udpSrvSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int udpSrvAddrLen = sizeof(udpSrvSockAddr);

	//client address
	struct sockaddr_in udpClntSockAddr;

	int udpClntSockAddrLen = sizeof(udpClntSockAddr);


	//create UDP server socket
	int udpSrvSock = socket(AF_INET, SOCK_DGRAM, 0);

	if (udpSrvSock < 0) {
		perror("\nCannot create UDP server socket: ");
		return -1;
	}

	//bind UDP server socket
	if (bind(udpSrvSock, (struct sockaddr *) &udpSrvSockAddr, udpClntSockAddrLen) < 0) {
		perror("\nCannot bind UDP server socket: ");
		return -1;
	}


	if (openUdpSrv(udpSrvSock, (struct sockaddr_in *) &udpClntSockAddr, udpClntSockAddrLen, userName) < 0) {
		printf("Cannot start UDP server");
		return -1;
	}


	//server TCP address
	struct sockaddr_in tcpSrvSockAddr;
	memset(&tcpSrvSockAddr, 0, sizeof(tcpSrvSockAddr));

	tcpSrvSockAddr.sin_family = AF_INET;
	tcpSrvSockAddr.sin_port = htons(TCP_SERVER_PORT);
	tcpSrvSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int tcpSrvSockAddrLen = sizeof(tcpSrvSockAddr);

	//client TCP address
	struct sockaddr_in tcpClntSockAddr;

	int tcpClntSockAddrLen = sizeof(tcpClntSockAddr);


	//create TCP server socket
	int tcpSrvSock = socket (AF_INET, SOCK_STREAM, 0);

	if (tcpSrvSock < 0) {
		perror("\nCannot create TCP server socket: ");
		return -1;
	}

	if (bind(tcpSrvSock, (struct sockaddr *) &tcpSrvSockAddr, tcpClntSockAddrLen) < 0) {
		perror("\nCannot bind TCP server socket: ");
		return -1;
	}

	//create socket for connection 
	int connectionSock;

	int tcpSrvIsOpen = openTcpSrv(tcpSrvSock, (struct sockaddr_in *) &tcpSrvSockAddr, tcpSrvSockAddrLen, 
		(struct sockaddr_in *) &tcpClntSockAddr, tcpClntSockAddrLen, &connectionSock);

	//verify connection
	if(tcpSrvIsOpen < 0) {
		printf("\nCannot enstabilist connection\n");
	}

	printf("Connection enstabilished\n");

	printf("ConnectionSock: %d\n", connectionSock);

	// //file name of the received file
	char *fileToReceive;
	fileToReceive = (char *) malloc(MAX_FILE_NAME_SIZE);
	memset(fileToReceive, 0, sizeof(fileToReceive));

	// char fileToReceive[MAX_FILE_NAME_SIZE];
	// memset(&fileToReceive, 0, sizeof(fileToReceive));


	char *senderName;
	senderName = (char *) malloc(MAX_NAME_LENGTH);

	// char senderName[MAX_NAME_LENGTH + 1 ];
	// memset(&senderName, 0, sizeof(senderName));

	// printf("file %d, nome %d\n", &fileToReceive, &senderName);

	//ask user to accept file
	if (acceptFile(connectionSock, &fileToReceive, &senderName) < 0) {
		printf("File not Accepted. \n");
		return 0;
	}

	printf("\nReceiving file %s from %s...\n", fileToReceive, senderName);

	//prova
	// char serviceBuffer[SERVICE_BUFFER_SIZE];
	// memset(&serviceBuffer, 0, sizeof(serviceBuffer));

	// recv(connectionSock, serviceBuffer, sizeof(serviceBuffer),  0);

	// printf("buffer: %s\n", serviceBuffer);


	// receive file from client
	if (receiveFile(connectionSock, fileToReceive) < 0) {
		printf("Cannot receive file.\n");
		return -1;
	}

	close(connectionSock);
	close(tcpSrvSock);
	return 0;

}


/**
* @brief opens a UDP socket listening for incoming requests, answers with LINKAPP/SRVON/<name>
*
* @param udpSrvSock an integer specifying UDP master socket.
* @param *udpClntSockAddr a pointer to sockaddr_in struct with UDP sleave socket address.
* @param udpClntSockAddrLen an integer with UDP sleave socket size.
* @param *name a string with curren username.
*
* @return 0 if no errors occours, -1 otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/

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


/**
* @brief opens a TCP master socket listening for incoming connection, accept them.
*
* @param tcpSrvSock an integer specifying TCP master socket.
* @param *tcpSrvSockAddr a pointer to sockaddr_in struct with TCP master socket address.
* @param tcpSrvSockAddrLen an integer with TCP master socket size.
* @param *tcpClntSockAddr a pointer to sockaddr_in struct with TCP sleave socket address.
* @param tcpClntSockAddrLen an integer with TCP sleave socket size.
* @param *connectionSock a pointer to opened connection's socket.
*
* @return 0 if no errors occours, -1 otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
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


/**
* @brief receives file from sleave.
*
* @param *connectionSock a pointer to opened connection's socket.
* @param *fileName a string with fileName to receive.
*
* @return 0 if no errors occours, -1 otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
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


/**
* @brief ask user to accept file from sleave and answer to sleave.
*
* @param *connectionSock a pointer to opened connection's socket.
* @param *fileName a string with fileName to receive.
* @param **senderName a string with name of sleave.
*
* @return 0 if no errors occours, -1 otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
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