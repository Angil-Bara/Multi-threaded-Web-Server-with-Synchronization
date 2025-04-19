#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include "webserver.h"

#define MAX_REQUEST 100

int port, numThread;

// Shared buffer and synchronization variables
int buffer[MAX_REQUEST];
int buffer_index = 0;
sem_t sem_full;  // Tracks the number of filled slots in the buffer
sem_t sem_empty; // Tracks the number of empty slots in the buffer
pthread_mutex_t mutex; // Protects access to the buffer

void *listener() {
    int r;
    struct sockaddr_in sin;
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);
    r = bind(sock, (struct sockaddr *) &sin, sizeof(sin));
    if (r < 0) {
        perror("Error binding socket:");
        return NULL;
    }

    r = listen(sock, 5);
    if (r < 0) {
        perror("Error listening socket:");
        return NULL;
    }

    printf("HTTP server listening on port %d\n", port);
    while (1) {
        int s = accept(sock, NULL, NULL);
        if (s < 0) {
            perror("Error accepting connection:");
            continue;
        }

        // Wait for an empty slot in the buffer
        sem_wait(&sem_empty);
        pthread_mutex_lock(&mutex);

        // Add the socket descriptor to the buffer
        buffer[buffer_index++] = s;
        printf("Listener: Added request %d to buffer\n", s);

        pthread_mutex_unlock(&mutex);
        sem_post(&sem_full); // Signal that a new request is available
    }

    close(sock);
    return NULL;
}

void *worker(void *arg) {
    while (1) {
        // Wait for a filled slot in the buffer
        sem_wait(&sem_full);
        pthread_mutex_lock(&mutex);

        // Remove the socket descriptor from the buffer
        int s = buffer[--buffer_index];
        printf("Worker %ld: Processing request %d\n", (long)arg, s);

        pthread_mutex_unlock(&mutex);
        sem_post(&sem_empty); // Signal that a slot is available

        // Process the request
        process(s);
        close(s);
    }
    return NULL;
}

void thread_control() {
    pthread_t listener_thread;
    pthread_t worker_threads[numThread];

    // Initialize semaphores and mutex
    sem_init(&sem_full, 0, 0);
    sem_init(&sem_empty, 0, MAX_REQUEST);
    pthread_mutex_init(&mutex, NULL);

    // Create the listener thread
    pthread_create(&listener_thread, NULL, listener, NULL);

    // Create worker threads
    for (int i = 0; i < numThread; i++) {
        pthread_create(&worker_threads[i], NULL, worker, (void *)(long)i);
    }

    // Join threads (this will block indefinitely)
    pthread_join(listener_thread, NULL);
    for (int i = 0; i < numThread; i++) {
        pthread_join(worker_threads[i], NULL);
    }

    // Clean up synchronization tools
    sem_destroy(&sem_full);
    sem_destroy(&sem_empty);
    pthread_mutex_destroy(&mutex);
}

int main(int argc, char *argv[]) {
    if (argc != 3 || atoi(argv[1]) < 2000 || atoi(argv[1]) > 50000) {
        fprintf(stderr, "./webserver_multi PORT(2001 ~ 49999) #_of_threads\n");
        return 0;
    }

    port = atoi(argv[1]);
    numThread = atoi(argv[2]);

    if (numThread > 100) {
        fprintf(stderr, "Number of threads cannot exceed 100\n");
        return 0;
    }

    thread_control();
    return 0;
}