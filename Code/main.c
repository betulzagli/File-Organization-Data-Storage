#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "preprocess.h"
#include "storage.h"
#include "compression.h"
#include "search.h"

int main() {
    printf("--- File Organization Project ---\n\n");

    const char* rawPath = "RawData/Pwdb_top-10000000.txt";
    const char* procPath = "Processed/cleaned.txt";

    const char* outRaw = "Storage/Raw/raw.txt";
    const char* outHash = "Storage/Hashed/hash.txt";
    const char* outComp = "Storage/Compressed/comp.txt";
    const char* resPath = "Results/report.txt";

    // 1. Veri ön iþleme
    printf("Step 1: Preprocessing data...\n");
    preprocessData(rawPath, procPath);

    // 2. Saklama yöntemleri
    printf("\nStep 2: Saving storage formats...\n");
    saveRawStorage(procPath, outRaw);
    saveHashedStorage(procPath, outHash);
    saveCompressedStorage(procPath, outComp);

    // 3. Performans Testi
    printf("\nStep 3: Running performance tests...\n");
    runPerformanceTests(procPath, outRaw, outHash, outComp, resPath);

    return 0;
}