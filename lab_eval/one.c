#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

int arr[] = {3, 2, 1, 4};
const int n = sizeof(arr) / sizeof(arr[0]);
const int n_half = sizeof(arr) / sizeof(arr[0]) / 2;


void sort_arr(int *half_arr, int n_half)
{
    int temp;
    for (int i = 0; i < n_half; i++)
    {
        for (int j = i + 1; j < n_half; j++)
        {
            if (half_arr[i] > half_arr[j])
            {
                temp = half_arr[i];
                half_arr[i] = half_arr[j];
                half_arr[j] = temp;
            }
        }
    }
    for (int i = 0; i < n_half; i++)
    {
        printf("%d ", half_arr[i]);
    }
    printf("\n");
}

int main()
{
    pid_t pid;
    int shm_id;
    int *shm_arr;

    int arr[] = {3, 2, 1, 4};
    const int n = sizeof(arr) / sizeof(arr[0]);
    const int n_half = sizeof(arr) / sizeof(arr[0]) / 2;

    shm_id = shmget(IPC_PRIVATE, n * sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0)
    {
        perror("shmget");
        return 1;
    }
    
    shm_arr = (int *)shmat(shm_id, NULL, 0);
    
    if (shm_arr == (int *)-1)
    {
        perror("shmat");
        return 1;
    }
    
    for (int i = 0; i < n; i++)
    {
        shm_arr[i] = arr[i];
        printf("%d ", shm_arr[i]);
    }
    printf("\n");
    
    pid = fork();
    
    if (pid == 0)
    {
        printf("Child process\n");
        printf("Sorting Half of the array\n");

        sort_arr(shm_arr, n_half);
    }
    else if (pid > 0)
    {
        wait(NULL);
        printf("Parent process\n");
        printf("Merging the sorted array\n");

        for (int i = 0; i < n_half; i++)
        {
                arr[i] = shm_arr[i];
        }

        for (int i = 0; i < n; i++)
        {
            printf("%d ", arr[i]);
        }
        printf("\n");

        shmdt(shm_arr);
        shmctl(shm_id, IPC_RMID, NULL);
    }
    else
    {
        perror("fork");
        return 1;
    }

    return 0;
}