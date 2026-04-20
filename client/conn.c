#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 25565

static int sock;

void send_byte(char b) {

	send(sock, &b, 1, 0); // remember htonl((uint32_t) value) for ints!
}

char read_byte() {
	
	char b;
	read(sock, &b, 1);
	return b;
}

// // initialize server connection
// sock = socket(AF_INET, SOCK_STREAM, 0);
// if (sock < 0) {
// 	log_error("Could not create socket");
// 	return 1;
// }

// struct sockaddr_in serv_addr;
// serv_addr.sin_family = AF_INET;
// serv_addr.sin_port = htons(SERVER_PORT);

// if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
// 	log_error("Invalid address");
// 	return 1;
// }

// if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
// 	log_error("Could not connect to server");
// 	return 1;
// }

// close(sock);