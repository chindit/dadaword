#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

class StorageManager
{
public:
    StorageManager();
    static bool isConfigDirectoryReadable();
};

#endif // STORAGEMANAGER_H
