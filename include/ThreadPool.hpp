// ThreadPool.hpp
#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <vector>
#include <thread>
#include <atomic>
#include <functional>
#include "TaskQueue.hpp"

using namespace std;

class ThreadPool
{
private:
    vector<thread> workers;
    TaskQueue taskQueue;
    atomic<bool> stopFlag;

    void workerFunction();

public:
    ThreadPool(size_t threads);
    ~ThreadPool();
    void enqueueTask(const shared_ptr<DownloadTask> &task);
    void shutdown();
};

#endif // THREADPOOL_HPP