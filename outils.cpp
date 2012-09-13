/*
  Développeur : David Lumaye (littletiger58@gmail.com)
  Date : 16/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#include "constantes.h"
#include "outils.h"

void Outils::fenetre_config(){
    //Création de la fenêtre de configuration
    configure_fen = new QWidget;
    configure_fen->setWindowModality(Qt::ApplicationModal);
    configure_fen->setAttribute(Qt::WA_DeleteOnClose);

    //Paramètres de la fenêtre
    configure_fen->setWindowIcon(QIcon(":/menu/images/outils.png"));
    configure_fen->setWindowTitle(tr("Configurer Dadaword"));

    QListWidget *settingsList = new QListWidget;
    settingsList->setMaximumWidth(100);
    QStackedWidget *settingsView = new QStackedWidget(configure_fen);
    QPushButton *fermer = new QPushButton;
    fermer->setText(tr("Fermer la fenêtre"));
    fermer->setIcon(QIcon(":/menus/images/exit.png"));
    connect(fermer, SIGNAL(clicked()), configure_fen, SLOT(close()));
    QPushButton *valider = new QPushButton;
    valider->setText(tr("Valider"));
    valider->setIcon(QIcon(":/menus/images/ok.png"));
    valider->setToolTip(tr("Enregistrer la configuration"));
    connect(valider, SIGNAL(clicked()), this, SLOT(enregistre_config()));
    QGridLayout *settingsWidgetLayout = new QGridLayout(configure_fen);

    //Connect QStackedWidget to QListWidget
    connect(settingsList, SIGNAL(currentRowChanged(int)), settingsView, SLOT(setCurrentIndex(int)));

    QListWidgetItem *itemEdit = new QListWidgetItem(QIcon(":/programme/images/textedit.png"), tr("Édition"));
    QListWidgetItem *itemGeneral = new QListWidgetItem(QIcon(":/programme/images/configure.png"), tr("Général"));
    settingsList->addItem(itemEdit);
    settingsList->addItem(itemGeneral);

    settingsWidgetLayout->addWidget(settingsList, 0, 0, 1, 1);
    settingsWidgetLayout->addWidget(settingsView, 0, 1, 1, 2);
    settingsWidgetLayout->addWidget(valider, 1, 1);
    settingsWidgetLayout->addWidget(fermer, 1, 2);

    QLabel *affiche_outils, *affiche_taille_police, *affiche_nom_police, *label_fichiers_vides, *label_alertes, *label_word, *label_dicos, *label_orthographe, *label_timer, *label_saving;
    affiche_outils = new QLabel(tr("Remplacer les onglets par des fenêtres"));
    affiche_taille_police = new QLabel(tr("Taille de la police par défaut"));
    affiche_nom_police = new QLabel(tr("Type de police par défaut"));
    label_fichiers_vides = new QLabel(tr("Autoriser l'ouverture de fichiers vides"));
    label_alertes = new QLabel(tr("Afficher les alertes"));
    label_orthographe = new QLabel(tr("Activer la correction orthographique"));
    label_word = new QLabel(tr("Activer la mise en page type \"Word\""));
    label_dicos = new QLabel(tr("Langue du dictionnaire"));
    label_timer = new QLabel(tr("Sauvegarde automatique (en secondes)"));
    label_saving = new QLabel(tr("Répertoire d'enregistrement par défaut"));
    label_alertes->setToolTip(tr("Cette action va active/désactive les alertes du programme"));
    label_word->setToolTip(tr("Activer une mise en page type \"Word\""));
    label_dicos->setToolTip(tr("Change la langue du dictionnaire par défaut (modifiable pour le document courant via l'option du menu \"Outils\")"));
    label_orthographe->setToolTip(tr("Active le surlignement automatique des erreurs dans le document courant"));
    label_timer->setToolTip(tr("Intervalle de temps avant de lancer une sauvegarde automatique du document."));

    checkbox_onglets = new QCheckBox;
    checkbox_fichiers_vides = new QCheckBox;
    checkbox_word = new QCheckBox;
    checkbox_orthographe = new QCheckBox;
    //On y met la configuration déjà existante :
    checkbox_onglets->setChecked(lire_config("onglets").toBool());
    checkbox_fichiers_vides->setChecked(lire_config("fichiers_vides").toBool());
    checkbox_orthographe->setChecked(lire_config("orthographe").toBool());
    checkbox_word->setChecked(lire_config("word").toBool());

    alertes = new QComboBox;
    alertes->addItem(tr("Aucunes"), LOW);
    alertes->addItem(tr("Indispensables"), MEDIUM);
    alertes->addItem(tr("Toutes"), HIGH);

    saving_edit = new QLineEdit;
    saving_edit->setText(lire_config("enregistrement").toString());
    saving_edit->setEnabled(false);
    QPushButton *changeSaving = new QPushButton("…");
    changeSaving->setMaximumWidth(25);
    connect(changeSaving, SIGNAL(clicked()), this, SLOT(returnDir()));

    liste_dicos = new QComboBox;
    QDir dossier;
    dossier.setPath("/usr/share/hunspell");
    QStringList extentions;
    extentions << "*.dic";
    QStringList dicos = dossier.entryList(extentions);
    //On récupère le dico actuel
    QString nom_dico = lire_config("dico").toString();
    for(int i=0; i<dicos.size(); i++){
        QString temp = dicos.at(i);
        temp.resize((temp.size()-4));
        liste_dicos->addItem(temp);
        //On présélectionne la langue actuelle
        if(temp == nom_dico){
            liste_dicos->setCurrentIndex(i);
        }
    }

    //Lecture des valeurs
    int result = lire_config("alertes").toInt();
    switch(result){
    case LOW:
        alertes->setCurrentIndex(0);
        break;
    case MEDIUM:
        alertes->setCurrentIndex(1);
        break;
    case HIGH:
        alertes->setCurrentIndex(2);
        break;
    default:
        Erreur instance_erreur;
        instance_erreur.Erreur_msg(tr("Exception dans la valeur de l'alerte"), QMessageBox::Warning);
    }

    taille_police_default = new QSpinBox;
    taille_police_default->setValue(lire_config("taille").toInt());
    spinbox_timer = new QSpinBox;
    spinbox_timer->setMaximum(1000);
    spinbox_timer->setMinimum(1);
    spinbox_timer->setValue(lire_config("timer").toInt());
    police_default = new QFontComboBox;
    police_default->setCurrentFont(lire_config("police").value<QFont>());

    //-------------------------------------------------
    // Édition
    //-------------------------------------------------
    QWidget *fen_edition = new QWidget(settingsView);
    settingsView->addWidget(fen_edition);
    QGridLayout *layoutEdition = new QGridLayout(fen_edition);
    layoutEdition->addWidget(affiche_nom_police, 0, 0);
    layoutEdition->addWidget(police_default, 0, 1);
    layoutEdition->addWidget(affiche_taille_police, 1, 0);
    layoutEdition->addWidget(taille_police_default, 1, 1);
    layoutEdition->addWidget(label_dicos, 2, 0);
    layoutEdition->addWidget(liste_dicos, 2, 1);
    layoutEdition->addWidget(label_word, 3, 0);
    layoutEdition->addWidget(checkbox_word, 3, 1);
    layoutEdition->addWidget(label_orthographe, 4, 0);
    layoutEdition->addWidget(checkbox_orthographe, 4, 1);

    //---------------------------------------------------
    // Général
    //---------------------------------------------------
    QWidget *fen_general = new QWidget(settingsView);
    settingsView->addWidget(fen_general);
    QGridLayout *layoutGeneral = new QGridLayout(fen_general);
    layoutGeneral->addWidget(label_fichiers_vides, 0, 0);
    layoutGeneral->addWidget(checkbox_fichiers_vides, 0, 1);
    layoutGeneral->addWidget(affiche_outils, 1, 0);
    layoutGeneral->addWidget(checkbox_onglets, 1, 1);
    layoutGeneral->addWidget(label_alertes, 2, 0);
    layoutGeneral->addWidget(alertes, 2, 1);
    layoutGeneral->addWidget(label_timer, 3, 0);
    layoutGeneral->addWidget(spinbox_timer, 3, 1);
    layoutGeneral->addWidget(label_saving, 4, 0);
    QHBoxLayout *layoutSaving = new QHBoxLayout;
    layoutSaving->addWidget(saving_edit);
    layoutSaving->addWidget(changeSaving);
    layoutGeneral->addWidget(label_saving, 4, 0);
    layoutGeneral->addLayout(layoutSaving, 4, 1);

    configure_fen->move((QApplication::desktop()->width() - configure_fen->width())/2, (QApplication::desktop()->height() - configure_fen->height())/2);
    configure_fen->show();
    return;
}

void Outils::enregistre_config(QString nom, int valeur){
    //Enregistrement des paramètres par des QSettings
    QSettings settings("Dadaword", "dadaword");
    if(nom != "null"){
        settings.setValue(nom, valeur);
        return;
    }
    //S'il y a eu un changement, on affiche une alerte.
    if((lire_config("onglets").toBool() && !checkbox_onglets->isChecked()) || (!lire_config("onglets").toBool() && checkbox_onglets->isChecked()) || (lire_config("word").toBool() && !checkbox_word->isChecked()) || (!lire_config("word").toBool() && checkbox_word->isChecked())){
        //Avertissement
        if(lire_config("alertes").toInt() != LOW){
            QMessageBox::information(0, tr("Configuration modifiée"), tr("Attention, la modification de certains paramètres ne sera prise en compte que lors du redémarrage du programme."));
        }
    }//Fin des "alertes"

    settings.setValue("onglets", checkbox_onglets->isChecked());
    settings.setValue("fichiers_vides", checkbox_fichiers_vides->isChecked());
    settings.setValue("police", police_default->currentFont());
    settings.setValue("taille", taille_police_default->value());
    settings.setValue("alertes", alertes->itemData(alertes->currentIndex()));
    settings.setValue("orthographe", checkbox_orthographe->isChecked());
    settings.setValue("word", checkbox_word->isChecked());
    settings.setValue("dico", liste_dicos->currentText());
    settings.setValue("timer", spinbox_timer->value());
    settings.setValue("enregistrement", saving_edit->text());

    configure_fen->close();
    return;
}

//Lire une valeur booléenne dans la configuration
QVariant Outils::lire_config(QString nom){
    QSettings settings("Dadaword", "dadaword");
    //On teste la valeur
    if(nom == "taille" || nom == "timer"){
        bool ok = false;
        settings.value(nom).toInt(&ok);
        if(!ok){
            if(nom == "taille"){
                return QVariant(12);
            }
            else{
                return QVariant(300);
            }
        }
    }
    return settings.value(nom);
}

//Enregistre un fichier ouvert dans les «récemment ouverts»
void Outils::enregistre_fichiers(QString fichier){
    //Lecture des préférences
    QSettings settings("Dadaword", "dadaword");
    //On récupère les derniers fichiers

    QStringList fichiers_recents = settings.value("recents").toStringList();
    //S'il y en a 10, on vire le premier (donc le plus ancien)
    if(fichiers_recents.size() >= 10){
        fichiers_recents.removeFirst();
    }
    //On ajoute la nouvelle URL
    fichiers_recents.append(fichier);

    //On enregistre les valeurs
    settings.setValue("recents", QVariant(fichiers_recents));

    return;
}

//Lire les fichiers récemment ouverts
QStringList Outils::fichiers_recents(){
    //Lecture des préférences
    QSettings settings("Dadaword", "dadaword");
    //On récupère les derniers fichiers
    return settings.value("recents").toStringList();
}

//Affiche la fenêtre de gestion du log
void Outils::affiche_log(){
    //Création de la fenêtre de configuration
    log_fen = new QWidget;
    log_fen->setWindowModality(Qt::ApplicationModal);

    //Paramètres de la fenêtre
    log_fen->setWindowIcon(QIcon(":/menu/images/log.png"));
    log_fen->setWindowTitle(tr("Gestion du log de DadaWord"));

    QGridLayout layout;

    //Labels
    QLabel *titre = new QLabel(tr("<h1>Gestion du log</h1>"));
    QPushButton *clean_log, *close_window;
    QTextEdit *contenu_log;
    close_window = new QPushButton;
    clean_log = new QPushButton;
    contenu_log = new QTextEdit;

    //Remplissage des boutons
    close_window->setText(tr("Fermer la fenêtre"));
    close_window->setIcon(QIcon(":/menus/images/sortir.png"));
    clean_log->setText(tr("Nettoyer le log"));
    clean_log->setIcon(QIcon(":/menus/images/balai.png"));
    clean_log->setToolTip(tr("Ne laisse que les dix dernières entrées dans le fichier le log"));

    //Remplissage du text edit
    QString place_log = QDir::homePath();
    place_log += "/.dadaword/dadaword.log";
    QFile fichier(place_log);
    QString contenu = "";
    int nb_lignes = 0;
    if(fichier.open(QIODevice::ReadOnly)){
        QTextStream stream(&fichier);
        while(!stream.atEnd()){
             contenu.insert(0, (stream.readLine()+"\n"));
             nb_lignes++;
        }
        fichier.close();
        //On met dans le QTextEdit
        contenu_log->setText(contenu);
    }

    //Layout
    layout.addWidget(titre, 0, 0, 1, 2, Qt::AlignHCenter);
    layout.addWidget(contenu_log, 1, 0, 1, 2);
    layout.addWidget(clean_log, 2, 0, 1, 1);
    layout.addWidget(close_window, 2, 1, 1, 1);

    log_fen->setLayout(&layout);

    //Connects
    connect(close_window, SIGNAL(clicked()), log_fen, SLOT(close()));
    connect(clean_log, SIGNAL(clicked()), this, SLOT(clean_log()));

    //Affichage de la fenêtre
    log_fen->move((QApplication::desktop()->width() - log_fen->width())/2, (QApplication::desktop()->height() - log_fen->height())/2);
    log_fen->show();
    return;
}

//Nettoye le log
bool Outils::clean_log(){
    QString place_log = QDir::homePath();
    place_log += "/.dadaword/dadaword.log";
    QFile fichier(place_log);
    QString contenu = "";
    if(fichier.open(QIODevice::ReadOnly)){
        QTextStream stream(&fichier);
        while(!stream.atEnd()){
            contenu.append(stream.readLine());
            contenu.append("\n");
        }
        fichier.close();
    }

    //Calcul des lignes restantes
    QStringList liste_erreurs = contenu.split("\n", QString::SkipEmptyParts);
    if(liste_erreurs.size() <= ITEMS_LOG){
        //On ne fait rien, il n'y a pas assez de données dans le fichier de log
        return true;
    }
    Erreur instance_erreur;

    //On broie l'ancien fichier de log
    fichier.resize(0);

    for(int i=(liste_erreurs.size() - ITEMS_LOG ); i<liste_erreurs.size(); i++){
        QString message = liste_erreurs.at(i);
        message = message.remove(0, 24);
        instance_erreur.Erreur_msg(message, QMessageBox::Ignore);
    }
    Outils instance;
    if(instance.lire_config("alertes").toInt() == HIGH){
        QMessageBox::information(0, tr("Nettoyage effectué"), tr("Le nettoyage du fichie de log a été effectué avec succès"));
    }
    else{
        //On ne fait rien
    }

    return true;
}

//Return a directory
void Outils::returnDir(){
    saving_edit->setText(QFileDialog::getExistingDirectory(0, tr("Dossier d'enregistrement"), QDir::homePath()));
    return;
}
