#ifndef ORTHMANAGER_H
#define ORTHMANAGER_H

#include <QDialog>

namespace Ui {
class OrthManager;
}

class OrthManager : public QDialog
{
    Q_OBJECT
    
public:
    explicit OrthManager(QWidget *parent = 0);
    ~OrthManager();
    
private:
    Ui::OrthManager *ui;
};

#endif // ORTHMANAGER_H
