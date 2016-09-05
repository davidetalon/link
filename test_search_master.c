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
#include <netinet/in.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>


/****************************/

static const int UDP_CLIENT_PORT = 1234;
static const int UDP_SERVER_PORT = 1235;

static const int TCP_CLIENT_PORT = 2345;
static const int TCP_SERVER_PORT = 2346;


static const int MAX_NAME_LENGTH = 50;
static const int SERVICE_BUFFER_SIZE = 128;
static const int SERVER_SEARCH_TIME = 5;
static const int MAX_NUMBER_SERVERS = 8;


static const char VALID_SERVER_ON[15] = "LINKAPP/SRVON/";
static const char VALID_CLIENT_REQUEST[22] = "LINKAPP/CLNTRQT/SRVON?/";

/**************************/

struct srv {
	char name[MAX_NAME_LENGTH];
	struct sockaddr_in sockAddr;
	int sockAddrLen;
};

typedef struct srv srv;

/************ MAIN ************/
int main (void) {

	//client UDP address
	struct sockaddr_in udpClntSockAddr;
	memset(&udpClntSockAddr, 0, sizeof(udpClntSockAddr));

	udpClntSockAddr.sin_family = AF_INET;
	udpClntSockAddr.sin_port = htons(UDP_CLIENT_PORT);
	udpClntSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int udpClntSockAddrLen = sizeof(udpClntSockAddr);

	//server UDP address
	struct sockaddr_in udpSrvSockAddr;
	memset(&udpSrvSockAddr, 0, sizeof(udpSrvSockAddr));
	
	udpSrvSockAddr.sin_family = AF_INET;
	udpSrvSockAddr.sin_port = htons(UDP_SERVER_PORT);
	udpSrvSockAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	int udpSrvSockAddrLen = sizeof(udpSrvSockAddr);


	//client TCP address
	struct sockaddr_in tcpClntSockAddr;
	tcpClntSockAddr.sin_family = AF_INET;
	tcpClntSockAddr.sin_port = htons(TCP_CLIENT_PORT);
	tcpClntSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int tcpClntSockAddrLen = sizeof(tcpClntSockAddr);


	//create UDP client socket
	int udpClntSock = socket(AF_INET, SOCK_DGRAM, 0);

	if (udpClntSock < 0) {
		perror("\nCannot create client UDP socket: ");
		return -1;
	}

	//binding UDP client socket
	if (bind(udpClntSock, (struct sockaddr *) &udpClntSockAddr, udpClntSockAddrLen) < 0) {
		perror("\nCannot bind UDP client socket: ");
		return -1;
	}

	//sending broadcast message
	int broadcasted = sendBroadcast(udpClntSock, (struct sockaddr *) &udpClntSockAddr, udpClntSockAddrLen, (struct sockaddr *) &udpSrvSockAddr, udpSrvSockAddrLen);

	if (broadcasted < 0) {
		perror("\nCannot send broadcast message: ");
		return -1;
	}


	srv foundSrvs[MAX_NUMBER_SERVERS];

	for (int i = 0; i < MAX_NUMBER_SERVERS; i++) {
		memset(foundSrvs[i].name, 0, MAX_NAME_LENGTH);
		foundSrvs[i].sockAddr.sin_family = 0;
		foundSrvs[i].sockAddr.sin_port = 0;
		foundSrvs[i].sockAddr.sin_addr.s_addr = 0;

		foundSrvs[i].sockAddrLen = sizeof(srv);
	}

	// int foundSrvsLen = srvsInNet(udpClntSock, (struct sockaddr *) &udpClntSockAddr, udpClntSockAddrLen, &foundSrvs);
	int foundSrvsLen = srvsInNet(udpClntSock, &foundSrvs);
	
	if (foundSrvsLen < 0) {
		printf("\nMaster research failed\n");
		return -1;
	} else if (foundSrvsLen == 0){

		printf("\n 0 Master found\n");

	} else {

		printf("%d masters found\n", foundSrvsLen);
		//listing servers avaiable
		listingSrvs((struct srv*) &foundSrvs, foundSrvsLen);
		printf("\nScegliere un master valido: ");
		int selectedServer;

		scanf("%d", &selectedServer);
		while (selectedServer < 0 || selectedServer > foundSrvsLen-1) {
			printf("\nScegliere un master valido: ");
			scanf("%d", &selectedServer);
		}

		//open a TCP connection with server
		int tcpClntSock = socket (AF_INET, SOCK_STREAM, 0);
		if (tcpClntSock < 0) {
			perror("Cannot create TCP socket for communication: ");
		}

		//binding TCP client socket
		if (bind(tcpClntSock, (struct sockaddr *) &tcpClntSockAddr, tcpClntSockAddrLen) < 0) {
			perror("\nCannot bind TCP client socket: ");
			return -1;
		}

		if (openConnection(tcpClntSock, (struct sockaddr_in *) &tcpClntSockAddr, sizeof(tcpClntSockAddr),
		 selectedServer, (struct srv*) foundSrvs) < 0) {
			printf("\nCannot communicate with master!\n");
			return -1;
		}

	}

	
	printf("\nConnection enstabilished.\n");

	return 0;
}

/************ END OF MAIN ************/


/************ SENDBROADCAST ************/

//function for broadcasting a service messace which pourpose is to discover Link masters listening on the local network
int sendBroadcast (int clntSock, const struct sockaddr_in *clntSockAddr, const int *clntSockAddrLen, struct sockaddr_in *srvSockAddr, int srvSockAddrLen) {

	//setting socket for broadcasting
	int broadcast = 1;
	if (setsockopt(clntSock, SOL_SOCKET, SO_BROADCAST, (void*) &broadcast, sizeof(broadcast)) < 0) {
		perror("\nCannot set socket for broadcasting: ");
	}

	char buffer[SERVICE_BUFFER_SIZE];
	strncpy(buffer, VALID_CLIENT_REQUEST, 22);

	if( sendto(clntSock, buffer, sizeof(buffer), 0, (struct sockaddr *) srvSockAddr, srvSockAddrLen) < 0) {
		return -1;
	}

	printf("Broadcast sent: %s\n", buffer);
	return 0;
}
/************ END OF SENDBROADCAST ************/

/************ SRVSINNET ************/

//found server on the local network
int srvsInNet(const int udpClntSock, srv *srvs) {

	//number of server
	int nSrvs = 0;

	struct sockaddr_in currentAddr;
	currentAddr.sin_family = AF_INET;
	currentAddr.sin_port = htons(UDP_SERVER_PORT);
	currentAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int currentAddrLen = sizeof(currentAddr);

	printf("Waiting for master response...\n");

	//set socket non blocking
	fcntl(udpClntSock, F_SETFL, O_NONBLOCK);
	int err;

	//buffer created
	char buffer[SERVICE_BUFFER_SIZE];

	int excededTime = time(0) + SERVER_SEARCH_TIME;

	while ((nSrvs <= MAX_NUMBER_SERVERS ) && (time(0) < excededTime) ) {

		//cleaning buffer
		bzero(buffer, SERVICE_BUFFER_SIZE);

		//checking received message
		recvfrom(udpClntSock, buffer, SERVICE_BUFFER_SIZE, 0, (struct sockaddr *) &currentAddr, &currentAddrLen);
		err = errno;

		//handle error from recvfrom
		if ((err != EAGAIN) && (err != EWOULDBLOCK)) {
      		printf("recv returned unrecoverable error(errno=%d)\n", err);
      		return -1;
      	}

		char *strName;
		//checking received message
		if (strncmp(buffer, VALID_SERVER_ON, 14) == 0) {

			//getting server name
			strtok(buffer, "/");
			strtok(NULL, "/");
			strName = strtok(NULL, "/");

			srv currentSrv;
			strcpy(currentSrv.name, strName);
			currentSrv.sockAddr = currentAddr;
			currentSrv.sockAddrLen  = sizeof(currentSrv.sockAddr);

			srvs[nSrvs] = currentSrv;
			nSrvs++;

		}	
	}

	return nSrvs;

}

/************ END OF SRVSINNET ************/


/************ LISTINGSERVS ************/

//listing servers avaiable
int listingSrvs (const struct srv *foundSrvs, int foundSrvsLen) {

	for(int i = 0; i < foundSrvsLen; i++) {
		printf("\n%d. %s", i, foundSrvs[i].name);
	}

	return 0;

}
/************ END OF LISTINGSERVS ************/


/************ OPENCONNECTION ************/

//open connection with selected master
int openConnection(int tcpClntSock, struct sockaddr_in *tcpClntSockAddr, int tcpClntSockAddrLen, int selectedServer, srv *foundSrvs) {

	foundSrvs[selectedServer].sockAddr.sin_port = htons(TCP_SERVER_PORT);

	printf("Connetcting to %s...", foundSrvs[selectedServer].name);
	if (connect(tcpClntSock, (struct sockaddr *) &foundSrvs[selectedServer].sockAddr, foundSrvs[selectedServer].sockAddrLen) < 0) {
      perror("Cannot connect to master: ");
      return -1;
   }

}
/************ END OF OPENCONNECTION ************/
