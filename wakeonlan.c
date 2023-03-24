#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

const size_t MAGIC_PACKET_SIZE = 102;
const size_t HEADER_SIZE = 6;

char *createMagicPacket(char *mac) {
	size_t plen = sizeof(char) * MAGIC_PACKET_SIZE;

	char *payload = malloc(plen);

	if (payload == NULL) {
		printf("Unable to allocate payload\n");
		exit(1);
	}

	//printf("Payload size: %d\n", plen);
	//printf("addr: %s\n", mac);

	// Zero out the allocated memory
	memset(payload, (char)61, plen);

	// Create header
	memset(payload, 0xFF, HEADER_SIZE);

	// Populate with MAC address
	//printf("%d, %d\n", (char)*mac, (char)*(mac + 1));
	// printf("heck: %d\n", strlen(mac));

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
	// printf("Payload: %d\n", *(payload+102));

	return payload;
}

bool validateMACAddress(char *mac) {
	size_t len = strlen(mac);

	if (len != 12) {
		// 12 characters for the characters, 17 with the dashes
		return false;
	}

	// TODO: Proper validation at some point.

	return true;
}

// 0, 7, 9

#define SERVERPORT 9

// https://beej.us/guide/bgnet/examples/broadcaster.c
// "We’ll call this program broadcaster.c"
void sendWakeUpToTarget(char *ip, char *frame) {
    int sockfd;
    struct sockaddr_in their_addr; // connector's address information
    struct hostent *he;
    int numbytes;
    int broadcast = 1;
    // char broadcast = '1'; // if that doesn't work, try this

    if ((he = gethostbyname(ip)) == NULL) { // get the host info
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // this call is what allows broadcast packets to be sent:
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
                   sizeof broadcast) == -1) {
        perror("setsockopt (SO_BROADCAST)");
        exit(1);
    }

    their_addr.sin_family = AF_INET;         // host byte order
    their_addr.sin_port = htons(SERVERPORT); // short, network byte order
    their_addr.sin_addr = *((struct in_addr *)he->h_addr_list[0]);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    if ((numbytes = sendto(sockfd, frame, MAGIC_PACKET_SIZE, 0,
                           (struct sockaddr *)&their_addr, sizeof their_addr)) == -1) {
        perror("sendto");
        exit(1);
    }

    printf("sent %d bytes to %s\n", numbytes,
           inet_ntoa(their_addr.sin_addr));

    close(sockfd);
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Missing IP & MAC addresses.\nUsage: ./wakeonlan <broadcast_ip> <mac_address>\n");
		return 1;
	}

	if (!validateMACAddress(argv[2])) {
		printf("Invalid MAC address!\n");
		return 1;
	}

    printf("Broadcast Address: %s\n", argv[1]);
    printf("MAC Address: %s\n", argv[2]);

    // Create Magic Packet
	char *frame = createMagicPacket(argv[2]);

	// Write to File
	FILE *file = fopen("test.txt", "wb");

	if (file == NULL) {
		printf("Could not open test file.\n");
		return 1;
	}

	fwrite(frame, MAGIC_PACKET_SIZE, 1, file);
	printf("Wrote to file.\n");

	fclose(file);
	file = NULL;

    sendWakeUpToTarget(argv[1], frame);

    return 0;
}
