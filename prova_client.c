/**
* @prova_client.c
* @brief open a TCP socket to Link master
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

int main (void) {

/**IDEA
* Creo un socket UDP su una porta random, invio un messaggio in broadcast e attendo la risposta del serve
* contenente ip, porta e nome. Li elenco ed aspetto che l'utente scelga il master di destinazione.
* Una volta scelto il master apro un socket TCP con il master per inviare i dati.
*/



/** 
*	Link application settings
*/
	//used port for application
	int client_port = 9092;
	int server_port = 9091;


/** 
*	Defining server settings and used variables 
*/

	//server socket address
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	server_address.sin_addr.s_addr = inet_addr("192.168.1.54");



/** 
*	Defining client settings and used variables 
*/
	

	//client socket address
	struct sockaddr_in client_address;
	memset(&client_address, 0, sizeof(client_address));

	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(client_port);
	client_address.sin_addr.s_addr = inet_addr("192.168.1.55");
	

	//creating server socket
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
		perror("Socket not created");
		// exit(1);
	}

	//client buffer
	char buffer[256];
	int buffer_size = sizeof(buffer);


	if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
      perror("Cannot connect to server");
      // exit(1);
   } else {
   		// printf("Connection enstabilised\n");
   		// send(client_socket,buffer, sizeof(buffer), 0);
   		// printf("Message sent\n");
   		do {

   			scanf("%256s", buffer);
   			send(client_socket, buffer, sizeof(buffer), 0);
   		} while (1==1);
   }

   	close(client_socket);
	return 0;
}
