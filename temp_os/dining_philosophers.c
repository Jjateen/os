#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

// States
enum { THINKING, HUNGRY, EATING };

// Shared variables
int state[NUM_PHILOSOPHERS];
sem_t mutex;              // For mutual exclusion
sem_t S[NUM_PHILOSOPHERS]; // One semaphore per philosopher

// Get left and right neighbor
#define LEFT (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS
#define RIGHT (i + 1) % NUM_PHILOSOPHERS

void test(int i) {
    if (state[i] == HUNGRY &&
        state[LEFT] != EATING &&
        state[RIGHT] != EATING) {

        state[i] = EATING;
        printf("Philosopher %d takes forks %d and %d and starts eating.\n", 
               i, LEFT, i);
        sem_post(&S[i]);
    }
}

void take_forks(int i) {
    sem_wait(&mutex);        // Enter critical section
    state[i] = HUNGRY;
    printf("Philosopher %d is hungry.\n", i);
    test(i);                 // Try to acquire forks
    sem_post(&mutex);        // Exit critical section
    sem_wait(&S[i]);         // Block if forks not acquired
}

void put_forks(int i) {
    sem_wait(&mutex);
    state[i] = THINKING;
    printf("Philosopher %d puts down forks %d and %d and starts thinking.\n", 
           i, LEFT, i);
    test(LEFT);              // See if left neighbor can eat
    test(RIGHT);             // See if right neighbor can eat
    sem_post(&mutex);
}

void* philosopher(void* num) {
    int i = *(int*)num;
    while (1) {
        printf("Philosopher %d is thinking...\n", i);
        sleep(1 + rand() % 3);
        take_forks(i);
        sleep(2); // Simulate eating
        put_forks(i);
    }
}

int main() {
    pthread_t tid[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    // Initialize the semaphores
    sem_init(&mutex, 0, 1);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_init(&S[i], 0, 0);
        state[i] = THINKING;
        ids[i] = i;
    }

    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&tid[i], NULL, philosopher, &ids[i]);
    }

    // Join threads (infinite loop so this won't execute, just here for completeness)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(tid[i], NULL);
    }

    // Cleanup (never reached)
    sem_destroy(&mutex);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        sem_destroy(&S[i]);
    }

    return 0;
}
