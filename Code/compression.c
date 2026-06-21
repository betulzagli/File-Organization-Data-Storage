#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compression.h"

#define MAX_SYMBOLS 256

typedef struct HuffNode {
    unsigned char id;
    unsigned int freq;
    struct HuffNode* left, * right;
} HuffNode;

HuffNode* createNode(unsigned char id, unsigned int freq) {
    HuffNode* node = (HuffNode*)malloc(sizeof(HuffNode));
    if (node) { node->id = id; node->freq = freq; node->left = node->right = NULL; }
    return node;
}

void buildCodes(HuffNode* root, char* code, int top, char codeTable[MAX_SYMBOLS][MAX_SYMBOLS]) {
    if (root->left) { code[top] = '0'; buildCodes(root->left, code, top + 1, codeTable); }
    if (root->right) { code[top] = '1'; buildCodes(root->right, code, top + 1, codeTable); }
    if (!root->left && !root->right) { code[top] = '\0'; strcpy(codeTable[root->id], code); }
}

void freeTree(HuffNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

HuffNode* buildTreeFromFreq(unsigned int freq[]) {
    HuffNode* nodes[MAX_SYMBOLS * 2];
    int count = 0;
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] > 0) nodes[count++] = createNode((unsigned char)i, freq[i]);
    }
    if (count == 0) return NULL;

    while (count > 1) {
        int min1 = 0, min2 = 1;
        if (nodes[min1]->freq > nodes[min2]->freq) { int t = min1; min1 = min2; min2 = t; }
        for (int i = 2; i < count; i++) {
            if (nodes[i]->freq < nodes[min1]->freq) { min2 = min1; min1 = i; }
            else if (nodes[i]->freq < nodes[min2]->freq) { min2 = i; }
        }
        HuffNode* parent = createNode('$', nodes[min1]->freq + nodes[min2]->freq);
        parent->left = nodes[min1]; parent->right = nodes[min2];
        nodes[min1] = parent;
        for (int i = min2; i < count - 1; i++) nodes[i] = nodes[i + 1];
        count--;
    }
    return nodes[0];
}

// dosyayý sýkýþtýrma
void saveCompressedStorage(const char* processedPath, const char* compressedPath) {
    unsigned int freq[MAX_SYMBOLS] = { 0 };
    FILE* in = fopen(processedPath, "rb");
    if (!in) return;

    int c;
    while ((c = fgetc(in)) != EOF) freq[c]++;

    HuffNode* root = buildTreeFromFreq(freq);
    if (!root) { fclose(in); return; }

    char codeTable[MAX_SYMBOLS][MAX_SYMBOLS] = { {0} };
    char codeBuffer[MAX_SYMBOLS];
    buildCodes(root, codeBuffer, 0, codeTable);

    FILE* out = fopen(compressedPath, "wb");
    if (!out) { fclose(in); freeTree(root); return; }

    // Header olarak frekans tablosu
    fwrite(freq, sizeof(unsigned int), MAX_SYMBOLS, out);

    rewind(in);
    unsigned char bitBuf = 0;
    int bitCount = 0;
    while ((c = fgetc(in)) != EOF) {
        char* str = codeTable[c];
        for (int i = 0; str[i] != '\0'; i++) {
            bitBuf = (bitBuf << 1) | (str[i] - '0');
            bitCount++;
            if (bitCount == 8) { fputc(bitBuf, out); bitBuf = 0; bitCount = 0; }
        }
    }
    if (bitCount > 0) { bitBuf <<= (8 - bitCount); fputc(bitBuf, out); }

    printf("Compressed Storage saved (Huffman encoding).\n");
    fclose(in); fclose(out); freeTree(root);
}

// sýkýþtýrýlmýþ dosyayý geri açma
void decompressStorage(const char* compressedPath, const char* decompressedPath) {
    FILE* in = fopen(compressedPath, "rb");
    FILE* out = fopen(decompressedPath, "wb");
    if (!in || !out) return;

    unsigned int freq[MAX_SYMBOLS] = { 0 };
    fread(freq, sizeof(unsigned int), MAX_SYMBOLS, in); // Header'i oku

    HuffNode* root = buildTreeFromFreq(freq);
    if (!root) { fclose(in); fclose(out); return; }

    long totalChars = root->freq; 
    long decodedCount = 0;
    int c;
    HuffNode* curr = root;

    while ((c = fgetc(in)) != EOF && decodedCount < totalChars) {
        for (int i = 7; i >= 0 && decodedCount < totalChars; i--) {
            int bit = (c >> i) & 1;
            curr = bit ? curr->right : curr->left;
            if (!curr->left && !curr->right) {
                fputc(curr->id, out);
                curr = root;
                decodedCount++;
            }
        }
    }
    fclose(in); fclose(out); freeTree(root);
}