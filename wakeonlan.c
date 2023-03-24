#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// 0, 7, 9
// 40000
#define WOL_PORT 9

#define MAGIC_PACKET_SIZE 102
#define HEADER_SIZE 6

const char *BROADCAST_IP = "255.255.255.255";

char *createMagicPacket(char *mac) {
	size_t plen = sizeof(char) * MAGIC_PACKET_SIZE;

	char *payload = malloc(plen);

	if (payload == NULL) {
		printf("Unable to allocate payload\n");
		exit(1);
	}

	// Zero out the allocated memory
	memset(payload, (char)61, plen);

	// Create header
	memset(payload, 0xFF, HEADER_SIZE);

	// Populate with MAC address
	char sub[3];
	for (int i = 0; i < 16; i++) {
		// 48 bit MAC address, 8 bits in a byte, 6 bytes.
		for (int j = 0; j < 6; j++) {
			memset(sub, 0, 3);
			memcpy(sub, &mac[j * 2], 2); // Multiply by two because... I don't know anymore.
			char num = (char)strtol(sub, NULL, 16);
			//printf("%x ", num);
			memset(payload + HEADER_SIZE + (i * 6) + j, num, sizeof(num));
		}
		//printf("---------\n");
	}

	//printf("Payload: %s\n", payload);

	return payload;
}

bool validateMACAddress(char *mac) {
	size_t len = strlen(mac);

	if (len != 12) {
		// 12 characters for the characters, 17 with the dashes
		return false;
	}

	// TODO: Proper validation when this hits production.

	return true;
}

// https://beej.us/guide/bgnet/examples/broadcaster.c
// "We’ll call this program broadcaster.c"
void sendWakeUpToTarget(char *frame) {
	int sockfd;
	struct sockaddr_in their_addr; // connector's address information
	struct hostent *he;
	int numbytes;
	int broadcast = 1;

	if ((he = gethostbyname(BROADCAST_IP)) == NULL) { // get the host info
		perror("gethostbyname");
        return;
    }

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		return;
	}

	// this call is what allows broadcast packets to be sent:
	if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
		perror("setsockopt (SO_BROADCAST)");
		goto sendWakeUptoTarget_socketfail;
	}

	their_addr.sin_family = AF_INET;         // host byte order
	their_addr.sin_port = htons(WOL_PORT); // short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

	if ((numbytes = sendto(sockfd, frame, MAGIC_PACKET_SIZE, 0, (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
		perror("sendto");
		goto sendWakeUptoTarget_socketfail;
	}

	printf("sent %d bytes to %s\n", numbytes, inet_ntoa(their_addr.sin_addr));

	sendWakeUptoTarget_socketfail:
		close(sockfd);
		sockfd = -1;
	
	return;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Missing MAC addresses.\nUsage: ./wakeonlan <mac_address>\n");
		return 1;
	}

	if (!validateMACAddress(argv[1])) {
		printf("Invalid MAC address!\n");
		return 1;
	}

	printf("MAC Address: %s\n", argv[1]);

	// Create Magic Packet
	char *frame = createMagicPacket(argv[1]);

	// Send Magic Packet
	sendWakeUpToTarget(frame);

	free(frame);
	frame = NULL;

	return 0;
}
