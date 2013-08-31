/*
  Développeur : David Lumaye (littletiger58.aro-base.gmail.com)
  Date : 02/04/13
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#include <QApplication>
#include <QLocale>
#include <QObject>
#include <QTranslator>
#include <QLibraryInfo>
#include <QTextCodec>
#include <QSplashScreen>
#include "dadaword.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    QSplashScreen screen(QPixmap(":/images/chargement.png"));
    screen.show();
    app.processEvents();

    //Traduction des boutons
    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);
    
    //-----------------------------------------------------
    // Vérification de l'existence d'une autre instance
    //-----------------------------------------------------
    app.setApplicationName("Dadaword");
    QSharedMemory sharedMemory(app.applicationName());
    
    // On vérifie à la création de cette zone mémoire si celle-ci existe
    if(sharedMemory.create(sizeof(int))==false){
        /*QMessageBox::warning(0, QObject::tr("Programme en cours d'exécution"), QObject::tr("Dadaword est déjà en cours d'exécution.  Veuillez fermer l'instance ouverte avant de le lance à nouveau."));
        exit(EXIT_SUCCESS);*/
    }

    DadaWord instance;

    //Création de l'interface utilisateur
    instance.createUI();

    //Affichage
    instance.show();
    screen.finish(&instance);

    return app.exec();
}
