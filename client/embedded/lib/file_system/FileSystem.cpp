#include "FileSystem.h"
#include "util/Logger.h"

FileSystem::FileSystem(bool deleteOnFailed) 
{
    this->deleteOnFailed = deleteOnFailed;
};

bool FileSystem::Begin() 
{
    return LittleFS.begin(deleteOnFailed);
}

File FileSystem::OpenFile(String path) 
{
    Logger::log("FileSystem", Logger::LogLevel::INFO, "Opened file: %s", path.c_str());
    return LittleFS.open(path, FILE_READ);
}

bool FileSystem::FileExists(String path) 
{
    return LittleFS.exists(path);    
}

int FileSystem::GetFreeSpace() 
{
    return 0;
}

fs::FS* FileSystem::GetInternalFileSystem() 
{
    return &LittleFS;
}

FileSystem::~FileSystem()
{

}