#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2

pthread_mutex_t mutex;
pthread_mutex_t forks[NUM_PHILOSOPHERS];
int state[NUM_PHILOSOPHERS];
int phil_num[NUM_PHILOSOPHERS];

void test(int i) {
    int left = (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
    int right = (i + 1) % NUM_PHILOSOPHERS;

    if (state[i] == HUNGRY && state[left] != EATING && state[right] != EATING) {
        state[i] = EATING;
        pthread_mutex_unlock(&forks[i]);
    }
}

void take_forks(int i) {
    pthread_mutex_lock(&mutex);
    state[i] = HUNGRY;
    printf("Philosopher %d is HUNGRY\n", i);
    test(i);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_lock(&forks[i]);
}

void put_forks(int i) {
    pthread_mutex_lock(&mutex);
    state[i] = THINKING;
    printf("Philosopher %d is THINKING\n", i);
    int left = (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
    int right = (i + 1) % NUM_PHILOSOPHERS;
    test(left);
    test(right);
    pthread_mutex_unlock(&mutex);
}

void* philosopher(void* num) {
    int i = *(int*)num;
    while (1) {
        printf("Philosopher %d is THINKING\n", i);
        sleep(rand() % 3 + 1);
        take_forks(i);
        printf("Philosopher %d is EATING\n", i);
        sleep(rand() % 2 + 1);
        put_forks(i);
    }
    return NULL;
}

int main() {
    pthread_t thread_id[NUM_PHILOSOPHERS];
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_init(&forks[i], NULL);
        pthread_mutex_lock(&forks[i]);
        phil_num[i] = i;
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_create(&thread_id[i], NULL, philosopher, &phil_num[i]);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(thread_id[i], NULL);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    pthread_mutex_destroy(&mutex);

    return 0;
}
