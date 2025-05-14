#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <string.h>

#define SHM_SIZE 1024  // size of shared memory
#define MSG_SIZE 128   // size of message

struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    key_t shm_key = ftok("shmfile", 65);
    int shm_id;
    char *shm_ptr;

    key_t msg_key = ftok("msgfile", 75);
    int msg_id;
    struct msg_buffer message;

    shm_id = shmget(shm_key, SHM_SIZE, 0666 | IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }
    printf("Shared memory created with ID: %d\n", shm_id);

    shm_ptr = (char*) shmat(shm_id, NULL, 0);
    if (shm_ptr == (char*) -1) {
        perror("shmat");
        exit(1);
    }
    printf("Shared memory attached at address: %p\n", shm_ptr);

    int transferred_amount = 1000; // Example transferred amount
    sprintf(shm_ptr, "Banking system using IPC. Transferred amount: %d", transferred_amount);
    printf("Written to shared memory: %s\n", shm_ptr);

    msg_id = msgget(msg_key, 0666 | IPC_CREAT);
    if (msg_id == -1) {
        perror("msgget");
        exit(1);
    }
    printf("Message queue created with ID: %d\n", msg_id);

    message.msg_type = 1;
    strcpy(message.msg_text, "Transaction completed successfully");
    if (msgsnd(msg_id, &message, sizeof(message), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
    printf("Message sent: %s\n", message.msg_text);

    if (msgrcv(msg_id, &message, sizeof(message), 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }
    printf("Received message: %s\n", message.msg_text);

    if (shmdt(shm_ptr) == -1) {
        perror("shmdt");
        exit(1);
    }
    printf("Shared memory detached\n");

    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }
    printf("Shared memory removed\n");

    if (msgctl(msg_id, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
    printf("Message queue removed\n");

    return 0;
}