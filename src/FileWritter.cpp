#define _HAS_STD_BYTE 0  // Fix Windows SDK byte conflict

// FileWriter.cpp
#include "FileWritter.hpp"
#include <iostream>

using namespace std;

FileWriter::FileWriter(const string &filePath)
{
    fileStream.open(filePath, ofstream::out | ofstream::binary | ofstream::trunc);
    if (!fileStream.is_open())
    {
        cerr << "Failed to open file: " << filePath << endl;
    }
}

FileWriter::~FileWriter()
{
    if (fileStream.is_open())
    {
        fileStream.flush(); // CRITICAL: Flush before closing
        fileStream.close();
    }
}

int FileWriter::write(char *data, int size)
{
    if (!fileStream.is_open() || !fileStream.good())
    {
        cerr << "File not opened or stream is bad" << endl;
        return 0; // Return 0 to tell curl to abort
    }
    
    fileStream.write(data, size);
    
    if (fileStream.fail())
    {
        cerr << "Error writing to file" << endl;
        return 0; // Return 0 to tell curl to abort
    }
    
    // Flush periodically to ensure data is written
    fileStream.flush();
    
    return size; // Return actual bytes written
}

void FileWriter::close()
{
    if (fileStream.is_open())
    {
        fileStream.flush(); // Flush before closing
        fileStream.close();
    }
}