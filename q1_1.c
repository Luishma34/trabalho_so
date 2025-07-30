#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0;
int active = 3;
sem_t empty;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *producer(void *args) {
    int n = *((int *)args);
    int iter = 1;
    srand(time(NULL) + (unsigned long)pthread_self());

    while (n-- > 0) {
        int val = rand() % 1000 + 1;
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[count++] = val;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        printf("(P) TID: %lu | VALOR: R$ %d | ITERACAO: %d\n", (unsigned long)pthread_self(), val, iter++);
        int delay = rand() % 5 + 1;
        sleep(delay);
    }

    pthread_mutex_lock(&mutex);
    active--;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    printf("(P) TID: %lu finalizou\n", (unsigned long)pthread_self());
    return NULL;
}

void *consumer(void *args) {
    int iter = 1;
    while (active > 0 || count > 0) {
        pthread_mutex_lock(&mutex);
        while (count < BUFFER_SIZE && active > 0) {
            pthread_cond_wait(&cond, &mutex);
        }
        if (count < BUFFER_SIZE) {
            int num = count;
            if (num > 0) {
                double sum = 0;
                for (int i = 0; i < num; i++) sum += buffer[i];
                double avg = sum / num;
                printf("(C) TID: %lu | MEDIA: R$ %.2f (partial de %d itens) | ITERACAO: %d\n", (unsigned long)pthread_self(), avg, num, iter++);
                count = 0;
                pthread_mutex_unlock(&mutex);
                for (int j = 0; j < num; j++) sem_post(&empty);
            } else {
                pthread_mutex_unlock(&mutex);
            }
            continue;
        }
        double sum = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) sum += buffer[i];
        double avg = sum / BUFFER_SIZE;
        printf("(C) TID: %lu | MEDIA: R$ %.2f | ITERACAO: %d\n", (unsigned long)pthread_self(), avg, iter++);
        count = 0;
        pthread_mutex_unlock(&mutex);
        for (int i = 0; i < BUFFER_SIZE; i++) sem_post(&empty);
    }
    printf("(C) TID: %lu finalizou\n", (unsigned long)pthread_self());
    return NULL;
}

int main(void) {
    sem_init(&empty, 0, BUFFER_SIZE);
    int ns[3] = {20, 25, 30};
    pthread_t producers[3], consumer_t;
    for (int i = 0; i < 3; i++) {
        pthread_create(&producers[i], NULL, producer, &ns[i]);
    }
    pthread_create(&consumer_t, NULL, consumer, NULL);
    for (int i = 0; i < 3; i++) {
        pthread_join(producers[i], NULL);
    }
    pthread_join(consumer_t, NULL);
    sem_destroy(&empty);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}