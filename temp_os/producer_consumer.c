#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 3
#define NUM_ITEMS_PER_PRODUCER 5

int buffer[BUFFER_SIZE];
int in = 0, out = 0;

sem_t mutex;     // mutual exclusion for buffer
sem_t empty;     // counts empty slots
sem_t full;      // counts filled slots

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS_PER_PRODUCER; i++) {
        int item = rand() % 100;

        sem_wait(&empty);
        sem_wait(&mutex);

        buffer[in] = item;
        printf("Producer %d: produced %d at index %d\n", id, item, in);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&full);

        sleep(1 + rand() % 2); // simulate production delay
    }
    return NULL;
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(&full);
        sem_wait(&mutex);

        int item = buffer[out];
        printf("Consumer %d: consumed %d from index %d\n", id, item, out);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&empty);

        sleep(1 + rand() % 3); // simulate consumption delay
    }
    return NULL;
}

int main() {
    pthread_t prod[NUM_PRODUCERS], cons[NUM_CONSUMERS];
    int ids[NUM_PRODUCERS > NUM_CONSUMERS ? NUM_PRODUCERS : NUM_CONSUMERS];

    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        ids[i] = i + 1;
        pthread_create(&prod[i], NULL, producer, &ids[i]);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        ids[i] = i + 1;
        pthread_create(&cons[i], NULL, consumer, &ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++)
        pthread_join(prod[i], NULL);

    // Let consumers run for a while then cancel (for demo purposes)
    sleep(10);
    for (int i = 0; i < NUM_CONSUMERS; i++)
        pthread_cancel(cons[i]);

    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}
