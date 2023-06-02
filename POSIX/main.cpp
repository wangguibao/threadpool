#include "threadpool.h"
#include <iostream>
#include <unistd.h>

void print_value(int value) {
    std::cout << value << std::endl;
}

int main() {
    ThreadPool thread_pool(10);

    for (size_t i = 0; i < 100; ++i) {
        Task task;
        task.value = i;
        task.run = print_value;

        thread_pool.add_task(task);
    }

    std::cout << "Added 100 tasks" << std::endl;
    sleep(1);
    std::cout << "main exit" << std::endl;
    return 0;
}
