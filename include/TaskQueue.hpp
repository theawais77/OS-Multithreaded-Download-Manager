// TaskQueue.hpp
#ifndef TASKQUEUE_HPP
#define TASKQUEUE_HPP

#include <queue>
#include <mutex>
#include <memory>
#include "DownloadTask.hpp"

using namespace std;

class TaskQueue
{
private:
    queue<shared_ptr<DownloadTask>> queue;
    mutex mutex;

public:
    void addTask(const shared_ptr<DownloadTask> &task);
    shared_ptr<DownloadTask> getNextTask();
    bool isEmpty();
};

#endif // TASKQUEUE_HPP