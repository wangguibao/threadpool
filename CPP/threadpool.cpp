#include "threadpool.h"
#include <thread>
#include <iostream>

ThreadPool::ThreadPool(int thread_num)
{
    running = true;

    for (int i = 0; i < thread_num; ++i) {
        thread_vec.push_back(std::thread(ThreadPool::thread_func, this));
    }
    this->thread_num = thread_num;
}

ThreadPool::~ThreadPool() {
    std::cout << "ThreadPool reclaiming resource" << std::endl;
    running = false;
    cond.notify_all();

    for (size_t i = 0; i < thread_vec.size(); ++i) {
        thread_vec[i].join();
    }

    std::cout << "ThreadPool quit" << std::endl;
}

void ThreadPool::addTask(Task task) {
    std::lock_guard<std::mutex> lock(mutex);
    task_queue.push(task);

    cond.notify_one();
}

void ThreadPool::thread_func(ThreadPool* pool) {
    while (1) {
        std::unique_lock<std::mutex> lock(pool->mutex);
        pool->cond.wait(lock,
                        [pool]() {
                            auto ret = (!pool->running) || (!pool->task_queue.empty());
                            return ret;
                        }
        );

        if (!pool->running) {
            std::cout << "pool not running, quit" << std::endl;
            return;
        }

        auto task = pool->task_queue.front();
        pool->task_queue.pop();
        lock.unlock();

        task.run(task.arg);

        delete task.arg;
    }
}
