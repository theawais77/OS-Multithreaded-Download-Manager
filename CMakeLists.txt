cmake_minimum_required(VERSION 3.16)

project(MultiThreadedDownloadManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix MSVC parallel build issue
if(MSVC)
    add_compile_options(/FS)
endif()

add_executable(download_manager
    main.cpp
    src/TaskQueue.cpp
    src/ThreadPool.cpp
    src/DownloadTask.cpp
    src/DownloadManager.cpp
    src/FileWritter.cpp
)

# Tell compiler where OUR headers are
target_include_directories(download_manager PRIVATE include)

# Threads
find_package(Threads REQUIRED)

# libcurl via vcpkg
find_package(CURL REQUIRED)

target_link_libraries(download_manager
    Threads::Threads
    CURL::libcurl
)
