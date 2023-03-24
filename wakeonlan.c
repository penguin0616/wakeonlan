#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

const size_t MAGIC_PACKET_SIZE = 102;
const size_t HEADER_SIZE = 6;

void createMagicPacket(char mac_address) {
	char* payload = malloc(MAGIC_PACKET_SIZE);
	
	memset(&payload, 0xFF, HEADER_SIZE);
	memset(&payload + HEADER_SIZE, 0, MAGIC_PACKET_SIZE - HEADER_SIZE);

	free(payload);
	payload = NULL;
}

bool validateMACAddress(char *address) {
	printf("mac address: %s\n", address);
	return false;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Missing MAC addresss.\n");
		return 1;
	}

	// printf("%s\n", argv[1]);

	if (!validateMACAddress(argv[1])) {
		printf("Invalid MAC address!\n");
		return 1;
	}

	return 0;
}