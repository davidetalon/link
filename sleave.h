/**
* @sleave.c
* @header for sleave
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/

#ifndef SLEAVE_H_INCLUDED   /* Include guard */
#define SLEAVE_H_INCLUDED


//start sleave mode
int sleaveMode(const char *userName, const char *fileName);

//function for broadcasting a service messace which pourpose is to discover Link masters listening on the local network
int sendBroadcast (int clntSock, const struct sockaddr_in *clntSockAddr, const int *clntSockAddrLen, 
	struct sockaddr_in *srvSockAddr, int srvSockAddrLen);

//found server on the local network
int srvsInNet(const int udpClntSock, srv *srvs); 

//listing servers avaiable
int listingSrvs (const struct srv *foundSrvs, int foundSrvsLen);

//open connection with selected master
int openConnection(int tcpClntSock, struct sockaddr_in *tcpClntSockAddr, int tcpClntSockAddrLen, int selectedServer, srv *foundSrvs);

//verify if file or folder exists in directory
char exists(const char *fileName, char c);

//ask to master to accept file
int askToSendFile (const int tcpClntSock, const char *fileName, const char *name);

// function for sending file
int sendFile (const int tcpClntSock, const char *fileName);

#endif 