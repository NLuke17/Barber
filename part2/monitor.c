#include <stdio.h>
#include <semaphore.h>
#include "monitor.h"

static sem_t mutex;
static CV stylistAvailable;
static CV customerAvailable;

static int waiting_chairs = 0;
static int total_haircuts = 0;
static int salon_full_count = 0;
static int salon_empty_count = 0;

#define MAX_CHAIRS 6

void cv_init(CV *cv)
{
    cv->count = 0;
    sem_init(&cv->sem, 0, 0);
}

void cv_wait(CV *cv)
{
    cv->count++;
    sem_post(&mutex);   // Signal-and-Continue: release monitor
    sem_wait(&cv->sem); // Wait for signal
    sem_wait(&mutex);   // Re-enter monitor
}

void cv_signal(CV *cv)
{
    if (cv->count > 0)
    {
        cv->count--;
        sem_post(&cv->sem);
    }
}

void mon_init()
{
    sem_init(&mutex, 0, 1);
    cv_init(&stylistAvailable);
    cv_init(&customerAvailable);
}

void mon_debugPrint()
{
    // Standard format per your request
    printf("|");
    for (int i = 0; i < MAX_CHAIRS; i++)
    {
        if (i < waiting_chairs)
            printf("1|");
        else
            printf("0|");
    }
    printf(" => %d\n", waiting_chairs);
    printf("Given haircuts = %d\n", total_haircuts);
    printf("Salon full = %d times\n", salon_full_count);
    printf("Salon empty = %d times\n", salon_empty_count);
}

void mon_checkCustomer()
{
    sem_wait(&mutex);

    if (waiting_chairs == 0)
    {
        salon_empty_count++;
        // The expected trace shows a print when the stylist checks and finds it empty
        mon_debugPrint();
    }

    while (waiting_chairs == 0)
    {
        cv_wait(&customerAvailable);
        // If we wake up, we might need to increment empty count again if we were
        // waiting a long time, but usually, waking up means a customer is here.
    }

    waiting_chairs--;
    total_haircuts++; // Move haircut increment here
    cv_signal(&stylistAvailable);

    mon_debugPrint();
    sem_post(&mutex);
}

int mon_checkStylist()
{
    sem_wait(&mutex);

    if (waiting_chairs < MAX_CHAIRS)
    {
        waiting_chairs++;
        cv_signal(&customerAvailable);

        mon_debugPrint(); // Arrival print
        cv_wait(&stylistAvailable);

        // Remove total_haircuts++ from here
        // Remove the second mon_debugPrint() from here

        sem_post(&mutex);
        return 1;
    }
    else
    {
        // Salon is full
        salon_full_count++;
        mon_debugPrint();
        sem_post(&mutex);
        return 0;
    }
}