#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsWritten;
	socklen_t sizeOfClientInfo;
	int status;
	pid_t pid;
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");

	while (1){
		listen(listenSocketFD, 12); // Flip the socket on - it can now receive up to 12 connections
			// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");
		pid = fork();	// Proces is forked
		switch (pid){
			case 0:{	// Child function
				int charsRead;	// Int charsRead declared
				char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; // Chars arrat is declared
				char bufferKey[11];	// Char array key is declared
				char bufferMess[11];	// Char array key is declared
				char message[100000];	// Char array message is declared
				char key[100000];	// Char array key is declared
				char ciphertext[100000];// Char array ciphertext is declared
				memset(message, '\0', 100000);	// Char array message is cleared
				memset(key, '\0', 100000);	// Char key message is cleared
				memset(ciphertext, '\0', 100000);	// Char array ciphertext is cleared

				recv(establishedConnectionFD, bufferMess, 10 , 0);// bufferMess is recieved

/*
Characters '*' are removed from input message.  for example,
if the message 555******, the stars are removed and 555 is created.
*/
				int k = 10;
				while (bufferMess[k-1] == '*')
				{
					bufferMess[k-1] = '\0';
					k--;

				}
				int message_Length = atoi(bufferMess); // Input is converted to integer
				recv(establishedConnectionFD, bufferKey, 10 , 0);	// Key size is received

/*
Characters '*' are removed from input message.  for example,
if the message 555******, the stars are removed and 555 is created.
*/
				int j = 10;
				while (bufferKey[j-1] == '*')
				{
					bufferKey[j-1] = '\0';
					j--;

				}
				int key_Length = atoi(bufferKey); // Input is converted to integer


				charsRead = recv(establishedConnectionFD, message, message_Length+1, MSG_WAITALL); // Read the client's message from the socket
				if (charsRead < 0) error("ERROR reading from socket");
				charsRead = recv(establishedConnectionFD, key, key_Length+1, MSG_WAITALL); // Read the client's message from the socket
				if (charsRead < 0) error("ERROR reading from socket");


				int numOne;
				int numTwo;
				int finNum;
					int i;
					for(i = 0; message[i] != '\n'; i++){
						// Character position for letter is found and set ad numOne
						int x;
						for (x = 0; x < 27; x++){
							if (message[i] == characters[x]){
									numOne = x;
							}
						}
						for (x = 0; x < 27; x++){
						// Character position for letter is found and set ad numTwo
							if (key[i] == characters[x]){
									numTwo = x;
							}
						}
						// finNum is calculated and encrypted
						finNum = numOne + numTwo;
						finNum = finNum % 27;

						// Message char array is declared and initialized to the  resulting encrypted message.
						ciphertext[i] = characters[finNum];
					}

				int cipherLen = strlen(ciphertext); // Length of ciphertext is found
				charsWritten = send(establishedConnectionFD, ciphertext, cipherLen+1, MSG_WAITALL); // ciphertext is sent to client
				if (charsWritten < 0) error("CLIENT: ERROR writing to socket"); // Check for errors
				if (charsWritten < strlen(ciphertext)) printf("CLIENT: WARNING: Not all data written to socket!\n"); // Check for errors

			exit(0);
			}
			default:{
				pid_t actialpid = waitpid(pid, &status, WNOHANG);	// PArent PID Wait
			}
		}

		close(establishedConnectionFD); // Close the existing socket which is connected to the client


	}
	close(listenSocketFD);
	return 0;
}
