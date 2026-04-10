#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "monitor.h"

#define NUM_CUSTOMERS 75
#define DELAY 100000

void *stylist(void *arg)
{
    while (1)
    {
        mon_checkCustomer();
        usleep(DELAY); // Cutting hair
    }
    return NULL;
}

void *customer(void *arg)
{
    int id = *(int *)arg;
    (void)id;
    int done = 0;
    while (!done)
    {
        if (mon_checkStylist())
        {
            done = 1;
        }
        else
        {
            usleep(DELAY * 3); // Full, go shop
        }
    }
    return NULL;
}

int main()
{
    pthread_t stylist_tid;
    pthread_t customers[NUM_CUSTOMERS];
    int ids[NUM_CUSTOMERS];

    mon_init();

    pthread_create(&stylist_tid, NULL, stylist, NULL);

    for (int i = 0; i < NUM_CUSTOMERS; i++)
    {
        ids[i] = i;
        pthread_create(&customers[i], NULL, customer, &ids[i]);
        usleep(DELAY * 2); // Control arrival rate
    }

    for (int i = 0; i < NUM_CUSTOMERS; i++)
    {
        pthread_join(customers[i], NULL);
    }

    return 0;
}