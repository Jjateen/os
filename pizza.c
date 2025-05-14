#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_STUDENTS 5
#define SLICES_PER_PIZZA 5

int slices = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pizza_arrived = PTHREAD_COND_INITIALIZER;
pthread_cond_t order_pizza = PTHREAD_COND_INITIALIZER;

int ordering = 0;  // flag to prevent multiple students from ordering at once

void* student_thread(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&lock);
        while (slices == 0) {
            if (!ordering) {
                ordering = 1;
                printf("Student %d is calling for more pizza...\n", id);
                pthread_cond_signal(&order_pizza);  // wake up delivery
            }
            printf("Student %d is waiting for pizza.\n", id);
            pthread_cond_wait(&pizza_arrived, &lock);
        }

        // Eat a slice
        slices--;
        printf("Student %d ate a slice. Slices left: %d\n", id, slices);
        pthread_mutex_unlock(&lock);

        // Study while eating
        sleep(1);
    }
    return NULL;
}

void* delivery_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);
        while (!ordering)
            pthread_cond_wait(&order_pizza, &lock);

        // Deliver pizza
        slices = SLICES_PER_PIZZA;
        printf("\n[Pizza delivered with %d slices]\n\n", slices);
        ordering = 0;
        pthread_cond_broadcast(&pizza_arrived);  // Wake up all students
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main() {
    pthread_t students[NUM_STUDENTS], delivery;
    int ids[NUM_STUDENTS];

    pthread_create(&delivery, NULL, delivery_thread, NULL);
    for (int i = 0; i < NUM_STUDENTS; i++) {
        ids[i] = i + 1;
        pthread_create(&students[i], NULL, student_thread, &ids[i]);
    }

    for (int i = 0; i < NUM_STUDENTS; i++)
        pthread_join(students[i], NULL);

    pthread_join(delivery, NULL);
    return 0;
}
