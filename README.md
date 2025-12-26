#  Multithreaded Download Manager (C++)

A C++ multithreaded download manager demonstrating core Operating System concepts such as thread pools, task scheduling, synchronization, and concurrency.
The project uses CMake for build configuration and libcurl for HTTP downloads.

##  Features

- Concurrent file downloads using a thread pool
- Thread-safe task queue
- Efficient CPU utilization
- Cross-platform build using CMake
- External dependency management using vcpkg

## Tech Stack

- **Language:** C++ 
- **Build System:** CMake
- **Compiler:** MSVC 
- **Library:** libcurl
- **Dependency Manager:** vcpkg
- **OS:** Windows 11

##  Project Structure

```
Multithread_DownloadManager/
├── main.cpp
├── CMakeLists.txt
├── include/
├── src/
└── build/
```

##  Build & Run (Windows)

### 1️ Install Dependencies

```bash
vcpkg install curl:x64-windows
```

### 2️ Configure Project

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### 3️ Build

```bash
cmake --build build
```

### 4️ Run

```bash
.\build\Debug\download_manager.exe
```

##  OS Concepts Demonstrated

- **Multithreading:** Parallel execution of download tasks
- **Thread Pool:** Reusable worker threads for efficient resource utilization
- **Task Queue:** Producer-consumer pattern for task distribution
- **Synchronization:** Mutex and condition variables for thread-safe operations
- **Resource Management:** RAII principles for proper cleanup
