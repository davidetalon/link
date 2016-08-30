/**
* @test_search_master.c
* @brief open a TCP socket to Link master
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/


#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in>
#include <ctype.h>


/****************************/

const static int UDP_CLIENT_PORT = 1234;
const static int UDP_SERVER_PORT = 1235;

const static int TCP_CLIENT_PORT = 2345;
const static int TCP_SERVER_PORT = 2346;



const static int SERVICE_BUFFER_SIZE = 256;
const static int SERVER_SEARCH_TIME = 15;
const static int MAX_NUMBER_SERVERS = 128;


const static char VALID_SERVER_ON[] = "LINKAPP:SRVON:";
const static char VALID_CLIENT_REQUEST = "LINKAPP:CLNTRQT:SRVON?";

/**************************/

struct srv {
	char name[35];
	struct sockaddr_in sockAddr;
	int SockAddrLen;
}


//function for broadcasting a service messace which pourpose is to discover Link masters listening on the local network
int sendbroadcast (int srvSocket, const sockaddr_in *clntSockAddr, const int *clntSockAddrLen, sockaddr_in *srvSockAddr, int *srvSockAddrLen) {

	//setting socket for broadcasting
	int broadcast = 1;
	if (setsockopt(udpClntSock, SOL_SOCKET, SO_BROADCAST, (void*) &broadcast, sizeof(broadcast)) < 0) {
		perror("\nCannot set socket for broadcasting: ");
	}

	char buffer[SERVICE_BUFFER_SIZE];
	memset(buffer, VALID_CLIENT_REQUEST, 22);

	if( sendto(srvSocket, buffer, sizeof(buffer), 0, (struct sockaddr *) &srvSockAddr, &srvSockAddrLen) < 0) {
		perror("\nCannot send broadcast message: ");
		return -1
	}

	return 0;
}

//found server on the local network
int srvsInNet(int const udpClntSock, const struct sockaddr_in udpClntSockAddr, const int *udpClntSockAddrLen, struct srv *foundSrvs) {

	//time to wait until show found servers
	unsigned int delay = time(0) + SERVER_SEARCH_TIME;

	char buffer[SERVICE_BUFFER_SIZE];

	struct sockaddr_in srvAddr;
	srvAddrLen = sizeof(srvAddr);

	unsigned int delay = time(0) + SERVER_SEARCH_TIME;
	while (time(0) < delay) {

		//cleaning buffer
		bzero(buffer, SERVICE_BUFFER_SIZE);

		//checking received message
		if (recvfrom(udpClntSock, buffer, SERVICE_BUFFER_SIZE, 0, (struct sockaddr *) &srvAddr, &srvAddrLen)) < 0) {
			return -1;
		}

		//checking received message
		if (strncmp(buffer, VALID_SERVER_ON, 14) == 0) {
			*foundSrvs[foundSrvsLen].name = buffer;
			*foundSrvs[foundSrvsLen].sockAddr = srvAddr;
			*foundSrvs[foundSrvsLen].SockAddrLen = sizeof(srvAddr);
			
			foundSrvsLen = foundSrvsLen + 1;
		}
	}

	return foundSrvsLen;

}

//listing servers avaiable
int listingSrvs (const struct srv *foundSrvs, int foundSrvsLen) {

	printf("Found Servers:");
	for(int i = 0; i < foundSrvsLen; i++) {
		printf("\n%d. %s", i, toupper(*foundSrvs[i].name));
	}

	return 0;

}

//open connection with selected master
int openConnection(int tcpClntSock, struct sockaddr_in *tcpClntSockAddr, int tcpClntSockAddr, int selectedServer, struct srv *foundSrvs) {

	printf("Connetcting to %s...", *foundSrvs[selectedServer].name);
	if (connect(tcpClntSock, (struct sockaddr *) &foundSrvs[selectedServer].srvAddr, sizeof(*foundSrvs[selectedServer].srvAddrLen)) < 0) {
      perror("Cannot connect to master: ");
      return -1;
   }

}


int main (void) {

	//client UDP address
	struct sockaddr_in udpClntSockAddr;
	udpClntSockAddr.sin_family = AF_INET;
	udpClntSockAddr.sin_port = htons(UDP_CLIENT_PORT);
	udpClntSockAddr.sin_addr.s_addr = inet_addr(INADDR_ANY);

	int udpClntSockAddrLen = sizeof(udpClntSockAddr);

	//client TCP address
	struct sockaddr_in tcpClntSockAddr;
	tcpClntSockAddr.sin_family = AF_INET;
	tcpClntSockAddr.sin_port = htons(TCP_CLIENT_PORT);
	tcpClntSockAddr.sin_addr.s_addr = inet_addr(INADDR_ANY);

	int tcpClntSockAddrLen = sizeof(tcpClntSockAddr);


	//server address
	struct sockaddr_in udpSrvSockAddr;

	int udpSrvSockAddrLen = sizeof(udpSrvSockAddr);


	//create UDP client socket
	int udpClntSock = socket(AF_INET, SOCK_DGRAM, 0);

	if (udpClntSock < 0) {
		perror("\nCannot create client UDP socket: ");
		return -1;
	}

	//binding UDP client socket
	if (bind(udpClntSock, (struct sockaddr *) &udpClntSockAddr, udpClntSockAddrLen) < 0) {
		perror("\nCannot bind UDP client socket: ");
		return -1
	}

	//sending broadcast message
	int broadcasted = sendbroadcast(udpClntSock, (struct sockaddr *) udpClntSockAddr, udpClntSockAddrLen, 
		(struct sockaddr_in *) udpSrvSockAddr, udpSrvSockAddrLen);

	if (broadcasted < 0) {
		perror("\nCannot send broadcast message: ");
		return -1;
	}

	//array of found servers
	struct srv *foundSrvs[MAX_NUMBER_SERVERS];

	foundSrvsLen = srvsInNet(udpClntSock, (struct sockaddr *) &udpClntSockAddr, udpClntSockAddrLen, (struct srv *) &foundSrvs);
	
	if (foundSrvsLen < 0) {
		printf("\nMaster research failed");
		return -1;
	} else {

		//listing servers avaiable
		listingSrvs((struct srv*) &foundSrvs, foundSrvsLen);

		int selectedServer;
		scanf("%d", &selectedServer);
		while (selectedServer < 0 || selectedServer > foundSrvs -1) {
			printf("\nScegliere un server valido: ");
			scanf("%d", &selectedServer);
		}

		//open a TCP connection with server
		tcpClntSock = socket (AF_INET, SOCK_STREAM, 0);
		if (tcpClntSock < 0) {
			perror("Cannot create TCP socket for communication: ");
		}

		//binding TCP client socket
		if (bind(tcpClntSock, (struct sockaddr *) &tcpClntSockAddr, tcpClntSockAddrLen) < 0) {
			perror("\nCannot bind TCP client socket: ");
			return -1
		}

		if (openConnection(tcpClntSock, (struct sockaddr_in *) &tcpClntSockAddr, sizeof(tcpClntSockAddr),
		 selectedServer, (struct srv*) foundSrvs) < 0) {
			printf("\nCannot communicate with master!");
			return -1;
		}

		return 0;

	}



	return 0;
}

