#define _HAS_STD_BYTE 0  // Fix Windows SDK byte conflict

#include <iostream>
#include <vector>
#include "DownloadManager.hpp"
#include "DownloadTask.hpp"
#include "FileWritter.hpp"
#include "TaskQueue.hpp"
#include "ThreadPool.hpp"
#include <curl/curl.h>
#include <thread>
#include <algorithm>

using namespace std;

class DownloadApplication
{
public:
    DownloadApplication() : stopFlag(false), manager(5)
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);

        // Run the cleanup thread in background
        t2 = thread(&DownloadApplication::usualCleanup, this);
    }

    ~DownloadApplication()
    {
        stopFlag = true;
        if (t1.joinable())
        {
            t1.join();
        }

        if (t2.joinable())
        {
            t2.join();
        }

        manager.waitForCompletion();
        curl_global_cleanup();
    }

    void startAllDownloads()
    {
        if (t1.joinable())
        {
            t1.join();
        }
        cout << "\nStarting all downloads...\n";
        t1 = thread([this]()
                         { manager.startDownloads(); });
        this_thread::sleep_for(chrono::milliseconds(500)); // Let downloads start
        cout << "Downloads initiated. Check progress below.\n\n";
    }

    void startDownload()
    {
        int urlNo;
        if (t1.joinable())
        {
            t1.join();
        }
        showDownloadList();
        cout << "Enter the url number to start: ";
        cin >> urlNo;
        if (urlNo > filesToDownload.size() || urlNo < 1)
        {
            cout << "Invalid url number\n";
            return;
        }
        cout << "\nStarting download...\n";
        t1 = thread([this, urlNo]()
                    { manager.startDownload(filesToDownload[urlNo - 1]); });
        this_thread::sleep_for(chrono::milliseconds(500)); // Let download start
        cout << "Download initiated. Check progress below.\n\n";
    }

    void pauseDownload()
    {
        int urlNo;
        showDownloadList();
        cout << "Enter the url number to pause: ";
        cin >> urlNo;
        if (urlNo > filesToDownload.size() || urlNo < 1)
        {
            cout << "Invalid url number\n";
            return;
        }
        manager.pauseDownload(filesToDownload[urlNo - 1]);
    }

    void resumeDownload()
    {
        int urlNo;
        showDownloadList();
        cout << "Enter the url number to resume: ";
        cin >> urlNo;
        if (urlNo > filesToDownload.size() || urlNo < 1)
        {
            cout << "Invalid url number\n";
            return;
        }
        manager.resumeDownload(filesToDownload[urlNo - 1]);
    }

    void cancelDownload()
    {
        int urlNo;
        showDownloadList();
        cout << "Enter the url number to cancel: ";
        cin >> urlNo;
        if (urlNo > filesToDownload.size() || urlNo < 1)
        {
            cout << "Invalid url number\n";
            return;
        }
        manager.cancelDownload(filesToDownload[urlNo - 1]);
    }

    void removeDownload()
    {
        int urlNo;
        vector<string>::iterator it;
        showDownloadList();
        cout << "Enter the url number to remove: ";
        cin >> urlNo;
        it = find(filesToDownload.begin(), filesToDownload.end(), filesToDownload[urlNo - 1]);
        // If element is found, erase it
        if (it != filesToDownload.end())
        {
            filesToDownload.erase(it);
        }
        else
        {
            cout << "Url not found in download list\n";
        }
    }

    void addFilesToDownload(string url)
    {
        filesToDownload.push_back(url);
        manager.addDownload(url, url.substr(url.find_last_of('/') + 1));
    }

    void CLITest()
    {
        string url;

        while (1)
        {
            printMenu();
            int option;
            cin >> option;
            switch (option)
            {
            case 1:
                cout << "Enter the url to add: ";
                cin >> url;
                addFilesToDownload(url);
                showDownloadList();
                break;
            case 2:
                removeDownload();
                break;
            case 3:
                startAllDownloads();
                break;
            case 4:
                startDownload();
                break;
            case 5:
                pauseDownload();
                break;
            case 6:
                resumeDownload();
                break;
            case 7:
                cancelDownload();
                break;
            case 8:
                stopFlag = true;
                return;
            default:
                cout << "Invalid option. Exiting...\n";
                stopFlag = true;
                return;
            }
        }
    }

private:
    DownloadManager manager; // Start with 5 threads in the thread pool
    vector<string> filesToDownload;
    thread t1, t2;
    bool stopFlag;

    string downloadStatusToString(DownloadStatus status)
    {
        switch (status)
        {
        case DownloadStatus::Starting:
            return "Starting";
        case DownloadStatus::Pending:
            return "Pending";
        case DownloadStatus::Downloading:
            return "Downloading";
        case DownloadStatus::Paused:
            return "Paused";
        case DownloadStatus::Completed:
            return "Completed";
        case DownloadStatus::Failed:
            return "Failed";
        // Add other cases as needed
        default:
            return "Unknown";
        }
    }

    void showDownloadList()
    {
        int i = 1;
        for (const auto &url : filesToDownload)
        {
            if (manager.getDownloadStatus(url) == DownloadStatus::Completed)
            {
                // Remove the completed downloads from the list
                filesToDownload.erase(remove(filesToDownload.begin(), filesToDownload.end(), url), filesToDownload.end());
            }
        }
        cout << "\n--- Current Downloads ---\n";
        if (filesToDownload.empty())
        {
            cout << "No downloads in queue.\n";
            return;
        }
        for (const auto &url : filesToDownload)
        {
            string filename = url.substr(url.find_last_of('/') + 1);
            cout << i++ << ". " << filename << " [" << downloadStatusToString(manager.getDownloadStatus(url)) << "]\n";
        }
        cout << "-------------------------\n";
    }

    void usualCleanup()
    {
        while (!stopFlag)
        {
            // clear the downloaded or failed tasks
            this_thread::sleep_for(chrono::seconds(2)); // Check every 2 seconds
            manager.clearTasks();
        }
    }

    void printMenu()
    {
        cout << "\n========== Download Manager ==========\n";
        cout << "1. Add file to download list\n";
        cout << "2. Remove file from download list\n";
        cout << "3. Start all downloads\n";
        cout << "4. Start a download\n";
        cout << "5. Pause a download\n";
        cout << "6. Resume a download\n";
        cout << "7. Cancel a download\n";
        cout << "8. Exit\n";
        cout << "======================================\n";
        cout << "Enter your choice: ";
    }
};

int main()
{
    DownloadApplication app;
    app.CLITest();
    return 0;
}


// https://ash-speed.hetzner.com/100MB.bin
// https://proof.ovh.net/files/100Mb.dat
// https://speed.hetzner.de/100MB.bin
// https://bouygues.testdebit.info/100M.iso
// http://ipv4.download.thinkbroadband.com/100MB.zip