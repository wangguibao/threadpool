#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

class Task {
public:
    std::function<void(void* arg)> run;
    void* arg;
};

class ThreadPool
{
public:
    ThreadPool(int thread_num);

    ~ThreadPool();

public:
    void addTask(Task task);

private:
    static void thread_func(ThreadPool *pool);

private:
    std::queue<Task> task_queue;
    std::mutex mutex;
    std::condition_variable cond;

    std::vector<std::thread> thread_vec;
    int thread_num;

    bool running = false;
};

#endif // THREADPOOL_H
