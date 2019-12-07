#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
    srand(time(NULL));
    if (argc != 2) {
        printf("An Error Occured! Not Enough Parameters.\n");
        exit(0);
    }
    int keySize;
    char randomLetter;
    keySize = atoi(argv[1]);
    char key[keySize+1];
    int i;
    for(i = 0; i < keySize; i++){
        randomLetter = " ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rand()%27];
        key[i] = randomLetter;
    }
    key[keySize] = '\0';
    printf("%s\n", key);
return 0;
}
