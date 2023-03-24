#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

const size_t MAGIC_PACKET_SIZE = 102;
const size_t HEADER_SIZE = 6;

char *createMagicPacket(char *address) {
    size_t plen = sizeof(char) * MAGIC_PACKET_SIZE;

	char *payload = malloc(plen);

    if (payload == NULL) {
		printf("Unable to allocate payload\n");
		exit(1);
	}

    printf("Payload size: %d\n", plen);
	printf("addr: %s\n", address);
    
	// Zero out the allocated memory
	memset(payload, (char) 61, plen);

	// Create header
	memset(payload, 0xFF, HEADER_SIZE);

    // Populate with MAC address
	printf("%d, %d\n", (char) *address, (char) *(address+1));
	//printf("heck: %d\n", strlen(address));

    char sub[3];
    for (int i = 0; i < 16; i++) {
		// 48 bit MAC address, 8 bits in a byte, 6 bytes.
		for (int j = 0; j < 6; j++) {
            memset(sub, 0, 3);
            memcpy(sub, &address[j * 2], 2); // Multiply by two because... I don't know anymore.
            char num = (char)strtol(sub, NULL, 16);
			printf("%x ", num);
            memset(payload + HEADER_SIZE + (i * 6) + j, num, sizeof(num));
        }
		printf("---------\n");
    }


	/*
	char sub[3];
	memset(sub, 0, 3);
	memcpy(sub, &address[0], 2);

    printf("%s\n", sub);

    char num = (char)strtol(sub, NULL, 16);

	printf("%x", num);

    memset(payload + HEADER_SIZE, num, sizeof(num));

    for (int i = 0; i < 8; i++) {
		for (int j = 0; j < strlen(address); j += 2) {
            //char num = ((char) *(address+j)) * 16 + ((char) *((address+j)+1));
			//char seg[] = {
			//char num = sscanf(str)
			//printf("%x ", num);
        }
		
		printf("-----\n");
        //memcpy(payload + HEADER_SIZE + (i * 12), address, 12);
    }
	*/

	printf("Payload: %s\n", payload);
    //printf("Payload: %d\n", *(payload+102));

	return payload;
}

bool validateMACAddress(char *address) {
	size_t len = strlen(address);

	if (len != 12) {
		// 12 characters for the characters, 17 with the dashes
		return false;
	}

	// TODO: Proper validation at some point.

	return true;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Missing MAC addresss.\n");
		return 1;
	}

	if (!validateMACAddress(argv[1])) {
		printf("Invalid MAC address!\n");
		return 1;
	}

	// Create Magic Packet
	printf("MAC Address: %s\n", argv[1]);

	char* frame = createMagicPacket(argv[1]);
	
	// Write to File
	FILE *file = fopen("test.txt", "wb");

    if (file == NULL) {
		printf("Could not open test file.\n");
		return 1;
	}

	//printf("|%s|\n", frame);

    fwrite(frame, MAGIC_PACKET_SIZE, 1, file);
	printf("Wrote to file.\n");

    fclose(file);

	return 0;
}