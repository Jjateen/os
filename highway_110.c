#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef enum { NORTH, SOUTH } Direction;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_enter = PTHREAD_COND_INITIALIZER;

int cars_in_tunnel = 0;
int waiting_north = 0, waiting_south = 0;
Direction current_direction;
int tunnel_direction_set = 0;  // flag: 0 if tunnel is free

void Arrive(Direction dir) {
    pthread_mutex_lock(&lock);

    if (dir == NORTH) waiting_north++;
    else waiting_south++;

    while ((cars_in_tunnel > 0 && current_direction != dir) ||
           (tunnel_direction_set && current_direction != dir)) {
        pthread_cond_wait(&can_enter, &lock);
    }

    if (dir == NORTH) waiting_north--;
    else waiting_south--;

    if (!tunnel_direction_set) {
        tunnel_direction_set = 1;
        current_direction = dir;
    }

    cars_in_tunnel++;
    printf("Car entered tunnel from %s. Cars in tunnel: %d\n",
           dir == NORTH ? "NORTH" : "SOUTH", cars_in_tunnel);

    pthread_mutex_unlock(&lock);
}

void Depart(Direction dir) {
    pthread_mutex_lock(&lock);

    cars_in_tunnel--;

    printf("Car exited tunnel from %s. Cars left in tunnel: %d\n",
           dir == NORTH ? "NORTH" : "SOUTH", cars_in_tunnel);

    if (cars_in_tunnel == 0) {
        tunnel_direction_set = 0;  // Tunnel is now free
        pthread_cond_broadcast(&can_enter);
    } else {
        pthread_cond_signal(&can_enter);
    }

    pthread_mutex_unlock(&lock);
}

// --- Thread function to simulate car ---
void* Car(void* arg) {
    Direction dir = *(Direction*)arg;

    Arrive(dir);
    sleep(1);  // simulate travel time
    Depart(dir);

    return NULL;
}

// --- Main driver ---
int main() {
    pthread_t cars[10];
    Direction directions[10] = {NORTH, NORTH, SOUTH, SOUTH, NORTH, SOUTH, NORTH, SOUTH, NORTH, SOUTH};

    for (int i = 0; i < 10; i++) {
        pthread_create(&cars[i], NULL, Car, &directions[i]);
        usleep(200000);  // Stagger arrivals
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(cars[i], NULL);
    }

    return 0;
}
