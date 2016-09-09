/**
* @linkutils.h
* @utils for link applicatin
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/

#ifndef LINKUTILS_H_INCLUDED   /* Include guard */
#define LINKUTILS_H_INCLUDED


#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

static const char DEFAULT_USER_NAME[] = "Unknow";

/************* APPLICATION OPTION *************/


/**
*
*	Ports used in link application
*
*/

//UDP port esed sleave side
static const int UDP_CLIENT_PORT = 1234;

//UDP port esed master side
static const int UDP_SERVER_PORT = 1235;

//TCP port used sleave side
static const int TCP_CLIENT_PORT = 2345;

//TCP port used master side
static const int TCP_SERVER_PORT = 2346;



/**
*
*	Buffer size used in link application
*
*/

//size of service buffer used for information transmission
static const int SERVICE_BUFFER_SIZE = 256;

//size of data buffer used for send file
static const int DATA_BUFFER_SIZE = 4096;

//size of header used for ask to master to accept
static const int HEADER_SIZE = 336;



/**
*
*	service communication between master and sleave
*
*/

//server avaiable valid response
static const char VALID_SERVER_ON[15] = "LINKAPP/SRVON/";

//ask for avaiable servers
static const char VALID_CLIENT_REQUEST[23] = "LINKAPP/CLNTRQT/SRVON?/";

//file transmission accepted
static const char SEND_ACCEPTED[23] =        "LINKAPP/SEND/ACCEPTED/";

//file transmission not accepted
static const char SEND_NOT_ACCEPTED[26] = "LINKAPP/SEND/NOTACCEPTED/";



//UNIX max file name size (extension included)
static const int MAX_FILE_NAME_SIZE = 255;

//max user name size for link application
static const int MAX_NAME_LENGTH = 50;

//variables for searchig avaiable masters
static const int SERVER_SEARCH_TIME = 1;
static const int MAX_NUMBER_SERVERS = 8;



/**
*
*	system call command size used for compression
*
*/

//exists command size
const static int EXISTS_COMMAND_SIZE = 264;

//tar command size
const static int TAR_COMMAND_SIZE = 528;

//remove command size
const static int REMOVE_COMMAND_SIZE = 268;



/**
*
*	server structure for array of server
*
*/

//server structure
struct srv {
	char name[MAX_NAME_LENGTH];
	struct sockaddr_in sockAddr;
	int sockAddrLen;
};

typedef struct srv srv;


#endif 