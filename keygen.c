#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
    srand(time(NULL)); // Time is seeded
    if (argc != 2) {
        printf("An Error Occured! Not Enough Parameters.\n");
        exit(0);
    }
    int keySize;
    char randomLetter;
    keySize = atoi(argv[1]);    // Keysize is converted to integer
    char key[keySize+1];    // char key array is initialized using keySize
    int i;
    for(i = 0; i < keySize; i++){   // For loop is used to calculate keySize
        randomLetter = " ABCDEFGHIJKLMNOPQRSTUVWXYZ"[rand()%27];
        key[i] = randomLetter;  // Random letter is taken and added to key
    }
    key[keySize] = '\0';    // Key adds '\0' to the end
    printf("%s\n", key);    // Key is printed
return 0;
}
