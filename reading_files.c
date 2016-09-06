/**
* @reading_files.c
* @connect master to sleave
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main (int argc, char *argv[]) {

	if(argc == 2) {
		printf("The argoment is: %s\n", argv[1]);
	}

	char buffer[20];
	memset(&buffer, 0, sizeof(buffer));

	FILE 	*fpi, *fpo;

		fpi = fopen("Drone.tar.gz", "r");
		// fpi = fopen("prova.txt", "r");
		fpo = fopen("CopiaDrone.tar.gz", "w");

		fseek(fpi, 0, SEEK_SET);
		while (!feof(fpi)) {
			fread(buffer, 20, 1, fpi);
			// printf("%s", buffer);
			fwrite(buffer, 20, 1, fpo);
		}

   
		fclose(fpo);
		fclose(fpi);

	

	return 0;

}