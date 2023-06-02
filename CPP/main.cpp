#include "threadpool.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

struct TaskArg1 {
    int thread_id;
    int loop;
};

void looper1(void* arg) {
    TaskArg1* task_arg = reinterpret_cast<TaskArg1*>(arg);
    std::cout << "Task looper1 " << task_arg->thread_id << ", loop " << task_arg->loop << std::endl;
    std::cout.flush();

    auto duration = std::chrono::seconds(task_arg->loop);
    std::this_thread::sleep_for(duration);
    std::cout << "End of task looper1 " << task_arg->thread_id << std::endl;
    std::cout.flush();
}

struct TaskArg2 {
    int thread_id;
    int loop;
    int abs;
};

void looper2(void* arg) {
    TaskArg2* task_arg = reinterpret_cast<TaskArg2*>(arg);
    std::cout << "Task looper2 " << task_arg->thread_id << ", sleep " << task_arg->loop << std::endl;
    std::cout.flush();

    auto duration = std::chrono::seconds(task_arg->loop);
    std::this_thread::sleep_for(duration);
    std::cout << "End of task looper2 " << task_arg->thread_id << std::endl;
    std::cout.flush();
}

int main()
{
    ThreadPool thread_pool(3);

    std::srand(std::time(nullptr));
    for (int i = 0; i < 3; ++i) {
        TaskArg1* task_arg = new TaskArg1;
        task_arg->thread_id = i;
        task_arg->loop = std::rand() % 100;

        Task task;
        task.arg = task_arg;
        task.run = looper1;

        thread_pool.addTask(task);
    }

    for (int i = 3; i < 6; ++i) {
        TaskArg2* task_arg = new TaskArg2;
        task_arg->thread_id = i;
        task_arg->loop = std::rand() % 1000;
        task_arg->abs = std::rand() / 10;

        Task task;
        task.arg = task_arg;
        task.run = looper2;

        thread_pool.addTask(task);
    }

    std::this_thread::sleep_for(10s);
    return 0;
}
