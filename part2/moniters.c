#include <stdio.h>
#include <semaphore.h>

#define CHAIRS 6

typedef struct {
    int count;
    sem_t sem;
} CV;

sem_t entry_mutex;
CV stylistAvailable = {0};
CV customerAvailable = {0};
int customer_count = 0;
int stylist_sleeping = 0;
int haircuts = 0, salon_full = 0, salon_empty = 0;

void cv_wait(CV *cv) {
    cv->count++;
    sem_post(&entry_mutex); // Leave monitor
    sem_wait(&cv->sem);     // Block
    sem_wait(&entry_mutex); // Re-enter monitor
    cv->count--;
}

void cv_signal(CV *cv) {
    if (cv->count > 0) sem_post(&cv->sem); // Signal-and-Continue
}

void mon_debugPrint() {
    for (int i = 0; i < CHAIRS; i++) {
        printf("|%d|", (i < customer_count) ? 1 : 0);
    }
    printf(" => %d\n", customer_count);
    printf("Given haircuts = %d\n", haircuts);
    printf("Salon full = %d times\n", salon_full);
    printf("Salon empty = %d times\n", salon_empty);
}

void mon_checkCustomer() {
    sem_wait(&entry_mutex);
    cv_signal(&stylistAvailable); // Stylist ready [cite: 183]
    if (customer_count == 0) {
        salon_empty++;
        cv_wait(&customerAvailable); // Sleep [cite: 185]
    }
    customer_count--;
    haircuts++;
    sem_post(&entry_mutex);
}

int mon_checkStylist() {
    sem_wait(&entry_mutex);
    if (customer_count < CHAIRS) {
        customer_count++;
        if (customer_count == CHAIRS) salon_full++;
        cv_signal(&customerAvailable);
        cv_wait(&stylistAvailable);
        sem_post(&entry_mutex);
        return 1; // Got haircut
    }
    sem_post(&entry_mutex);
    return 0; // Go shopping
}