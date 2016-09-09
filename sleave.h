/**
* @file sleave.h
* @brief header for sleave side
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


/**
* @brief starts sleave mode
*
* @param *userName a string with current username
* @param *fileName a string with fileName to send.
*
* @return 0 if no errors occours, -1 otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int sleaveMode(const char *userName, const char *fileName);


/**
* @brief sends a broadcast message to all masters listening on the network.
*
* @param clntSock an integer specifying UDP sleave socket.
* @param *clntSockAddr a pointer to sockaddr_in struct with UDP sleave socket address.
* @param clntSockAddrLen an integer with UDP sleave socket size.
* @param *srvSockAddr a pointer to sockaddr_in struct with UDP master socket address.
* @param srvSockAddrLen an integer with UDP master socket size.
*
* @return 0 if no errors occours, -1 otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int sendBroadcast (int clntSock, const struct sockaddr_in *clntSockAddr, const int *clntSockAddrLen, 
	struct sockaddr_in *srvSockAddr, int srvSockAddrLen);


/**
* @brief finds masters on the local network.
*
* @param udpClntSock an integer specifying UDP sleave socket.
* @param *srvs a pointer to an array of srv struct.
*
* @return 0 if no errors occours, -1 otherwise.
*
* @bug RECV return errno=9 BEBADF Bad file number
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int srvsInNet(const int udpClntSock, srv *srvs); 


/**
* @brief list found masters on the local network.
*
* @param *foundSrvs a pointer to an array of srv struct.
* @param foundSrvsLen an integer specifyng number of found servers.
*
* @return number of servers on the network, -1 if errors occours.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int listingSrvs (const struct srv *foundSrvs, int foundSrvsLen);


/**
* @brief opens a TCP sleave socket connecting to master's socket.
*
* @param tcpClntSock an integer specifying TCP sleave socket.
* @param *tcpClntSockAddr a pointer to sockaddr_in struct with TCP sleave socket address.
* @param tcpClntSockAddrLen an integer with TCP sleave socket size.
* @param selectedServer an integer specifyng selected server
* @param *foundSrvs a pointer to an array of srv struct.
*
* @return 0 if no errors occours, -1 otherwise.
*
* @bug BIND GET ADDRES ALREDY IN USE
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int openConnection(int tcpClntSock, struct sockaddr_in *tcpClntSockAddr, int tcpClntSockAddrLen, int selectedServer, srv *foundSrvs);


/**
* @brief check if a file or folder exists.
*
* @param *fileName a string containing name of file to check.
* @param c a char that specify folder or file.
*
* @return '1' if file/folder exists, '0' otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
char exists(const char *fileName, char c);


/**
* @brief asks to master to accept file.
*
* @param tcpClntSock an integer specifying TCP sleave socket.
* @param *fileName a string with fileName to send.
* @param *name a string containing user name
*
* @return 0 if no errors occours, -1 otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int askToSendFile (const int tcpClntSock, const char *fileName, const char *name);


/**
* @brief sends file to master.
*
* @param tcpClntSock an integer specifying TCP sleave socket.
* @param *fileName a string with fileName to send.
*
* @return 0 if no errors occours, -1 otherwise.
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
int sendFile (const int tcpClntSock, const char *fileName);

#endif 