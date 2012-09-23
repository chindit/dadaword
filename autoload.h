#ifndef AUTOLOAD_H
#define AUTOLOAD_H

#include <QListWidgetItem>
#include <QWidget>

#include "dadaword.h"

class autoLoad : public QDialog
{
    Q_OBJECT
public:
    explicit autoLoad(QWidget *parent = 0);
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
