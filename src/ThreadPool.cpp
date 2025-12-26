#define _HAS_STD_BYTE 0  // Fix Windows SDK byte conflict

// ThreadPool.cpp
#include "ThreadPool.hpp"
#include <iostream>

using namespace std;

ThreadPool::ThreadPool(size_t threads) : stopFlag(false)
{
    for (size_t i = 0; i < threads; ++i)
    {
        workers.emplace_back(&ThreadPool::workerFunction, this);
    }
}

ThreadPool::~ThreadPool()
{
    shutdown();
}

void ThreadPool::enqueueTask(const shared_ptr<DownloadTask> &task)
{
    taskQueue.addTask(task);
}

void ThreadPool::workerFunction()
{
    while (!stopFlag)
    {
        auto task = taskQueue.getNextTask();
        
        if (task != nullptr)
        {
            // Check if task should be started
            if (task->getStartCommand())
            {
                // Execute the task - this blocks until download completes/fails
                task->start();
                // Task is done, don't re-add it to queue
            }
            else
            {
                // Task not ready to start yet
                DownloadStatus status = task->getStatus();
                
                // Only re-add if still pending or starting
                if (status == DownloadStatus::Pending || status == DownloadStatus::Starting)
                {
                    taskQueue.addTask(task);
                    // Sleep briefly to avoid busy-waiting
                    this_thread::sleep_for(chrono::milliseconds(100));
                }
                // If completed, failed, paused, or downloading - don't re-add
            }
        }
        else
        {
            // No task available, sleep briefly
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}

void ThreadPool::shutdown()
{
    stopFlag = true;
    for (auto &worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}