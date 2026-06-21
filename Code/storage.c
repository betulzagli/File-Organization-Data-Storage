#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <wincrypt.h>
#include "storage.h"

#define MAX_LINE 256


static HCRYPTPROV hProv = 0;

void computeMD5(const char* input, char* output) {
    // eðer motor henüz açýlmadýysa
    if (hProv == 0) {
        CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    }

    HCRYPTHASH hHash = 0;
    BYTE rgbHash[16];
    DWORD cbHash = 16;
    char hexDigits[] = "0123456789abcdef";

    CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
    CryptHashData(hHash, (BYTE*)input, (DWORD)strlen(input), 0);
    CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0);

    for (DWORD i = 0; i < cbHash; i++) {
        output[i * 2] = hexDigits[rgbHash[i] >> 4];
        output[i * 2 + 1] = hexDigits[rgbHash[i] & 0xf];
    }
    output[32] = '\0';

    CryptDestroyHash(hHash);
}

void saveRawStorage(const char* processedPath, const char* rawStoragePath) {
    FILE* inFile = fopen(processedPath, "r");
    FILE* outFile = fopen(rawStoragePath, "w");
    if (inFile == NULL || outFile == NULL) return;

    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, inFile) != NULL) {
        fprintf(outFile, "%s", buffer);
    }
    printf("Raw Storage saved.\n");
    fclose(inFile);
    fclose(outFile);
}

void saveHashedStorage(const char* processedPath, const char* hashedStoragePath) {
    FILE* inFile = fopen(processedPath, "r");
    FILE* outFile = fopen(hashedStoragePath, "w");
    if (inFile == NULL || outFile == NULL) return;

    char buffer[MAX_LINE];
    char hashOutput[33];
    int count = 0;

    while (fgets(buffer, MAX_LINE, inFile) != NULL) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        computeMD5(buffer, hashOutput);
        fprintf(outFile, "%s\n", hashOutput);

        count++;
        // iþlemin donduðunu sanmamak için her 100.000 adimda ekrana bilgi basýlýyor 
        if (count % 100000 == 0) {
            printf("   ... %d passwords hashed ...\n", count);
        }
    }
    printf("Hashed Storage saved (MD5 implemented).\n");
    fclose(inFile);
    fclose(outFile);
}