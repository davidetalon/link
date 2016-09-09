/**
* @master.h
* @header for master
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/

#ifndef MASTER_H_INCLUDED   /* Include guard */
#define MASTER_H_INCLUDED

//start master mode
int masterMode(const char *userName);

//open an UDP server
int openUdpSrv(const int udpSrvSock, const struct sockaddr_in *udpClntSockAddr, int udpClntSockAddrLen, const char *name);

//open a TCP server
int openTcpSrv(const int tcpSrvSock, struct sockaddr_in *tcpSrvSockAddr, const int tcpSrvSockAddrLen, 
	struct sockaddr_in *tcpClntSockAddr, int tcpClntSockAddrLen, int *connectionSock);

//receive file
int receiveFile (const int *connectionSock, const char *fileName);

//accept to receive file
int acceptFile(const int *connectionSock, char **fileName, char **senderName);

#endif 