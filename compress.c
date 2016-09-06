/**
* @compress.c
* @compress data using unix tar
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


const static int EXISTS_COMMAND_SIZE = 264;
const static int TAR_COMMAND_SIZE = 528;

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

int main (void) {

	char fileName[] = "Drone";

	//construction of command for shell
	char cDir = exists (fileName, 'd');
	char cFile = exists (fileName, 'f');

	//check if the file exists (the returned value from pipe)
	if (cDir == '1' || cFile == '1' ) {

		char tarCommand[TAR_COMMAND_SIZE];
		memset(&tarCommand, 0, sizeof(char) * TAR_COMMAND_SIZE);

		strcpy(tarCommand, "tar -zcf " );
		strcat(tarCommand, fileName);
		strcat(tarCommand, ".tar.gz ");
		strcat(tarCommand, fileName);
		system(tarCommand);

	} else {

		printf("File doesn't exist\n");

	}


	return 0;
}


