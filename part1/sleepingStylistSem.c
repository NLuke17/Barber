#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CHAIRS 6
#define NUM_CUSTOMERS 75
#define DELAY 1000

sem_t mutex;
sem_t stylistReady;
sem_t customerReady;

int waiting = 0;
int haircutsDone = 0;

void *stylist(void *arg)
{
    while (1)
    {
        // Stylist sleeps until a customer signals
        sem_wait(&customerReady);

        sem_wait(&mutex);
        // Stylist takes a customer from a chair
        waiting--;
        sem_post(&stylistReady);
        sem_post(&mutex);

        // Simulate the haircutting process
        usleep(DELAY);

        sem_wait(&mutex);
        haircutsDone++;

        // Exact output format requested
        printf("Hair cut received\n");
        printf("Number of haircuits done: %d\n", haircutsDone);
        printf("Number of people waiting: %d\n", waiting);

        if (haircutsDone == NUM_CUSTOMERS)
        {
            sem_post(&mutex);
            break;
        }
        sem_post(&mutex);
    }
    return NULL;
}

void *customer(void *arg)
{
    int id = *(int *)arg;

    while (1)
    {
        sem_wait(&mutex);

        if (waiting < CHAIRS)
        {
            waiting++;

            // Exact output format requested
            printf("Customer %d waiting\n", id);
            printf("Number of people waiting: %d\n", waiting);

            sem_post(&customerReady);
            sem_post(&mutex);

            sem_wait(&stylistReady);
            // Customer is now in the stylist's chair
            break;
        }
        else
        {
            // Optional: You can print if someone leaves,
            // but your requested output doesn't show rejections.
            sem_post(&mutex);
            usleep(DELAY * 2); // Go shopping and come back
        }
    }
    return NULL;
}

int main()
{
    pthread_t stylistThread;
    pthread_t customerThreads[NUM_CUSTOMERS];
    int ids[NUM_CUSTOMERS];

    sem_init(&mutex, 0, 1);
    sem_init(&stylistReady, 0, 0);
    sem_init(&customerReady, 0, 0);

    pthread_create(&stylistThread, NULL, stylist, NULL);

    for (int i = 0; i < NUM_CUSTOMERS; i++)
    {
        ids[i] = i; // Using 0-indexed IDs to match your sample
        pthread_create(&customerThreads[i], NULL, customer, &ids[i]);

        // Control arrival speed to see both build-up and empty states
        if (i % 5 == 0)
            usleep(DELAY * 2);
        else
            usleep(DELAY / 2);
    }

    for (int i = 0; i < NUM_CUSTOMERS; i++)
    {
        pthread_join(customerThreads[i], NULL);
    }

    pthread_join(stylistThread, NULL);

    sem_destroy(&mutex);
    sem_destroy(&stylistReady);
    sem_destroy(&customerReady);

    return 0;
}