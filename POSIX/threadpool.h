#pragma once

#include <pthread.h>
#include <vector>
#include <queue>

struct Task {
    int value;
    void (*run)(int arg);
};

class ThreadPool {
public:
    ThreadPool(size_t size);
    ~ThreadPool();

    void add_task(Task task);

private:
    static void* thread_worker(void* arg); 

private:
    std::queue<Task> q;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    std::vector<pthread_t> workers;

    bool running = true;
};
