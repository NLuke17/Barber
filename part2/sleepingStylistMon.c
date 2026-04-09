#include <pthread.h>
#include <unistd.h>

extern void mon_checkCustomer();
extern int mon_checkStylist();
extern void mon_debugPrint();

#define NUM_CUSTOMERS 75
#define DELAY 1000000

void *stylist(void *arg) {
    while(1) {
        mon_debugPrint();
        mon_checkCustomer();
        for (int j = 0; j < DELAY; j++); // Cutting hair
    }
}

void *customer(void *arg) {
    while(1) {
        mon_debugPrint();
        if (mon_checkStylist()) break;
        for (int j = 0; j < DELAY; j++); // Shopping
    }
    return NULL;
}

int main() {
    pthread_t stylist_tid;
    pthread_t customer_tids[NUM_CUSTOMERS];
    int customer_ids[NUM_CUSTOMERS];

    // 1. Initialize Monitor Entry Queue Semaphore [cite: 31]
    sem_init(&entry_mutex, 0, 1);

    // 2. Initialize Condition Variable Semaphores 
    sem_init(&stylistAvailable.sem, 0, 0);
    sem_init(&customerAvailable.sem, 0, 0);
    stylistAvailable.count = 0;
    customerAvailable.count = 0;

    // 3. Create Stylist Thread [cite: 129]
    if (pthread_create(&stylist_tid, NULL, stylistFunction, NULL) != 0) {
        perror("Failed to create stylist thread");
        return 1;
    }

    // 4. Create 75 Customer Threads [cite: 127]
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        customer_ids[i] = i + 1;
        if (pthread_create(&customer_tids[i], NULL, customerFunction, &customer_ids[i]) != 0) {
            perror("Failed to create customer thread");
            return 1;
        }
    }

    // 5. Join Customer Threads [cite: 128]
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customer_tids[i], NULL);
    }

    // Note: Stylist thread runs in an infinite loop; 
    // In a real scenario, you'd signal it to exit here.
    printf("All customers have been served.\n");

    return 0;
}