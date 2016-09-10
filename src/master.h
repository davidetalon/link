/**
* @file master.h
* @brief header for master side
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

/**
* @brief starts master mode
*
* @param *userName a string with current username
*
* @return 0 if no errors occours, -1 otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int masterMode(const char *userName);


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
int openUdpSrv(const int udpSrvSock, const struct sockaddr_in *udpClntSockAddr, int udpClntSockAddrLen, const char *name);


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
* @bug BIND GET ADDRES ALREDY IN USE
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int openTcpSrv(const int tcpSrvSock, struct sockaddr_in *tcpSrvSockAddr, const int tcpSrvSockAddrLen, 
	struct sockaddr_in *tcpClntSockAddr, int tcpClntSockAddrLen, int *connectionSock);


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
int receiveFile (const int *connectionSock, const char *fileName);


/**
* @brief ask user to accept file from sleave and answer to sleave.
*
* @param *connectionSock a pointer to opened connection's socket.
* @param **fileName a string with fileName to receive.
* @param **senderName a string with name of sleave.
*
* @return 0 if no errors occours, -1 otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int acceptFile(const int *connectionSock, char **fileName, char **senderName);

#endif 