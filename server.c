/**
* @file server.c
* @brief starts the server
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/



#include <stdio.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/socket.h>


/**
* @function deamon 
* @brief The deamon start the server for receiving connection requests
* @params VOID
* @return VOID
*/
void deamon() {
	if( int server_socket = socket(AF_INET, SOCK_STREAM, 0) <0 -1 ){
		printf("Impossible to create socket");
		exit(1);
	} 

	//Setting used port;
	int port = 1234;

	//preparing struct for sockaddress
	typedef struct sockaddr sockaddress;
	sockaddress server_sockaddress;

	//setting socket address
	server_sockaddress.sin_family = AF_INET;
	server_sockaddress.sin_port = htons(port);
	server_sockaddress.sin_addr.s_addr = htonl(INADDR_ANY);
	int address_len = sizeof(server_sockaddress);

	//adressing the socket with used protocol, port and IP
	if (bind(server_socket, (sockaddress *) &server_sockaddress, address_len) < 0){
		printf("Impossible to bind socket\n");
		exit(1);
	}

	// listening on the port with  backlog of 5
	if (listen(svc, 5) < 0) {
		println("Server cannot starts listening\n");
		exit(1);
	}

	for (;;) {
		if(request = accept(server_socket, (sockaddress *) &client_sockaddress, sizeof(client_sockaddress)) < 0) {
			printf("Impossible accept new request");
			exit(1);
		}
	}










// }
int main (void) {


	printf("**********  LINK APPLICATION  **********\n\n");

	// pthread_t  server; 
	// pthread_create(&server,NULL,deamon);
	printf("Service is running.");
	printf("\n\nSelect from the menu.\n");

	printf("A. Send a File\n");
	printf("B. Settings\n");
	printf("C. Credits\n");
	printf("Q. Quit\n");

	printf("Link, multi-platforms communication.\n");

	char choose;
	choose = toupper(getchar());
	while (choose != 'Q'){
		switch (choose){
			case 'A':
				printf("Send a file\n");
				// sendFile();
				break;
			case 'B':
				printf("Settings menu\n");
				// settings();
				break;
			case 'C':
				printf("\n\n**********************************************************\n\nLINK APPLICATION\nAuthor Davide Talon (<talon.davide@gmail.com>) \nUniversity of Padua.\nVersion 1.0\nCopyright (c) 2016-2017\nCopyright Apache License, Version 2.0\n\n**********************************************************\n");
				break;
			default:
				printf("Please, select an option.\n");
				break;
		}
		getchar();
	}

	return 0;
}