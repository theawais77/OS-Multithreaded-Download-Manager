// DownloadManager.hpp
#ifndef DOWNLOADMANAGER_HPP
#define DOWNLOADMANAGER_HPP

#include "ThreadPool.hpp"
#include <string>
#include <unordered_map>

using namespace std;

class DownloadManager
{
private:
    ThreadPool threadPool;
    unordered_map<string, shared_ptr<DownloadTask>> tasks;
    mutex taskMutex;

public:
    DownloadManager(size_t threadCount);
    void startDownloads();
    void addDownload(const string &url, const string &destinationPath);
    void startDownload(const string &url);
    void pauseDownload(const string &url);
    void resumeDownload(const string &url);
    void cancelDownload(const string &url);
    DownloadStatus getDownloadStatus(const string &url);
    void waitForCompletion();
    void clearTasks();
};

#endif // DOWNLOADMANAGER_HPP