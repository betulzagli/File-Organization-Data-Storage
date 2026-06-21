#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "search.h"
#include "utils.h"
#include "compression.h"

#define TEST_SIZE 100
#define MAX_LINE 256

extern void computeMD5(const char* input, char* output);

char testPasswords[TEST_SIZE][MAX_LINE];

void loadRandomPasswords(const char* processedPath) {
    FILE* file = fopen(processedPath, "r");
    if (!file) return;

    char buffer[MAX_LINE];
    int totalLines = 0;
    while (fgets(buffer, MAX_LINE, file)) totalLines++;

    srand((unsigned int)time(NULL));
    rewind(file);

    int currentLine = 0, savedCount = 0;
    while (fgets(buffer, MAX_LINE, file) && savedCount < TEST_SIZE) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        if (rand() % totalLines < TEST_SIZE) {
            strcpy(testPasswords[savedCount++], buffer);
        }
        currentLine++;
    }
    rewind(file);
    while (savedCount < TEST_SIZE && fgets(buffer, MAX_LINE, file)) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        strcpy(testPasswords[savedCount++], buffer);
    }
    fclose(file);
}

void runPerformanceTests(const char* processedPath, const char* rawPath, const char* hashedPath, const char* compressedPath, const char* resultsPath) {
    printf("Loading 100 random passwords for benchmark...\n");
    loadRandomPasswords(processedPath);

    double rawTimes[TEST_SIZE], hashTimes[TEST_SIZE], compTimes[TEST_SIZE];
    double rawAvg = 0, hashAvg = 0, compAvg = 0;
    double rawMax = 0, hashMax = 0, compMax = 0;
    double rawRam = 0, hashRam = 0, compRam = 0;
    double rawCpu = 0, hashCpu = 0, compCpu = 0;

    char buffer[MAX_LINE], hashBuffer[33];
    clock_t start, end;

    // 1. Raw testi
    printf(" -> Running RAW search test...\n");
    startCPUTracking();
    for (int i = 0; i < TEST_SIZE; i++) {
        start = clock();
        FILE* f = fopen(rawPath, "r");
        while (fgets(buffer, MAX_LINE, f)) {
            buffer[strcspn(buffer, "\r\n")] = 0;
            if (strcmp(buffer, testPasswords[i]) == 0) break;
        }
        fclose(f);
        end = clock();
        rawTimes[i] = calculateTimeMS(start, end);
        rawAvg += rawTimes[i];
        if (rawTimes[i] > rawMax) rawMax = rawTimes[i];
        rawRam += getMemoryUsageMB(); // Dongu ici RAM ortalamasi
    }
    rawCpu = getAverageCPUUsage(); // Arama suresindeki CPU ortalamasi
    rawAvg /= TEST_SIZE;
    rawRam /= TEST_SIZE;

    // 2. Hashed Testi
    printf(" -> Running HASHED search test...\n");
    startCPUTracking();
    for (int i = 0; i < TEST_SIZE; i++) {
        computeMD5(testPasswords[i], hashBuffer);
        start = clock();
        FILE* f = fopen(hashedPath, "r");
        while (fgets(buffer, MAX_LINE, f)) {
            buffer[strcspn(buffer, "\r\n")] = 0;
            if (strcmp(buffer, hashBuffer) == 0) break;
        }
        fclose(f);
        end = clock();
        hashTimes[i] = calculateTimeMS(start, end);
        hashAvg += hashTimes[i];
        if (hashTimes[i] > hashMax) hashMax = hashTimes[i];
        hashRam += getMemoryUsageMB();
    }
    hashCpu = getAverageCPUUsage();
    hashAvg /= TEST_SIZE;
    hashRam /= TEST_SIZE;

    // 3. Compressed testi
    printf(" -> Running COMPRESSED Decompression test...\n");
    start = clock();
    decompressStorage(compressedPath, "Storage/Compressed/temp_unpacked.txt");
    end = clock();
    double decompTime = calculateTimeMS(start, end);

    printf(" -> Running COMPRESSED search test...\n");
    startCPUTracking();
    for (int i = 0; i < TEST_SIZE; i++) {
        start = clock();
        FILE* f = fopen("Storage/Compressed/temp_unpacked.txt", "r");
        while (fgets(buffer, MAX_LINE, f)) {
            buffer[strcspn(buffer, "\r\n")] = 0;
            if (strcmp(buffer, testPasswords[i]) == 0) break;
        }
        fclose(f);
        end = clock();
        compTimes[i] = calculateTimeMS(start, end);
        compAvg += compTimes[i];
        if (compTimes[i] > compMax) compMax = compTimes[i];
        compRam += getMemoryUsageMB();
    }
    compCpu = getAverageCPUUsage();
    compAvg /= TEST_SIZE;
    compRam /= TEST_SIZE;

    // Matematiksel hesaplar
    double rawStd = calculateStdDev(rawTimes, TEST_SIZE, rawAvg);
    double hashStd = calculateStdDev(hashTimes, TEST_SIZE, hashAvg);
    double compStd = calculateStdDev(compTimes, TEST_SIZE, compAvg);

    long rawSize = getFileSize(rawPath);
    long hashSize = getFileSize(hashedPath);
    long compSize = getFileSize(compressedPath);
    double compRatio = rawSize > 0 ? (double)compSize / rawSize : 0.0;

    // Sonuçlarý kaydetme ve yazma
    FILE* report = fopen(resultsPath, "w");
    if (!report) return;

#define PRINT_TABLE(...) \
        printf(__VA_ARGS__); \
        fprintf(report, __VA_ARGS__)

    PRINT_TABLE("\nPERFORMANCE BENCHMARK RESULTS\n\n");
    PRINT_TABLE("Method\t\tAvgTime(ms)\tMaxTime(ms)\tStdDev(ms)\tCPU(%%)\tRAM(MB)\tDisk(MB)\tDisk(Bytes)\n");
    PRINT_TABLE("RAW\t\t%.4f\t\t%.4f\t\t%.4f\t\t%.2f\t%.2f\t%.2f\t\t%ld\n", rawAvg, rawMax, rawStd, rawCpu, rawRam, rawSize / (1024.0 * 1024.0), rawSize);
    PRINT_TABLE("HASHED\t\t%.4f\t\t%.4f\t\t%.4f\t\t%.2f\t%.2f\t%.2f\t\t%ld\n", hashAvg, hashMax, hashStd, hashCpu, hashRam, hashSize / (1024.0 * 1024.0), hashSize);
    PRINT_TABLE("COMPRESSED\t%.4f\t\t%.4f\t\t%.4f\t\t%.2f\t%.2f\t%.2f\t\t%ld\n", compAvg, compMax, compStd, compCpu, compRam, compSize / (1024.0 * 1024.0), compSize);

    PRINT_TABLE("\nDecompression Time: %.4f ms\n", decompTime);
    PRINT_TABLE("Compression Ratio: %.4f\n", compRatio);

    fclose(report);
    printf("\nResults successfully saved to %s\n", resultsPath);
}