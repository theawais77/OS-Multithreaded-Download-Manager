#define _HAS_STD_BYTE 0  // Fix Windows SDK byte conflict

// DownloadManager.cpp
#include "DownloadManager.hpp"

using namespace std;

DownloadManager::DownloadManager(size_t threadCount) : threadPool(threadCount) {}

void DownloadManager::addDownload(const string &url, const string &destinationPath)
{
    lock_guard<mutex> lock(taskMutex);
    auto task = make_shared<DownloadTask>(url, destinationPath);
    tasks[url] = task;
    threadPool.enqueueTask(task);
}

void DownloadManager::startDownloads()
{
    lock_guard<mutex> lock(taskMutex);
    for (const auto &task : tasks)
    {
        task.second->setStartCommand();
    }
}

void DownloadManager::startDownload(const string &url)
{
    lock_guard<mutex> lock(taskMutex);
    if (tasks.find(url) != tasks.end())
    {
        tasks[url]->setStartCommand();
    }
    else
    {
        cout << "Url not in download queue" << endl;
    }
}

void DownloadManager::pauseDownload(const string &url)
{
    lock_guard<mutex> lock(taskMutex);
    if (tasks.find(url) != tasks.end())
    {
        tasks[url]->pause();
    }
    else
    {
        cout << "Url not in download queue" << endl;
    }
}

void DownloadManager::resumeDownload(const string &url)
{
    lock_guard<mutex> lock(taskMutex);
    if (tasks.find(url) != tasks.end())
    {
        tasks[url]->resume();
    }
    else
    {
        cout << "Url not in download queue" << endl;
    }
}

void DownloadManager::cancelDownload(const string &url)
{
    lock_guard<mutex> lock(taskMutex);
    if (tasks.find(url) != tasks.end())
    {
        tasks[url]->cancel();
    }
    else
    {
        cout << "Url not in download queue" << endl;
    }
}

DownloadStatus DownloadManager::getDownloadStatus(const string &url)
{
    lock_guard<mutex> lock(taskMutex);
    if (tasks.find(url) != tasks.end())
    {
        return tasks.at(url)->getStatus();
    }
    else
    {
        return DownloadStatus::Completed;
    }
}

void DownloadManager::waitForCompletion()
{
    lock_guard<mutex> lock(taskMutex);
    for (const auto &task : tasks)
    {
        while (task.second->getStatus() == DownloadStatus::Downloading)
        {
            // Wait for completion
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}

void DownloadManager::clearTasks()
{
    lock_guard<mutex> lock(taskMutex);
    if (!(tasks.empty()))
    {
        auto it = tasks.begin();
        while (it != tasks.end())
        {
            if (it->second->getStatus() == DownloadStatus::Completed)
            {
                string filename = it->first.substr(it->first.find_last_of('/') + 1);
                cout << "\n[CLEANUP] Removing completed task: " << filename << "\n";
                it = tasks.erase(it);
            }
            else if (it->second->getStatus() == DownloadStatus::Failed)
            {
                string filename = it->first.substr(it->first.find_last_of('/') + 1);
                cout << "\n[RETRY] Re-queuing failed task: " << filename << "\n";
                threadPool.enqueueTask(it->second);
                ++it;
            }
            else
            {
                ++it;
            }
        }
    }
}