#ifndef AUTOLOAD_H
#define AUTOLOAD_H

#include <QIcon>
#include <QListWidgetItem>
#include <QWidget>

#include "dadaword.h"

class autoLoad : public QDialog
{
    Q_OBJECT
public:
    explicit autoLoad();
    ~autoLoad();
    bool hasFilesNames();
    QStringList getFilesNames();
    
signals:
    
public slots:

private:
    void showFiles();

    QStringList names;
    QListWidgetItem *items;
    
};

#endif // AUTOLOAD_H
