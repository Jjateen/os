#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_TRAINS 5

// Binary semaphore for the railway track
sem_t track_sem;

// Struct for each train
typedef struct {
    int train_id;
    int travel_time; // seconds on track
} Train;

void* train_runner(void* arg) {
    Train* train = (Train*)arg;

    printf("Train %d: waiting for track\n", train->train_id);
    sem_wait(&track_sem);  // request access to track

    // On the track
    printf("Train %d: on track (for %d sec)\n", train->train_id, train->travel_time);
    sleep(train->travel_time);  // simulate travel time

    printf("Train %d: leaving track\n", train->train_id);
    sem_post(&track_sem);  // release track

    return NULL;
}

int main() {
    pthread_t trains[NUM_TRAINS];
    Train train_data[NUM_TRAINS];

    // Initialize the binary semaphore (1 = track available)
    sem_init(&track_sem, 0, 1);

    // Create train threads
    for (int i = 0; i < NUM_TRAINS; i++) {
        train_data[i].train_id = i + 1;
        train_data[i].travel_time = 2 + rand() % 4; // random 2–5 seconds
        pthread_create(&trains[i], NULL, train_runner, &train_data[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_TRAINS; i++) {
        pthread_join(trains[i], NULL);
    }

    sem_destroy(&track_sem);
    return 0;
}
