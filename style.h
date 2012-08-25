#ifndef STYLE_H
#define STYLE_H

#include <QColor>
#include <QDialog>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QStringList>

class Style : public QDialog
{
    Q_OBJECT

public:
    explicit Style(QWidget *parent = 0);
    
signals:
    
public slots:
    void affiche_fen();
    
};

#endif // STYLE_H
