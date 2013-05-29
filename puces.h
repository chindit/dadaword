#ifndef PUCES_H
#define PUCES_H

#include <QDialog>

#include "errorManager.h"

namespace Ui {
class Puces;
}

class Puces : public QDialog
{
    Q_OBJECT
    
public:
    explicit Puces(QWidget *parent = 0);
    ~Puces();
    QString getRadio();
    bool isClosed();

private slots:
    void close2();
    
private:
    Ui::Puces *ui;
    bool globalState;
};

#endif // PUCES_H
