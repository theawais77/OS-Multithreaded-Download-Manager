// FileWriter.hpp
#ifndef FILEWRITER_HPP
#define FILEWRITER_HPP

#include <fstream>
#include <string>

using namespace std;

class FileWriter
{
private:
    ofstream fileStream;

public:
    FileWriter(const string &filePath);
    ~FileWriter();
    int write(char *data, int size);
    void close();
};

#endif // FILEWRITER_HPP