#ifndef __POST_RUNNABLES_HPP__
#define __POST_RUNNABLES_HPP__

#include <queue>
#include <mutex>
#include "delegates.hpp"

class PostRunnables {
    std::queue<runnable_t> runnables;
    std::recursive_mutex mutex;
public:
    void postRunnable(runnable_t task) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        runnables.push(std::move(task));
    }

    void run() {
        std::queue<runnable_t> tasksToRun;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex);
            std::swap(tasksToRun, runnables);
        }

        while (!tasksToRun.empty()) {
            auto& task = tasksToRun.front();
            task();
            tasksToRun.pop();
        }
    }
};

#endif
