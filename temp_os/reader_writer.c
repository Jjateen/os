#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2

int shared_data = 0;
int read_count = 0;

sem_t mutex; // To protect read_count
sem_t wrt;   // To lock shared_data for writers

void* reader(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(&mutex);
        read_count++;
        if (read_count == 1)
            sem_wait(&wrt); // First reader locks writers
        sem_post(&mutex);

        // Reading section
        printf("Reader %d: read shared_data = %d\n", id, shared_data);
        sleep(1); // simulate reading time

        sem_wait(&mutex);
        read_count--;
        if (read_count == 0)
            sem_post(&wrt); // Last reader unlocks writer
        sem_post(&mutex);

        sleep(1 + rand() % 3); // random delay
    }
    return NULL;
}

void* writer(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(&wrt); // Writer locks resource
        shared_data += 10;
        printf("Writer %d: updated shared_data to %d\n", id, shared_data);
        sleep(2); // simulate writing
        sem_post(&wrt); // Unlock resource

        sleep(2 + rand() % 3); // random delay
    }
    return NULL;
}

int main() {
    pthread_t rtid[NUM_READERS], wtid[NUM_WRITERS];
    int rids[NUM_READERS], wids[NUM_WRITERS];

    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);

    // Create reader threads
    for (int i = 0; i < NUM_READERS; i++) {
        rids[i] = i + 1;
        pthread_create(&rtid[i], NULL, reader, &rids[i]);
    }

    // Create writer threads
    for (int i = 0; i < NUM_WRITERS; i++) {
        wids[i] = i + 1;
        pthread_create(&wtid[i], NULL, writer, &wids[i]);
    }

    // Join threads (they run infinitely, so this part won't execute)
    for (int i = 0; i < NUM_READERS; i++)
        pthread_join(rtid[i], NULL);
    for (int i = 0; i < NUM_WRITERS; i++)
        pthread_join(wtid[i], NULL);

    sem_destroy(&mutex);
    sem_destroy(&wrt);
    return 0;
}
    