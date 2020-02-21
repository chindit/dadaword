#include "storagemanager.h"

StorageManager::StorageManager()
{

}

bool StorageManager::isConfigDirectoryReadable()
{
    QDir dataDirectory(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    if(!dataDirectory.exists()){
        if(!dataDirectory.mkdir(QStandardPaths::writableLocation(QStandardPaths::DataLocation))){
            return false;
        }
        else{
            QDir dir_autosave(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/autosave");
            if(!dir_autosave.exists()){
                if (!dir_autosave.mkdir(QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/autosave")) {
                    return false;
                }
            }
        }
    }

    return QFileInfo(dataDirectory.path()).isWritable();
}
