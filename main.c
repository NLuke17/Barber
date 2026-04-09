#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CHAIRS 6
#define NUM_CUSTOMERS 75
#define STYLIST_SPEED 200000 // Time to cut hair
#define CUSTOMER_ARRIVAL_GAP 50000
#define SHOPPING_DELAY 500000 // Time a customer shops before returning

sem_t mutex;         // Protects the 'waiting' count
sem_t stylistReady;  // Stylist waits for customer
sem_t customerReady; // Customer waits for stylist
sem_t cuttingDone;   // Customer waits for cut to finish

int waiting = 0;
int total_haircuts = 0;

void *stylist_func(void *arg)
{
    while (1)
    {
        // Wait for a customer to wake you up
        sem_wait(&customerReady);

        sem_wait(&mutex);
        waiting--; // Customer leaves waiting chair to enter cutting chair
        printf("[STYLIST]: Taking a customer. Chairs occupied: %d\n", waiting);
        sem_post(&stylistReady); // Tell the customer to come to the chair
        sem_post(&mutex);

        // Simulate the haircut
        usleep(STYLIST_SPEED);

        sem_wait(&mutex);
        total_haircuts++;
        printf("[STYLIST]: Finished haircut #%d.\n", total_haircuts);
        sem_post(&cuttingDone); // Signal the customer they can leave

        if (total_haircuts == NUM_CUSTOMERS)
        {
            sem_post(&mutex);
            break;
        }
        sem_post(&mutex);
    }
    printf("--- Stylist has finished for the day. ---\n");
    return NULL;
}

void *customer_func(void *arg)
{
    int id = *(int *)arg;
    int has_haircut = 0;

    while (!has_haircut)
    {
        sem_wait(&mutex);

        if (waiting < CHAIRS)
        {
            waiting++;
            printf("Customer %2d: Entered salon. Waiting chairs: %d/%d\n", id, waiting, CHAIRS);

            sem_post(&customerReady); // Wake stylist
            sem_post(&mutex);

            sem_wait(&stylistReady); // Wait for stylist to call you
            printf("Customer %2d: Getting hair cut...\n", id);

            sem_wait(&cuttingDone); // Wait for the stylist to finish
            printf("Customer %2d: Finished and leaving salon.\n", id);
            has_haircut = 1;
        }
        else
        {
            // Salon is full
            printf("Customer %2d: Salon full. Going shopping...\n", id);
            sem_post(&mutex);
            usleep(SHOPPING_DELAY); // Wait before trying again
        }
    }
    return NULL;
}

int main()
{
    pthread_t stylistThread;
    pthread_t customerThreads[NUM_CUSTOMERS];
    int ids[NUM_CUSTOMERS];

    sem_init(&mutex, 1, 1);
    sem_init(&stylistReady, 1, 0);
    sem_init(&customerReady, 1, 0);
    sem_init(&cuttingDone, 1, 0);

    pthread_create(&stylistThread, NULL, stylist_func, NULL);

    for (int i = 0; i < NUM_CUSTOMERS; i++)
    {
        ids[i] = i + 1;
        pthread_create(&customerThreads[i], NULL, customer_func, &ids[i]);
        // Slight delay between arrivals to see the "build up"
        usleep(CUSTOMER_ARRIVAL_GAP);
    }

    for (int i = 0; i < NUM_CUSTOMERS; i++)
    {
        pthread_join(customerThreads[i], NULL);
    }

    // Join stylist last
    pthread_join(stylistThread, NULL);

    sem_destroy(&mutex);
    sem_destroy(&stylistReady);
    sem_destroy(&customerReady);
    sem_destroy(&cuttingDone);

    printf("Main: All 75 customers have been served. Program exiting.\n");
    return 0;
}