#include "storageManager.h"

StorageManager::StorageManager()
= default;

bool StorageManager::isConfigDirectoryReadable()
{
    QDir dataDirectory(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    if(!dataDirectory.exists()){
        if(!dataDirectory.mkdir(QStandardPaths::writableLocation(QStandardPaths::DataLocation))){
            return false;
        }
    }

    QDir autoSaveDirectory(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/autosave");
    if(!autoSaveDirectory.exists()){
        if (!autoSaveDirectory.mkdir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/autosave")) {
            return false;
        }
    }

    return QFileInfo(dataDirectory.absolutePath()).isWritable() && QFileInfo(autoSaveDirectory.absolutePath()).isWritable();
}
