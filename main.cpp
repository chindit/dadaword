/*
  @author : David Lumaye (littletiger58@gmail.com)
  @date : 27/02/19
  @license : GPLv3
  DO NOT REMOVE THIS NOTICE
  */

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QTextCodec>
#include <QSplashScreen>

#include "dadaword.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    QSplashScreen screen(QPixmap(":/images/loading.png"));
    screen.show();
    QApplication::processEvents();

    // Loading locales
    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApplication::installTranslator(&translator);
    
    //-----------------------------------------------------
    // Check if another mainWindow is running
    //-----------------------------------------------------
    QApplication::setApplicationName("Dadaword");
    QSharedMemory sharedMemory(QApplication::applicationName());
    
    // If memory zone already exists, it means program is already running
    if(!sharedMemory.create(sizeof(int))){
#ifndef QT_DEBUG
        QMessageBox::warning(nullptr, QObject::tr("Programme en cours d'exécution"), QObject::tr("Dadaword est déjà en cours d'exécution.  Veuillez fermer l'mainWindow ouverte avant de le lance à nouveau."));
        exit(EXIT_SUCCESS);
#endif
    }

    DadaWord mainWindow;

    // Creating UI
    mainWindow.createUI();

    // Display
    mainWindow.show();
    screen.finish(&mainWindow);

    // Start main loop
    return QApplication::exec();
}
