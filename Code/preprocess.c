#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocess.h"

#define MAX_LINE 256
#define MAX_PASSWORDS 500000 // RAM sýnýrý

// qsort için string karþýlaþtýrma fonksiyonu
int comparePasswords(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

void preprocessData(const char* inputFilePath, const char* outputFilePath) {
    FILE* inFile = fopen(inputFilePath, "r");
    FILE* outFile = fopen(outputFilePath, "w");

    if (inFile == NULL || outFile == NULL) {
        printf("Error: RawData/ files not found.\n");
        return;
    }

    // Parolalarý RAM'e alma 
    char** passwords = (char**)malloc(MAX_PASSWORDS * sizeof(char*));
    for (int i = 0; i < MAX_PASSWORDS; i++) {
        passwords[i] = (char*)malloc(MAX_LINE * sizeof(char));
    }

    int count = 0;
    char buffer[MAX_LINE];

    // Veri okuma
    while (fgets(buffer, MAX_LINE, inFile) != NULL && count < MAX_PASSWORDS) {
        if (strlen(buffer) > 1) {
            buffer[strcspn(buffer, "\r\n")] = 0;
            strcpy(passwords[count], buffer);
            count++;
        }
    }

    // Alfabetik sýralama
    qsort(passwords, count, sizeof(char*), comparePasswords);

    // tekrarlarý atlayarak dosyaya yazma
    int uniqueCount = 0;
    for (int i = 0; i < count; i++) {
        if (i == 0 || strcmp(passwords[i], passwords[i - 1]) != 0) {
            fprintf(outFile, "%s\n", passwords[i]);
            uniqueCount++;
        }
    }

    printf("Preprocessing complete. %d unique passwords saved.\n", uniqueCount);

    // Bellek temizliði
    for (int i = 0; i < MAX_PASSWORDS; i++) {
        free(passwords[i]);
    }
    free(passwords);

    fclose(inFile);
    fclose(outFile);
}