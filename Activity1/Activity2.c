#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#define SHARED_MEM_SIZE 4

int main()
{
    // Create and initialize the semaphores
    sem_t *sem1 = sem_open("/activity2_sem1", O_CREAT, 0666, 1);
    sem_t *sem2 = sem_open("/activity2_sem2", O_CREAT, 0666, 0);

    // Create shared memory space
    int fd = shm_open("/activity2_shm", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        printf("Failed to create shared memory space.\n");
        return 1;
    }

    // Set shared memory space size to 4 bytes
    ftruncate(fd, SHARED_MEM_SIZE);

    // Map shared memory space in process
    int *shared_mem = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_mem == MAP_FAILED) {
        printf("Failed to map shared memory space.\n");
        return 1;
    }

    // Seed the random number generator
    srand(time(NULL));

    // Generate a random number and write it to shared memory
    int rand_num = rand() % 100 + 1;
    *shared_mem = rand_num;
    printf("Random number generated: %d\n", rand_num);

    // Create child process
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        while (*shared_mem > 0) {
            sem_wait(sem2);
            *shared_mem -= 1;
            printf("Child process decremented shared memory: %d\n", *shared_mem);
            sem_post(sem1);
        }
    } else if (pid > 0) {
        // Parent process
        while (*shared_mem > 0) {
            sem_wait(sem1);
            *shared_mem -= 1;
            printf("Parent process decremented shared memory: %d\n", *shared_mem);
            sem_post(sem2);
        }
    } else {
        // Error creating child process
        printf("Failed to create child process.\n");
        return 1;
    }

    // Release semaphores
    sem_close(sem1);
    sem_close(sem2);

    // Unmap, close and free shared memory space
    munmap(shared_mem, SHARED_MEM_SIZE);
    close(fd);
    shm_unlink("/activity2_shm");

    return 0;
}

