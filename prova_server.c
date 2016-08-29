#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
// #include <stdlib.h>

#include <errno.h>


int main (void) {	

/** 
*	Link application settings
*/
	//used port for application
	int client_port = 9092;
	int server_port = 9091;

/** 
*	Defining server settings and used variables 
*/

	//creating server socket
	int server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		printf("Socket not created");
		// exit(1);
	}

	//server socket address
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(server_port);
	server_address.sin_addr.s_addr = inet_addr("192.168.1.54");
	int server_address_length = sizeof(server_address);

	// used variables
	int recvMsgSize;
	int request;

	//server buffer
	char buffer[256];
	int receiver_buffer_size = sizeof(buffer);

/** 
*	Defining client settings and used variables 
*/
	
	//creating client socket
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(client_socket < 0) {
		printf("Client socket not found");
	}

	//client socket address
	struct sockaddr_in client_address;
	memset(&client_address, 0, sizeof(client_address));

	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(client_port);
	client_address.sin_addr.s_addr = inet_addr("192.168.1.55");

	int client_address_length = sizeof(client_address);

	//binding socket - addressing
	if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		perror("Socket not binded");
		// exit(1);
	} 


	//start listening with queue size of 5
	if(listen(server_socket, 5) < 0) {
		printf("Error starting listening\n");
		// exit(1);
	}


	printf("Starting deamon...\n");

	//accept new request
	for (;;) {

		request = accept(server_socket, (struct sockaddr *) &client_address, &client_address_length );
		if(request < 0) {
			perror("Connection not accepted: ");
			// printf("Impossible accept new request\n");
			// exit(1);
		} else {

			// receiving message
			while( recvMsgSize = read(request, buffer, sizeof(buffer)) > 0) {

				printf("\nRicevuto: %s", buffer);

			}
		}
		close(request);
		close(server_socket);
	}

	return 1;
}