#ifndef MONITOR_H
#define MONITOR_H

#include <semaphore.h>

typedef struct
{
    int count;
    sem_t sem;
} CV;

void mon_init();
void mon_checkCustomer();
int mon_checkStylist();
void mon_debugPrint(); // Crucial for the requested output

#endif