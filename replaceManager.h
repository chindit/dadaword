#ifndef REPLACEMANAGER_H
#define REPLACEMANAGER_H

#include <QDialog>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>

#include "errorManager.h"
#include "settingsManager.h"

class ReplaceManager : public QDialog
{
    Q_OBJECT
public:
    explicit ReplaceManager(QWidget *parent = 0);
    ~ReplaceManager();

public slots:
    void showWindow();

private slots:
    void addLine();
    void saveKeys();

private:
    SettingsManager *manSettings;
    QList<QLineEdit *> lineKeys, lineValues;
    QGridLayout *layout;
    QPushButton *addLineButton, *saveAllButton;
    
};

#endif // REPLACEMANAGER_H
