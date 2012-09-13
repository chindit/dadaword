/*
  Développeur : David Lumaye (littletiger58@gmail.com)
  Date : 16/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#ifndef OUTILS_H
#define OUTILS_H

#include <QApplication>
#include <QCheckBox>
#include <QtConcurrentMap>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QSettings>
#include <QVariant>

#include <fstream>
#include <iostream>

#include "dadaword.h"

class Outils : public QObject{

    Q_OBJECT

public:

public slots:
    void fenetre_config();
    QVariant lire_config(QString nom);
    void enregistre_config(QString nom = "null", int valeur = 0);
    void enregistre_fichiers(QString fichier);
    QStringList fichiers_recents();
    void affiche_log();

private slots:
    bool clean_log();
    void returnDir();

private:
    QWidget *log_fen;
    QWidget *configure_fen;
    QCheckBox *checkbox_onglets, *checkbox_fichiers_vides, *checkbox_word, *checkbox_orthographe;
    QLineEdit *saving_edit;
    QSpinBox *taille_police_default, *spinbox_timer;
    QFontComboBox *police_default;
    QComboBox *alertes, *liste_dicos;

};

#endif // OUTILS_H
