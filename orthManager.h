#ifndef ORTHMANAGER_H
#define ORTHMANAGER_H

#include <QDialog>

#include "dadaword.h"

namespace Ui {
class OrthManager;
}

class OrthManager : public QDialog
{
    Q_OBJECT
    
public:
    explicit OrthManager(QWidget *parent, QTextEdit *contenu);
    ~OrthManager();
    
private:
    Ui::OrthManager *ui;
};

#endif // ORTHMANAGER_H
