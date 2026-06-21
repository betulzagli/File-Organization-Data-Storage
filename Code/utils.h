#ifndef UTILS_H
#define UTILS_H
#include <time.h>

double calculateTimeMS(clock_t start, clock_t end);
long getFileSize(const char* filePath);
double calculateStdDev(double times[], int count, double avg);
double getMemoryUsageMB();

//  CPU ölçümü için baþlatma ve bitirme fonksiyonlari
void startCPUTracking();
double getAverageCPUUsage();

#endif