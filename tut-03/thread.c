#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>

void* print_thread(void* id) {
    printf("Printing from thread %"PRIdPTR"\n", (intptr_t) id);
    pthread_exit((void*) 0);
}

int main(int argc, char const *argv[])
{
#define NUM 5

    pthread_t threads[NUM];
    
    for (int i = 1; i <= NUM; i++) {
        int status = pthread_create(threads + i - 1, NULL, print_thread, (void*)((int*) i));

        if (status != 0) {
            printf("Error creating the thread\n");
            exit(1);
        }
    }

    for (int i = 0; i < NUM; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;

}
