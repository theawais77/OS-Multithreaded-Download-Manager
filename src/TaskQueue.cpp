#define _HAS_STD_BYTE 0  // Fix Windows SDK byte conflict

// TaskQueue.cpp
#include "TaskQueue.hpp"

using namespace std;

void TaskQueue::addTask(const shared_ptr<DownloadTask> &task)
{
    lock_guard<std::mutex> lock(mutex);
    queue.push(task);
}

shared_ptr<DownloadTask> TaskQueue::getNextTask()
{
    lock_guard<std::mutex> lock(mutex);
    if (queue.empty())
    {
        return nullptr;
    }
    auto task = queue.front();
    queue.pop();
    return task;
}

bool TaskQueue::isEmpty()
{
    lock_guard<std::mutex> lock(mutex);
    return queue.empty();
}