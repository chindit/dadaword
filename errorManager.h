/*
  Développeur : David Lumaye (littletiger58@gmail.com)
  Date : 01/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#ifndef ERREUR_H
#define ERREUR_H

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>
#include <QTextStream>

#include "constantes.h"

class ErrorManager : public QObject{

    Q_OBJECT

public:
    ErrorManager(int alertes = HIGH);
    void Erreur_msg(QString msg, int etat);

public slots:


private:
    void Affiche_msg(QString msg, int etat);

    int lvlAlertes;
};

#endif // ERREUR_H
