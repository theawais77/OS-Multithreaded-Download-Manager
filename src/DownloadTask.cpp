#define _HAS_STD_BYTE 0  // Fix Windows SDK byte conflict

// DownloadTask.cpp
#include "DownloadTask.hpp"
#include <fstream>

using namespace std;

static mutex progress_mutex;

// Helper function to write data received from libcurl
static size_t write_data(void *ptr, size_t size, size_t nmemb, DownloadTask *task)
{
    size_t total_size = size * nmemb;
    int written = task->writer->write(static_cast<char *>(ptr), total_size);
    
    // If write failed, return 0 to abort the transfer
    if (written != total_size)
    {
        cerr << "Write failed! Expected " << total_size << " but wrote " << written << endl;
        return 0;
    }
    
    return written;
}

// Progress callback function (using new XFERINFO API)
static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    DownloadTask *task = static_cast<DownloadTask *>(clientp);
    
    // Check if download should be paused or cancelled
    if (task->getStatus() == DownloadStatus::Paused)
    {
        return 0; // Continue but paused
    }
    
    if (task->getStatus() == DownloadStatus::Failed)
    {
        return 1; // Abort download
    }
    
    if (dltotal > 0)
    {
        double progress = static_cast<double>(dlnow) / static_cast<double>(dltotal);
        task->updateProgress(progress); // Update internal progress
        
        string url = task->getUrl();
        string filename = url.substr(url.find_last_of('/') + 1);
        
        lock_guard<mutex> lock(progress_mutex);
        
        // Show progress every 10%
        int percentage = static_cast<int>(progress * 100);
        static int last_percentage = -1;
        
        if (percentage != last_percentage && (percentage % 10 == 0 || percentage == 100))
        {
            cout << "[" << filename << "] " << percentage << "% downloaded ("
                 << dlnow / 1024 << " KB / " << dltotal / 1024 << " KB)" << endl;
            last_percentage = percentage;
        }
    }
    
    return 0;
}

DownloadTask::DownloadTask(const string &url, const string &destination)
    : url(url), destinationPath(destination), status(DownloadStatus::Pending), progress(0.0f)
{
    writer = new FileWriter(destinationPath);
    curlHandle = curl_easy_init();
    
    if (!curlHandle)
    {
        cerr << "Failed to initialize CURL handle" << endl;
        status = DownloadStatus::Failed;
    }
}

void DownloadTask::start()
{
    string filename = url.substr(url.find_last_of('/') + 1);
    cout << "\n[STARTING] " << filename << "\n";
    
    status = DownloadStatus::Downloading;
    progress = 0.0f;

    if (!curlHandle)
    {
        status = DownloadStatus::Failed;
        cout << "\n[FAILED] " << filename << " - CURL handle not initialized\n";
        return;
    }

    // Set CURL options
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curlHandle, CURLOPT_XFERINFOFUNCTION, progress_callback);
    curl_easy_setopt(curlHandle, CURLOPT_XFERINFODATA, this);
    curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0L); // For HTTPS
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curlHandle, CURLOPT_USERAGENT, "Mozilla/5.0");
    
    // Perform the download
    CURLcode res = curl_easy_perform(curlHandle);
    
    // Close and flush the file
    writer->close();
    
    if (res == CURLE_OK)
    {
        status = DownloadStatus::Completed;
        progress = 1.0f;
        cout << "\n[COMPLETED] " << filename << " - Download finished successfully!\n";
    }
    else
    {
        status = DownloadStatus::Failed;
        cout << "\n[FAILED] " << filename << " - Error: " << curl_easy_strerror(res) << "\n";
    }
}

string DownloadTask::getUrl() const
{
    return url;
}

bool DownloadTask::getStartCommand() const
{
    return (status == DownloadStatus::Starting);
}

void DownloadTask::setStartCommand()
{
    if (status == DownloadStatus::Pending || status == DownloadStatus::Failed)
    {
        status = DownloadStatus::Starting;
    }
    else
    {
        cout << "Download is already in progress or completed" << endl;
    }
}

void DownloadTask::pause()
{
    if (status == DownloadStatus::Downloading)
    {
        status = DownloadStatus::Paused;
        cout << "Download paused (note: current transfer will complete first)" << endl;
    }
    else
    {
        cout << "Download is not in progress" << endl;
    }
}

void DownloadTask::resume()
{
    if (status != DownloadStatus::Paused)
    {
        cout << "Download is not paused" << endl;
        return;
    }
    else
    {
        status = DownloadStatus::Downloading;
        cout << "Download resumed" << endl;
    }
}

void DownloadTask::cancel()
{
    status = DownloadStatus::Failed;
    cout << "Download cancelled" << endl;
}

DownloadStatus DownloadTask::getStatus() const
{
    return status;
}

float DownloadTask::getProgress() const
{
    return progress;
}

void DownloadTask::updateProgress(float newProgress)
{
    progress = newProgress;
}

DownloadTask::~DownloadTask()
{
    if (curlHandle)
    {
        curl_easy_cleanup(curlHandle);
    }
    delete writer;
}