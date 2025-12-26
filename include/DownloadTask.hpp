// DownloadTask.hpp
#ifndef DOWNLOADTASK_HPP
#define DOWNLOADTASK_HPP

#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include "FileWritter.hpp"
#include <curl/curl.h>
#include <iostream>
#include <mutex>

using namespace std;

enum class DownloadStatus
{
    Starting,
    Pending,
    Downloading,
    Paused,
    Completed,
    Failed
};

class DownloadTask
{
private:
    string url;
    string destinationPath;
    atomic<DownloadStatus> status; // Make atomic for thread safety
    atomic<float> progress; // Make atomic for thread safety

    CURL *curlHandle;

public:
    FileWriter *writer;
    DownloadTask(const string &url, const string &destination);
    ~DownloadTask();
    bool getStartCommand() const;
    string getUrl() const;
    void setStartCommand();
    void start();
    void pause();
    void resume();
    void cancel();
    DownloadStatus getStatus() const;
    float getProgress() const;
    void updateProgress(float newProgress); // Add this method
};

#endif // DOWNLOADTASK_HPP