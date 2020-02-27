#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

class StorageManager
{
public:
    StorageManager();
    static bool isConfigDirectoryReadable();
};

#endif // STORAGE_MANAGER_H
