#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;	// Variables are declared
	struct sockaddr_in serverAddress;	// Struct sockaddr_in declared
	struct hostent* serverHostInfo;	// Struct hostent declared
	char ciphertext[100000];	// Ciphertextchar array declared
	char key[100000];	// Key char array declared

	if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	int yes = 1;
	setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

// File containing ciphertext is taken in as file pointer inCipher
    FILE *inCipher;
    char cCipher;
    int countCipher = 0;
    inCipher = fopen(argv[1], "r");	// File is opened with read attribute
    cCipher = fgetc(inCipher);	// Char is pulled
    while (cCipher != EOF){	// While loop initiates pulling in every character
        cCipher = fgetc(inCipher);
        countCipher++;	// Number of chars in file are identified.
    }
	fclose(inCipher); // File is closed

// File containing key is taken in as file pointer inKey
    FILE *inKey;
    char cKey;
    int countKey = 0;
    inKey = fopen(argv[2], "r");	// File is opened with read attribute
    cKey = fgetc(inKey);	// Char is pulled
    while (cKey != EOF){	// While loop initiates pulling in every character
        cKey = fgetc(inKey);	// While loop initiates pulling in every character
        countKey++;	// Number of chars in file are identified.
    }
	fclose(inKey);	// File is closed

// Key size is compared to ciphertext size
// Error is identified if the key is less than the cipher char size
	if (countKey < countCipher){
		error("The size of the key is too small.");
		exit(1);
	}

// Char number in cipher file is wrtten to file_Len char array
	char file_Len[11];
	sprintf(file_Len, "%d", countCipher);
// Char number in key file is wrtten to key_Len char array
	char key_Len[11];
	sprintf(key_Len, "%d", countKey);

/*
The file_Len and key_Len are then sent to the server by using a buffer of 10.
The buffer fills in the char array with the previous function and fills
the remaining parts of the char array with "*" (star) characters.
This is used to identify how many chars to allow the server t know how  many chars
to expect for the ciphertext and the key.
*/
	char src[] = "*";
	while (strlen(file_Len) < 10){
		strncat(file_Len, src, 1);
	}
	send(socketFD, file_Len, strlen(file_Len), 0);	// file_Len is sent to server

	char srcTwo[] = "*";
	while (strlen(key_Len) < 10){
		strncat(key_Len, srcTwo, 1);
	}
	send(socketFD, key_Len, strlen(key_Len), 0); // key_Len is sent to server


    inCipher = fopen(argv[1], "r");	// File is open with read attribute
	while(fgets(ciphertext, sizeof(ciphertext)-1, inCipher));	// chars in file are pulled and stored in ciphertext
    fclose(inCipher);	// File is closed
	int cipherLen = strlen(ciphertext);	// Char number is found using strlen function
	charsWritten = send(socketFD, ciphertext, cipherLen+1, MSG_WAITALL);	// Ciiphertext is sent to server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");	// Errors are checked
	if (charsWritten < strlen(ciphertext)) printf("CLIENT: WARNING: Not all data written to socket!\n");	// Errors are checked

    inKey = fopen(argv[2], "r");	// File is open with read attribute
	while(fgets(key, sizeof(key)-1, inKey));	// chars in file are pulled and stored in key
	fclose(inKey);	// File is closed
	int keyLen = strlen(key);	// Char number is found using strlen function
	charsWritten = send(socketFD, key, keyLen+1, MSG_WAITALL);	// key is sent to server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");	// Errors are checked
	if (charsWritten < strlen(key)) printf("CLIENT: WARNING: Not all data written to socket!\n");	// Errors are checked

	char message[cipherLen]; // char array message is initialized
	charsRead = recv(socketFD, message, cipherLen+1, MSG_WAITALL); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket"); // Errors are checked
	printf("%s\n", message); // Message is printed
	close(socketFD); // Socket is closed
	return 0;
}
