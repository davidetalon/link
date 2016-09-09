/**
* @sleave.c
* @sleave functions
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/

#include "linkutils.h"
#include "sleave.h"

//start sleave mode
int sleaveMode(const char *userName, const char *fileName){
	
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


	//TCP client socket
	int tcpClntSock;

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
		tcpClntSock = socket (AF_INET, SOCK_STREAM, 0);
		if (tcpClntSock < 0) {
			perror("Cannot create TCP socket for communication: ");
		}

		//binding TCP client socket
		if (bind(tcpClntSock, (struct sockaddr *) &tcpClntSockAddr, tcpClntSockAddrLen) < 0) {
			perror("\nCannot bind TCP client socket: ");
			close(tcpClntSock);
			return -1;
		}

		if (openConnection(tcpClntSock, (struct sockaddr_in *) &tcpClntSockAddr, sizeof(tcpClntSockAddr),
		 selectedServer, (struct srv*) foundSrvs) < 0) {
			printf("\nCannot communicate with master!\n");
			close(tcpClntSock);
			return -1;
		} else {
			printf("\nConnection enstabilished.\n");
		}

	}

	//verify if Folder fileName exists
	char cDir = exists (fileName, 'd');

	//verify if file fileName exists
	char cFile = exists (fileName, 'f');

	//check if the file exists (the returned value from pipe)
	if (cDir == '1' || cFile == '1' ) {

		char tarCommand[TAR_COMMAND_SIZE];
		memset(&tarCommand, 0, sizeof(char) * TAR_COMMAND_SIZE);

		//invoke OS tar to compress
		strcpy(tarCommand, "tar -zcf " );
		strcat(tarCommand, fileName);
		strcat(tarCommand, ".tar.gz ");
		strcat(tarCommand, fileName);
		system(tarCommand);
		printf("tarCommand command: %s\n", tarCommand);

	} else {

		printf("File doesn't exist\n");
		close(tcpClntSock);
		return -1;

	}

	//ask to send file
	if (askToSendFile(tcpClntSock, fileName, userName) < 0 ){
		printf("File not accepted.\n");
		
	} else {

		// sendfile
		if(sendFile(tcpClntSock, fileName) < 0) {
			printf("Error sendig file.\n");
			close(tcpClntSock);
			return -1;
		}

		//remove temporary file .tar.gz
		char rmCommand[REMOVE_COMMAND_SIZE];
		strcpy(rmCommand, "rm -r ");
		strcat(rmCommand, fileName);
		strcat(rmCommand, ".tar.gz");
		system(rmCommand);
		printf("Remove command: %s\n", rmCommand);

		printf("File succefully sent.\n");
		return 0;

	}

	close(tcpClntSock);
	return 0;

}

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

//listing servers avaiable
int listingSrvs (const struct srv *foundSrvs, int foundSrvsLen) {

	for(int i = 0; i < foundSrvsLen; i++) {
		printf("\n%d. %s", i, foundSrvs[i].name);
	}

	return 0;

}

//open connection with selected master
int openConnection(int tcpClntSock, struct sockaddr_in *tcpClntSockAddr, int tcpClntSockAddrLen, int selectedServer, srv *foundSrvs) {

	foundSrvs[selectedServer].sockAddr.sin_port = htons(TCP_SERVER_PORT);

	printf("Connetcting to %s...", foundSrvs[selectedServer].name);
	
	//verify connection enstabilished
	if (connect(tcpClntSock, (struct sockaddr *) &foundSrvs[selectedServer].sockAddr, foundSrvs[selectedServer].sockAddrLen) < 0) {
      perror("Cannot connect to master: ");
      return -1;
   	}

   	return 0;

}

//verify if file or folder exists in directory
char exists(const char *fileName, char c) {

	if (c != 'f' && c != 'd') {
		printf("Argument not valid");
		return 0;
	}

	//construction of system call for verifying if file exists
	char existsCommand[EXISTS_COMMAND_SIZE];
	memset(&existsCommand, 0, sizeof(char) * EXISTS_COMMAND_SIZE);

	if (c == 'f') {
		strcpy(existsCommand, "[ -f ");
	} else {
		strcpy(existsCommand, "[ -d ");
	}

	strcat(existsCommand, fileName);
	strcat(existsCommand, " ] && echo 1 || echo 0");

	// open pipe with unix shell
	FILE *fp;
	fp = popen(existsCommand, "r");

	if (fp == NULL) {
    	printf("Failed to run command\n" );
    	exit(1);
  	}

  	char result = fgetc(fp);

  	//closing pipe with unix shell
  	int status = pclose(fp);
  	if (status == -1) {
	   printf("Error closing pipe with shell");
	}

	return result;

}

//ask to master to accept file
int askToSendFile (const int tcpClntSock, const char *fileName, const char *name) {

	//header will be the first HEADER_SIZE chars SLVNAME/<sleave name>/FNAME/<file name.tar.gz>/
	char header[HEADER_SIZE];
	memset(&header, 0, sizeof(header));

	strcpy(header, "LINKAPP/SLVNAME/");
	strcat(header, name);
	strcat(header, "/FNAME/");
	strcat(header, fileName);
	strcat(header, ".tar.gz/");

	if (send(tcpClntSock, header, sizeof(header), 0) > 0 ) {
		printf("Header sent: %s\nHeader size: %d\n", header, sizeof(header));
	}

	char buffer[SERVICE_BUFFER_SIZE];
	memset(&buffer, 0, sizeof(buffer));

	read(tcpClntSock, buffer, sizeof(buffer));
	printf("Server response: %s\n", buffer);
	
	if(strcmp(buffer,SEND_ACCEPTED) != 0) {
		return -1;
	} 

	return 0;

}


// function for sending file
int sendFile (const int tcpClntSock, const char *fileName) {

	char buffer[DATA_BUFFER_SIZE];

	char compressedFileName[MAX_FILE_NAME_SIZE];
	strcpy(compressedFileName, fileName);

	//constructing file to send name
	strcat(compressedFileName, ".tar.gz");

	//opening input file
	printf("Opening file %s ...\n", compressedFileName);

	FILE *fpInput;
	fpInput = fopen(compressedFileName, "r");

	if (fpInput == NULL) {
		printf("Cannot read file %s", compressedFileName);
	}

	//get file size
	struct stat st;
	stat(compressedFileName, &st);
	int size = st.st_size;
	printf("size: %d\n", size);
	// fseek(fpInput, 0, SEEK_SET);

	char serviceBuffer[SERVICE_BUFFER_SIZE];
	sprintf(serviceBuffer, "%d", size);
	if (send(tcpClntSock, serviceBuffer, sizeof(serviceBuffer), 0) < 0 ) {
		perror("Errore: ");
	}


	// reading input file
	fseek(fpInput, 0, SEEK_SET);
	while (!feof(fpInput)) {
		fread(buffer, DATA_BUFFER_SIZE, 1, fpInput);

		// printf("%s", buffer);

		// sending file using tcp connection 
		if (send(tcpClntSock, buffer, sizeof(buffer), 0) < 0) {
			printf("Error while sending.\n");
			return -1;
		}
	}

	close(fpInput);

	return 0;

}