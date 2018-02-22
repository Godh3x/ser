#include <stdio.h>

char S[256];
int i, j;
 
void swap(int i, int j) {
    char tmp = S[i]; S[i] = S[j]; S[j] = tmp;
}

/* Key-Scheduling Algorithm (KSA) */
void ksa(char *key, int key_len) {
    for (i = 0; i < 256; i++)
        S[i] = i;
 
    for (i = j = 0; i < 256; i++) {
        j = (j + key[i % key_len] + S[i]) & 255;
        swap(i, j);
    }
 
    i = j = 0;
}

/* Pseudo-Random Generation Algorithm (PRGA) */
unsigned char prga() {
    i = (i + 1) & 255;
    j = (j + S[i]) & 255;
 
    swap(i, j);
 
    return S[(S[i] + S[j]) & 255];
}

int main(int argc, char *argv[]) {
    char *str = argv[1];

    ksa(argv[2], 5);
 
    while (*str)
        printf("%02X", *str++ ^ prga());

    printf("\n");
}
