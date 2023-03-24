#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

typedef struct {
    uint32_t * data_ptr;
    pthread_mutex_t * mutex1;
    pthread_mutex_t * mutex2;
} 
thread_data_t;

void * thread_func1(void * arg) {
    thread_data_t * data = (thread_data_t *) arg;
    uint32_t * turns = data->data_ptr;

    while (*turns > 0) {
        pthread_mutex_lock(data->mutex1);
        if (*turns > 0) {
            printf("Thread 1: %d\n", *turns);
            (*turns)--;
        }
        pthread_mutex_unlock(data->mutex2);
    }

    return NULL;
}

void * thread_func2(void * arg) {
    thread_data_t * data = (thread_data_t *) arg;
    uint32_t * turns = data->data_ptr;

    while (*turns > 0) {
        pthread_mutex_lock(data->mutex2);
        if (*turns > 0) {
            printf("Thread 2: %d\n", *turns);
            (*turns)--;
        }
        pthread_mutex_unlock(data->mutex1);
    }

    return NULL;
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("Usage: %s <num_turns>\n", argv[0]);
        return 1;
    }

    uint32_t turns = atoi(argv[1]);
    pthread_mutex_t mutex1, mutex2;
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);

    thread_data_t data1 = {&turns, &mutex1, &mutex2};
    thread_data_t data2 = {&turns, &mutex2, &mutex1};

    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, thread_func1, &data1);
    pthread_create(&thread2, NULL, thread_func2, &data2);

    pthread_mutex_lock(&mutex1);
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);

    return 0;
}
