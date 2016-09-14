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


/* Program documentation. */
static char doc[] =
  "Link - send file over local network";

/* A description of the arguments we accept. */
static char args_doc[] = "ARG1 ARG2";

/* The options we understand. */
static struct argp_option options[] = {
  {"verbose",  'v', 0,      0,  "Produce verbose output" },
  {"listen",    'l', 0,      0,  "Start listening" },
  {"send",   's', "<FILENAME>",      0, 	"Send file"},
  {"setname",   'n', "<NEWNAME>", 0,	"Set user name" },
  {"getname",   'g', 0,      0, 	"Get user name"},
  { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments {
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

  	switch (key) {
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
      		if (state->maxlen >= 2) {
        		argp_usage (state);
        	}

      		arguments->args[state->maxlen] = arg;
      		break;

    	case ARGP_KEY_END:
      		if (state->maxlen < 2) {
     			argp_usage (state);
     		}
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

		// printf("READING: userName = %s, buffer = %s\n", userName, bufferSettings);

		close(settingsPointerR);

	} else {

		system(MAKE_SETTINGS_DIR);


		FILE *settingsPointerW = fopen(linkSettingsPath, "w");

		strcpy(bufferSettings, userName);
		// printf("\nWRITING: buffer = %s, userName = %s\n", bufferSettings, userName);
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

	  	if (sleaveMode(userName, fileName) < 0) {
	  		printf("Sleave mode failed.\n");
	  		return -1;
	  	}
	}


	if (arguments.listen == 1) {

		if (masterMode(userName) < 0 ){
			printf("Master mode failed.\n");
			return -1;
		}

	}

  	return 0;
}