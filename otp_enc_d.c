#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); }

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsWritten;
	socklen_t sizeOfClientInfo;
	int status;
	pid_t pid;
	struct sockaddr_in serverAddress, clientAddress;
	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } 
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); 
	portNumber = atoi(argv[1]); 
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(portNumber); 
	serverAddress.sin_addr.s_addr = INADDR_ANY; 
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) 
		error("ERROR on binding");

	while (1){
		listen(listenSocketFD, 12); 
		sizeOfClientInfo = sizeof(clientAddress); 
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); 
		if (establishedConnectionFD < 0) error("ERROR on accept");
		pid = fork();
		switch (pid){
			case 0:{	
				int charsRead;	
				char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; 
				char bufferKey[11];	
				char bufferMess[11];	
				char message[100000];	
				char key[100000];	
				char ciphertext[100000];
				memset(message, '\0', 100000);	
				memset(key, '\0', 100000);	
				memset(ciphertext, '\0', 100000);
				recv(establishedConnectionFD, bufferMess, 10 , 0);
				int k = 10;
				while (bufferMess[k-1] == '*'){
					bufferMess[k-1] = '\0';
					k--;

				}
				int message_Length = atoi(bufferMess); 
				recv(establishedConnectionFD, bufferKey, 10 , 0);

				int j = 10;
				while (bufferKey[j-1] == '*'){
					bufferKey[j-1] = '\0';
					j--;

				}
				int key_Length = atoi(bufferKey); 
				charsRead = recv(establishedConnectionFD, message, message_Length+1, MSG_WAITALL);
				if (charsRead < 0) error("ERROR reading from socket");
				charsRead = recv(establishedConnectionFD, key, key_Length+1, MSG_WAITALL);
				if (charsRead < 0) error("ERROR reading from socket");
				int numOne;
				int numTwo;
				int finNum;
					int i;
					for(i = 0; message[i] != '\n'; i++){
						int x;
						for (x = 0; x < 27; x++){
							if (message[i] == characters[x]){
									numOne = x;
							}
						}
						for (x = 0; x < 27; x++){
	
							if (key[i] == characters[x]){
									numTwo = x;
							}
						}
		
						finNum = numOne + numTwo;
						finNum = finNum % 27;
						ciphertext[i] = characters[finNum];
					}
				int cipherLen = strlen(ciphertext); 
				charsWritten = send(establishedConnectionFD, ciphertext, cipherLen+1, MSG_WAITALL); 
				if (charsWritten < 0) error("CLIENT: ERROR writing to socket"); 
				if (charsWritten < strlen(ciphertext)) printf("CLIENT: WARNING: Not all data written to socket!\n"); // Check for errors
				exit(0);
			}
			default:{
				pid_t actialpid = waitpid(pid, &status, WNOHANG);
			}
		}
		close(establishedConnectionFD); 
	}
	close(listenSocketFD);
	return 0;
}
