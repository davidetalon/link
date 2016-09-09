/**
* @file link.c
* @brief application for sending files over Wifi connection
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/

#include <stdlib.h>
#include "linkutils.h"
#include "master.h"
#include "sleave.h"
#include "argp.h"




const char *argp_program_version =
  "Link 1.0";

/* Program documentation. */
static char doc[] =
  "Argp example #3 -- a program with options and arguments using argp";

/* A description of the arguments we accept. */
static char args_doc[] = "ARG1 ARG2";

/* The options we understand. */
static struct argp_option options[] = {
  {"verbose",  'v', 0,      0,  "Produce verbose output" },
  {"listen",    'l', 0,      0,  "Don't produce any output" },
  {"send",   's', "<FILENAME>",      0, 	"Silent mode"},
  {"setname",   'n', "<NEWNAME>", 0,	"Output to FILE instead of standard output" },
  {"getname",   'g', 0,      0, 	"Silent mode"},
  { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
  char *args[2];                /* arg1 & arg2 */
  int listen, verbose, getName;
  char *sendFile;
  char *newName;
};


/**
* @brief parse arguments from command line.
*
* @param key an integer specifying option 
* @param *arg a pointer to option argument
* @param *state a pointer to parsing state (used by argp)
* @return a static_error, 0 if no errors occours, ARGP_KEY_END if all arguments have been parsed
*	ARGP_KEY_ARG a non-option argument, ARGP_ERR_UNKNOWN if current fey is not recognized
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'l':
      arguments->listen = 1;
      break;
    case 'v':
      arguments->verbose = 1;
      break;
    case 'g':
      arguments->getName = 1;
      break;
    case 'n':
      arguments->newName = arg;
      break;
    case 's':
      arguments->sendFile = arg;
      break;

    case ARGP_KEY_ARG:
      if (state->maxlen >= 2)
        /* Too many arguments. */
        argp_usage (state);

      arguments->args[state->maxlen] = arg;

      break;

    case ARGP_KEY_END:
      if (state->maxlen < 2)
        /* Not enough arguments. */
        argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };


/**
* @brief main for link applications, it allows to send and receive file over Wifi.
*
* @param argc a number of arguments passed by command line.
* @param **argv array of arguments.
*
* @return int, 0 if no error occours, -1 if there is a fatal error
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*/

int main (int argc, char **argv) {

	struct arguments arguments;

	/* Default values. */
	arguments.listen = 0;
	arguments.verbose = 0;
	arguments.getName = 0;
	arguments.sendFile = "-";
	arguments.newName = "-";


	/* Parse our arguments; every option seen by parse_opt will
	 *be reflected in arguments. */
	argp_parse (&argp, argc, argv, 0, 0, &arguments);

	printf ("VERBOSE = %d\nLISTEN = %d\nSEND_FILE = %s\nNEWNAME = %s\nGETNAME = %d\n", arguments.verbose, 
	  	arguments.listen, arguments.sendFile, arguments.newName, arguments.getName);

	char *bufferSettings = (char *) malloc(MAX_NAME_LENGTH);

	// initialize userName
	char userName[MAX_NAME_LENGTH];
	memset(&userName, 0, sizeof(userName));
	strcpy(userName, DEFAULT_USER_NAME);

	char *linkSettingsPath[MAX_SETTINGS_PATH];
	strcpy(linkSettingsPath, getenv("HOME"));
	strcat(linkSettingsPath, DEFAULT_SETTINGS_PATH);


	if ( exists(ABSOLUTE_DEFAULT_SETTINGS_PATH, 'f') == '1'){

		FILE *settingsPointerR = fopen(linkSettingsPath, "r");
		fread(bufferSettings, sizeof(bufferSettings), 1, settingsPointerR);
		
		strcpy(userName, bufferSettings);

		printf("READING: userName = %s, buffer = %s\n", userName, bufferSettings);

		close(settingsPointerR);

	} else {

		system(MAKE_SETTINGS_DIR);


		FILE *settingsPointerW = fopen(linkSettingsPath, "w");

		strcpy(bufferSettings, userName);
		printf("\nWRITING: buffer = %s, userName = %s\n", bufferSettings, userName);
		fwrite(bufferSettings, sizeof(bufferSettings) - 1, 1, settingsPointerW);
		// write(settingsPointerW, bufferSettings, sizeof(bufferSettings));

		close(settingsPointerW);

	}

	free(bufferSettings);

	//initialize fileName
	char fileName[MAX_FILE_NAME_SIZE];
	memset(&fileName, 0, sizeof(fileName));

	//setting user name;
	if (strcmp(arguments.newName, "-") != 0) {
	  	
	  	if (strlen(arguments.newName) <= MAX_NAME_LENGTH ) {
	  		
	  		memset(&userName, 0, sizeof(userName));
	  		strcpy(userName, arguments.newName);


		  	FILE *settingsPointerW = fopen(linkSettingsPath, "w");

			strcpy(bufferSettings, userName);
			printf("\nWRITING: buffer = %s, userName = %s\n", bufferSettings, userName);
			fwrite(bufferSettings, sizeof(bufferSettings) - 1, 1, settingsPointerW);
			// write(settingsPointerW, bufferSettings, sizeof(bufferSettings));

			close(settingsPointerW);


		  	printf("New username %s set!\n", userName);

	  	} else {
	  		printf("\nName must be max %d character\n", MAX_NAME_LENGTH);
	  		return -1;
	  	}

	}

	//get username
  	if (arguments.getName == 1) {

  		printf("Username:	%s\n", userName);
  		return 0;

  	}

	//NOT IMPLEMENTED
  	// if (arguments.verbose == 1) {
  	// // set verbose mode
  	// }


  	//send file
  	if (strcmp(arguments.sendFile, "-") != 0) {

  		//veryfing passed file name size
	  	if (strlen(arguments.sendFile) <= MAX_FILE_NAME_SIZE ) {

	  		//set fileName
	  		strcpy(fileName, arguments.sendFile);

	  	} else {
	  		printf("\nFile must be max %d character\n", MAX_FILE_NAME_SIZE );
	  		return -1;
	  	}


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


	if (arguments.listen == 1) {

		if (masterMode(userName) < 0 ){
			printf("Master mode failed.\n");
			return -1;
		}

	}

  	return 0;
}