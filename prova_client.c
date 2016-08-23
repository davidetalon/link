
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

int main (void) {


/** 
*	Link application settings
*/
	//used port for application
	int port = 9091;


/** 
*	Defining server settings and used variables 
*/

	//server socket address
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = inet_addr("192.168.1.54");



/** 
*	Defining client settings and used variables 
*/
	

	//client socket address
	struct sockaddr_in client_address;
	memset(&client_address, 0, sizeof(client_address));

	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(port);
	client_address.sin_addr.s_addr = inet_addr("192.168.1.55");
	

	//creating server socket
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
		perror("Socket not created");
		// exit(1);
	}

	//client buffer
	char buffer[256] = "AHAHAHAHHAHA";
	int buffer_size = sizeof(buffer);

	/*if (bind(client_socket, (struct sockaddr *) &client_address, sizeof(client_address)) < 0) {
		perror("Socket not binded");
		// exit(1);
	} */

	if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
      perror("Cannot connect to server");
      // exit(1);
   } else {
   		printf("Connection enstabilised\n");
   		send(client_socket,buffer, sizeof(buffer), 0);
   		printf("Message sent\n");
   }

   	close(client_socket);
	return 0;
}
