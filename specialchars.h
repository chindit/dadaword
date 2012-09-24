#ifndef SPECIALCHARS_H
#define SPECIALCHARS_H

#include <QDialog>
#include <QTableWidget>

#include "dadaword.h"

class specialChars : public QDialog
{
    Q_OBJECT
public:
    explicit specialChars(QWidget *parent = 0);
    void showWindow(QList<QChar> listRecent);
    QString getChars();
    QList<QChar> getList();
    
signals:
    
public slots:

private slots:
    void updateTable(int row, int coll);

private:
    QTableWidget *tableau;
    QTableWidget *recentChars;
    QLineEdit *lineChars;
    QList<QChar> liste;
    
};

#endif // SPECIALCHARS_H
