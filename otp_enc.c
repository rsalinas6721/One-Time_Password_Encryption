#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <ctype.h>

void error(const char *msg) { perror(msg); exit(0); }

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;	
	struct sockaddr_in serverAddress;	
	struct hostent* serverHostInfo;	
	char message[100000];	
	char key[100000];  

	if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); }

	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); 
	portNumber = atoi(argv[3]); 
	serverAddress.sin_family = AF_INET; 
	serverAddress.sin_port = htons(portNumber); 
	serverHostInfo = gethostbyname("localhost");
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); 

	socketFD = socket(AF_INET, SOCK_STREAM, 0); 
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	int yes = 1;
	setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) 
		error("CLIENT: ERROR connecting");

    FILE *inMess;
    char cMess;
    int countMess = 0;
    inMess = fopen(argv[1], "r");	
    
    fseek(inMess, 0L, SEEK_END);	
    long int size = ftell(inMess);	
    fseek(inMess, 0L, SEEK_SET);
    if (size == 0){
	printf("File is Empty\n");
	exit(1);
    }
    cMess = fgetc(inMess);
    while (cMess != EOF){	
	if ((!isupper(cMess)) && (cMess != ' ') && (cMess != '\n')){	
	    printf("Input contains bad characters.");
	    exit(1);
	}
        cMess = fgetc(inMess);
        countMess++;	
    }
	fclose(inMess);	

    FILE *inKey;
    char cKey;
    int countKey = 0;
    inKey = fopen(argv[2], "r");	
    cKey = fgetc(inKey);		
    while (cKey != EOF){
        cKey = fgetc(inKey);		
        countKey++;	
    }
	fclose(inKey);

	if (countKey < countMess){
		fprintf(stderr, "The size of the key is too small.");
		exit(1);
	}

	char file_Len[11];
	sprintf(file_Len, "%d", countMess);

	char key_Len[11];
	sprintf(key_Len, "%d", countKey);


	char src[] = "*";
	while (strlen(file_Len) < 10){
		strncat(file_Len, src, 1);
	}

	send(socketFD, file_Len, strlen(file_Len), 0);

	char srcTwo[] = "*";
	while (strlen(key_Len) < 10){
		strncat(key_Len, srcTwo, 1);
	}

	send(socketFD, key_Len, strlen(key_Len), 0);

    inMess = fopen(argv[1], "r");
	while(fgets(message, sizeof(message)-1, inMess));
    fclose(inMess);
	int messLen = strlen(message);
	charsWritten = send(socketFD, message, messLen+1, MSG_WAITALL);	
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(message)) printf("CLIENT: WARNING: Not all data written to socket!\n");


    inKey = fopen(argv[2], "r");
	while(fgets(key, sizeof(key)-1, inKey));
	fclose(inKey);	
	int keyLen = strlen(key);	
	charsWritten = send(socketFD, key, keyLen+1, MSG_WAITALL);	
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");	
	if (charsWritten < strlen(key)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	
	char ciphertext[messLen];	
	charsRead = recv(socketFD, ciphertext, messLen+1, MSG_WAITALL); 
	if (charsRead < 0) error("ERROR reading from socket");	

	printf("%s\n", ciphertext); // Message is printed
	close(socketFD);	// Socket is closed

	return 0;
}
