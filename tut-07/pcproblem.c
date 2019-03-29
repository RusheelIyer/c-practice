#include <pthread>

#define BUFFER_SIZE 10
int buffer[BUFFER_SIZE];
int index = 0;

pthread_mutex_t lock;
sem_t fill, empty;

void initialise() {
  pthread_mutex_init(&lock, NULL);
  sem_init(&sem, 0, 0);
  sem_init(&sem, 0, BUFFER_SIZE);
}

void* producer(void* arg) {
    while (1) {
        int item = produce();

        sem_wait(&empty);

        pthread_mutex_lock(&lock);
        buffer[index++] = item;
        pthread_mutex_unlock(&lock);

        sem_post(&empty);
    }
}

void* consumer(void * arg) {
    while (1) {

        sem_wait(&fill);

        pthread_mutex_lock(&lock);
        int item = buffer[index--];
        pthread_mutex_unlock(&lock);

        sem_post(&fill);

        consume(item);

    }
}
