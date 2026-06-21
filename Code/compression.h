#ifndef COMPRESSION_H
#define COMPRESSION_H

void saveCompressedStorage(const char* processedPath, const char* compressedPath);
void decompressStorage(const char* compressedPath, const char* decompressedPath);

#endif