# Multi-threaded-Web-Server-with-Synchronization
 successfully implemented a multi-threaded web server using a producer-consumer  model with thread synchronization as required by the project specifications. The  implementation converts the single-threaded web server into a more scalable thread-pool  architecture.

 # What I've Implemented
 1. Thread Pool Architecture: Created a thread pool consisting of N worker threads and 1
 listener thread, where N is specified by the user via command-line arguments.
 2. Producer-Consumer Model: Implemented a producer-consumer model where:
 • The listener thread (producer) accepts incoming client connections and adds
 them to a shared buffer.
 • Worker threads (consumers) retrieve connection requests from the buffer and
 process them.
 3. Synchronization Mechanisms: Used the following synchronization tools to avoid race
 conditions and deadlocks:
 • Twosemaphores (sem_full and sem_empty) to track buffer status
 • Onemutex lock (mutex) to protect access to the shared buffer
 4. Command-line Argument Handling: Added validation for user-provided arguments,
 including port range checking (2000-50000) and limiting the maximum number of
 threads to 100.
 5. Graceful Resource Management: Implemented proper initialization and cleanup of
 threads and synchronization resources.
 Design Details
 Shared Data Structure
 • Buffer Implementation: Used a fixed-size array (buffer[MAX_REQUEST]) to store
 socket descriptors of pending client connections.
 • Buffer Size: Set to 100 connections (MAX_REQUEST) to limit memory usage while
 providing sufficient capacity.
 • Buffer Index Management: A single index variable tracks the current position in the
 buffer, which is incremented by the producer and decremented by consumers.
 # Synchronization Design
 • Producer Logic (Listener Thread):
 1. Waits for an empty slot in the buffer using sem_wait(&sem_empty)
 2. Acquires the mutex lock to safely modify the buffer
 3. Adds the socket descriptor to the buffer and increments the index
 4. Releases the mutex lock
 5. Signals that a new item is available with sem_post(&sem_full)
 • Consumer Logic (Worker Threads):
 1. Waits for a filled slot in the buffer using sem_wait(&sem_full)
 2. Acquires the mutex lock to safely access the buffer
 3. Removes a socket descriptor from the buffer and decrements the index
 4. Releases the mutex lock
 5. Signals that an empty slot is available with sem_post(&sem_empty)
 6. Processes the request and closes the socket
 # Concurrency Problem Prevention
 1. Race Condition Prevention: The mutex lock ensures that only one thread can modify
 the shared buffer at any given time, preventing race conditions.
 2. Deadlock Prevention:
 • The semaphores manage the producer-consumer relationship correctly
 • The lock acquisition order is consistent (always acquire the mutex after the
 semaphore)
 • Each lock is released properly after use, even in error conditions
 3. Buffer Overflow/Underflow Prevention:
 • sem_empty prevents the producer from adding to a full buffer
 • sem_full prevents consumers from removing from an empty buffer
 Testing and Verification
 # Testing Methodology
 1. Functionality Testing:
 • Tested with the provided client application
 • Verified correct handling of simultaneous requests by launching multiple clients
 2. Concurrency Testing:
 • Added debug print statements to track thread behavior and buffer state
 • Used varying numbers of worker threads to observe synchronization behavior
 • Tested with high request rates to stress the synchronization mechanisms
 3. Performance Testing:
 • Compared response times with different numbers of worker threads
 • Verified that more threads improved performance up to a point, after which
 overhead became apparent
 # Race Condition Verification
 To verify the absence of race conditions:
 • Added detailed logging of buffer operations
 • Analyzed logs to ensure buffer modifications were atomic
 • Verified that the buffer index never exceeded bounds
 • Checked that each request was processed exactly once
 # Deadlock Testing
 To verify the absence of deadlocks:
 • Ran the server continuously with varying request patterns
 • Tested extreme cases (many simultaneous connections)
 • Verified that all threads remained responsive
 • Confirmed that resources were properly released after use
