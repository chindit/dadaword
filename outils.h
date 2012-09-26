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
#include "settingsManager.h"

class Outils : public QObject{

    Q_OBJECT

public:
    explicit Outils();

public slots:
    void fenetre_config();
    void enregistre_config();
    void enregistre_fichiers(QString fichier);
    QStringList fichiers_recents();
    void affiche_log();

private slots:
    bool clean_log();
    void returnDir();

private:
    SettingsManager *settings;
    QWidget *log_fen;
    QWidget *configure_fen;
    QCheckBox *checkbox_onglets, *checkbox_fichiers_vides, *checkbox_word, *checkbox_orthographe, *checkbox_autocorrection;
    QLineEdit *saving_edit;
    QSpinBox *taille_police_default, *spinbox_timer;
    QFontComboBox *police_default;
    QComboBox *alertes, *liste_dicos, *liste_themes;

};

#endif // OUTILS_H
