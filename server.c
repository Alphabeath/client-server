#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/**
 * TCP Uses 2 types of sockets, the connection socket and the listen socket.
 * The Goal is to separate the connection phase from the data exchange phase.
 * */

int main(int argc, char *argv[]) {
	//puerto en el que inicia el server
	int SERVER_PORT = 8877;

	// dirección del socket usado por el server
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));  // rellena con 0 cada campo de server_adress
	server_address.sin_family = AF_INET;


	// htons: host to network short: transforms a value in host byte
	server_address.sin_port = htons(SERVER_PORT); // la funcion tons traduce el entero server_port del host al orden de bytes en red

	// htonl: host to network long: same as htons but to long
	server_address.sin_addr.s_addr = htonl(INADDR_ANY); // lo mismo que lo anterior pero en formato long

	// Crea el socket TCP, retorna negativo si la creacion falla
	int listen_sock;
	if ((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("could not create listen socket\n");
		return 1;
	}

	// bind it to listen to the incoming connections on the created server
	// address, will return -1 on error
	if ((bind(listen_sock, (struct sockaddr *)&server_address,sizeof(server_address))) < 0) {
		printf("could not bind socket\n");
		return 1;
	}

	int wait_size = 16;  // definimos la cantidad maxima de clientes en espera
	if (listen(listen_sock, wait_size) < 0) {
		printf("could not open socket for listening\n");
		return 1;
	}

	// direccion de socket utilizado para guardar la direccion del cliente
	struct sockaddr_in client_address;
	int client_address_len = 0;

	// run indefinitely
	while (true) {
		// Nuevo socket para transmitir los datos
		int sock;
		if ((sock =
		         accept(listen_sock, (struct sockaddr *)&client_address,
		                &client_address_len)) < 0) {
			printf("could not open a socket to accept data\n");
			return 1;
		}

		int n = 0;
		int len = 0, maxlen = 100;
		char buffer[maxlen];
		char *pbuffer = buffer;

		printf("client connected with ip address: %s\n",
		       inet_ntoa(client_address.sin_addr));

		// keep running as long as the client keeps the connection open
		while ((n = recv(sock, pbuffer, maxlen, 0)) > 0) {
			pbuffer += n;
			maxlen -= n;
			len += n;
			pbuffer[0] = '\0';
			printf("received: '%s'\n", buffer);

			// echo received content back
			send(sock, buffer, len, 0);
		}

		close(sock);
	}

	close(listen_sock);
	return 0;
}