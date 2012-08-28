/*
  Développeur : David Lumaye (littletiger58@gmail.com)
  Date : 21/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#include "erreur.h"

Erreur::Erreur()
{
}

void Erreur::Affiche_msg(QString msg, int etat){
    Outils instance_outils;

    if(etat == QMessageBox::Critical){
            QMessageBox::critical(0, tr("Erreur fatale"), msg);
    }
    else if(etat == QMessageBox::Warning){
        if(instance_outils.lire_config("alertes").toInt() != LOW){
            QMessageBox::warning(0, tr("Alerte"), msg);
        }
    }
    else if(etat == QMessageBox::Information){
        if(instance_outils.lire_config("alertes").toInt() == LOW){
            QMessageBox::information(0, tr("Erreur légère"), msg);
        }
    }
    else if(etat == QMessageBox::Ignore){
        //On veut juste le marquer dans le log, pas faire ch**** l'utilisateur
        //Donc on ne fait rien vu que le log a déjà été mis à jour
    }
    else{
        //Une erreur dans l'erreur, c'est le comble
        QMessageBox::critical(0, tr("Erreur fatale"), tr("Une erreur inconnue est survenue.\n  Par mesure de précaution, le programme va maintenant se fermer."));
        exit(EXIT_FAILURE);
    }
    return;
}

void Erreur::Erreur_msg(QString msg, int etat){
    //1)Inscription dans le log
    //On stocke dans le fichier de log
    //Emplacement du fichier de LOG
#if defined(Q_OS_WIN32)
    QString place_log = QDir::currentPath()+"/dadaword.log";
#else
    QString place_log = QDir::homePath()+"/.dadaword/dadaword.log";
#endif
    QFile fichier(place_log);
    if(fichier.open(QIODevice::Append | QIODevice::Text)){
        QDateTime datetime = QDateTime::currentDateTime();
        QString date = datetime.toString("yyyy-MM-dd : hh:mm:ss");
        QString message = date+" - "+msg.replace("\n", "");
        QTextStream flux(&fichier);
        flux << message << "\n";
        fichier.close();
    }

    //2)Affichage du message
    Affiche_msg(msg, etat);
    return;
}
