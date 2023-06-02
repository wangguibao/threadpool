#include "threadpool.h"
#include <iostream>
#include <time.h>

ThreadPool::ThreadPool(size_t size) {
    workers.resize(size);

    for (size_t i = 0; i < size; ++i) {
        int ret = pthread_create(&workers[i], nullptr, &ThreadPool::thread_worker, this);
        if (ret != 0) {
            std::cout << "pthread create " << i << "failed";
        }
    }
}

ThreadPool::~ThreadPool() {
    std::cout << "~ThreadPool" << std::endl;

    running = false;
    pthread_cond_broadcast(&cond);

    size_t size = workers.size();
    for (size_t i = 0; i < size; ++i) {
        pthread_join(workers[i], nullptr);
    }
}

void* ThreadPool::thread_worker(void* arg) {
    ThreadPool* pool = reinterpret_cast<ThreadPool*>(arg);

    while (1) {
        if (!pool->running) {
            return nullptr;
        }

        pthread_mutex_lock(&pool->mutex);

        while (pool->running && pool->q.empty()) {
            timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 2;

            int state = pthread_cond_timedwait(&pool->cond, &pool->mutex, &ts);

            if (state == ETIMEDOUT) {
                pthread_mutex_unlock(&pool->mutex);
                break;
            } else if (state != 0){
                pthread_mutex_unlock(&pool->mutex);
                return nullptr;
            }
        }

        if (!pool->running) {
            pthread_mutex_unlock(&pool->mutex);
            return nullptr;
        }

        if (pool->q.empty()) {
            pthread_mutex_unlock(&pool->mutex);
            return nullptr;
        }

        Task task = pool->q.front();
        pool->q.pop();

        pthread_mutex_unlock(&pool->mutex);

        task.run(task.value);
    }
}

void ThreadPool::add_task(Task task) {
    if (!running) {
        return;
    }

    pthread_mutex_lock(&mutex);

    q.push(task);
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&mutex);
}

