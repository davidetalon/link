/**
* @master.c
* @connect master to sleave
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/



/**
* IDEA
*
* 1. Apro un server UDP che ascolti i messaggi di broadcast in entrata
* 2. Ricevuto un messaggio broadcast rispondo con SRVON:<nomeserver>
* 3. Metto in ascolto un server TCP
* 4. Accetto la connessione con lo sleave
* 5. Ricevo il file
* 6. Chiudo la connessione e spengo il server
* 7. Chiudo il socket
*/


#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>


/************* APPLICATION OPTION *************/

static const int UDP_CLIENT_PORT = 1234;
static const int UDP_SERVER_PORT = 1235;

static const int TCP_CLIENT_PORT = 2345;
static const int TCP_SERVER_PORT = 2346;



static const int SERVICE_BUFFER_SIZE = 256;
static const int DATA_BUFFER_SIZE = 4096;
static const int HEADER_SIZE = 336;

static const char VALID_SERVER_ON[] = "LINKAPP/SRVON/";
static const char VALID_CLIENT_REQUEST[] = "LINKAPP/CLNTRQT/SRVON?/";
static const char SEND_ACCEPTED[] = "LINKAPP/SEND/ACCEPTED";
static const char SEND_NOT_ACCEPTED[] = "LINKAPP/SEND/NOTACCEPTED";

static const char NAME[] = "TalloMaster";

static const int MAX_FILE_NAME_SIZE = 262;
static const int MAX_NAME_LENGTH = 50;

/*******************************************/


int main (void) {

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


	if (openUdpSrv(udpSrvSock, (struct sockaddr_in *) &udpClntSockAddr, udpClntSockAddrLen) < 0) {
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

	//file name of the received file
	char *fileName;
	fileName = (char *) malloc(MAX_FILE_NAME_SIZE);
	// memset(fileName, 0, sizeof(fileName));


	char *senderName;
	senderName = (char *) malloc(MAX_NAME_LENGTH);

	// char senderName[MAX_NAME_LENGTH + 1 ];
	// memset(&senderName, 0, sizeof(senderName));

	printf("file %d, nome %d\n", &fileName, &senderName);
	//ask user to accept file
	if (acceptFile(connectionSock, &fileName, &senderName) < 0) {
		printf("File not Accepted. \n");
		return 0;
	}

	printf("\nReceiving file %s from %s...\n", fileName, senderName);

	//prova
	// char serviceBuffer[SERVICE_BUFFER_SIZE];
	// memset(&serviceBuffer, 0, sizeof(serviceBuffer));

	// recv(connectionSock, serviceBuffer, sizeof(serviceBuffer),  0);

	// printf("buffer: %s\n", serviceBuffer);


	// receive file from client
	if (receiveFile(connectionSock, fileName) < 0) {
		printf("Cannot receive file.\n");
		return -1;
	}

	close(connectionSock);
	close(tcpSrvSock);
	return 0;
}

int acceptFile(const int *connectionSock, char **fileName, char **senderName) {


	//header will be the first HEADER_SIZE chars LINKAPP/SLVNAME/<sleave name>/FNAME/<file name.tar.gz>/


	//receiving filename and sender name
	char header[HEADER_SIZE];

	//variables used
	char sName[MAX_NAME_LENGTH + 1];
	char fName[MAX_FILE_NAME_SIZE + 1];
	
	read(connectionSock, header, sizeof(header));
	printf("Header: %s\n", header);
	strtok(header, "/");
	strtok(NULL, "/");
	strcat(sName, strtok(NULL, "/"));
	strcpy(*senderName, sName);

	strtok(NULL, "/");
	strcat(fName, strtok(NULL, "/"));
	strcpy(*fileName, fName);


	printf("Accept file %s from %s? (Y/N) ", *fileName, *senderName);

	char accepted = toupper(getchar());	

	if(accepted == 'Y') {
		return 0;
	} else {
		return -1;
	}

}

int openUdpSrv(const int udpSrvSock, const struct sockaddr_in *udpClntSockAddr, int udpClntSockAddrLen) {

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
			strcat(buffer, NAME);
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

int receiveFile (const int *connectionSock, const char *fileName) {

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
	fpOutput = fopen(fileName, "w");

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