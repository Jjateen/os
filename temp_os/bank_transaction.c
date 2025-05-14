#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_CLIENTS 6

int balance = 1000; // initial bank balance
sem_t balance_mutex;

typedef struct {
    int client_id;
    int amount;
    int is_deposit; // 1 = deposit, 0 = withdraw
} Transaction;

void* perform_transaction(void* arg) {
    Transaction* txn = (Transaction*)arg;

    sem_wait(&balance_mutex); // lock access to balance

    printf("Client %d: trying to %s ₹%d...\n", txn->client_id,
           txn->is_deposit ? "deposit" : "withdraw", txn->amount);

    if (txn->is_deposit) {
        balance += txn->amount;
        printf("Client %d: deposited ₹%d | Balance: ₹%d\n", txn->client_id, txn->amount, balance);
    } else {
        if (txn->amount <= balance) {
            balance -= txn->amount;
            printf("Client %d: withdrew ₹%d | Balance: ₹%d\n", txn->client_id, txn->amount, balance);
        } else {
            printf("Client %d: insufficient funds to withdraw ₹%d | Balance: ₹%d\n",
                   txn->client_id, txn->amount, balance);
        }
    }

    sem_post(&balance_mutex); // unlock
    return NULL;
}

int main() {
    pthread_t clients[NUM_CLIENTS];
    Transaction txns[NUM_CLIENTS];

    sem_init(&balance_mutex, 0, 1);

    srand(time(NULL)); // for randomness

    // Randomly generate deposit and withdrawal transactions
    for (int i = 0; i < NUM_CLIENTS; i++) {
        txns[i].client_id = i + 1;
        txns[i].amount = (rand() % 900) + 100; // ₹100 to ₹1000
        txns[i].is_deposit = rand() % 2;

        pthread_create(&clients[i], NULL, perform_transaction, &txns[i]);
    }

    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    printf("Final Balance: ₹%d\n", balance);
    sem_destroy(&balance_mutex);
    return 0;
}
