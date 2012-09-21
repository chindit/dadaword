#ifndef Settings_H
#define Settings_H

#include <QDir>
#include <QFont>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>

#include "constantes.h"

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit SettingsManager(QObject *parent = 0);
    QVariant getSettings(Setting s);
    
signals:
    
public slots:

private:
    void loadSettings();
    QVariant *settings;
    QString *names;
    
};

#endif // Settings_H
