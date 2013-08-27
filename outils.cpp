/*
  Développeur : David Lumaye (littletiger58.aro-base.gmail.com)
  Date : 16/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#include "constantes.h"
#include "outils.h"
#include "orthManager.h" //Include dans le CPP pour éviter des problèmes de non-connaissance de classe

Outils::Outils(){
}

void Outils::fenetre_config(){
    //Création de la fenêtre de configuration
    configure_fen = new QWidget;
    configure_fen->setWindowModality(Qt::ApplicationModal);
    configure_fen->setAttribute(Qt::WA_DeleteOnClose);

    //Paramètres de la fenêtre
    configure_fen->setWindowIcon(QIcon::fromTheme("preferences-system", QIcon(":/menus/images/outils.png")));
    configure_fen->setWindowTitle(tr("Configurer Dadaword"));

    //Settings
    settings = new SettingsManager;

    QListWidget *settingsList = new QListWidget;
    settingsList->setMaximumWidth(100);
    QStackedWidget *settingsView = new QStackedWidget(configure_fen);
    QPushButton *fermer = new QPushButton;
    QPushButton *button_reset = new QPushButton(tr("Réinitialiser"));
    connect(button_reset, SIGNAL(clicked()), settings, SLOT(resetSettings()));
    fermer->setText(tr("Fermer la fenêtre"));
    fermer->setIcon(QIcon::fromTheme("dialog-close", QIcon(":/menus/images/exit.png")));
    connect(fermer, SIGNAL(clicked()), configure_fen, SLOT(close()));
    QPushButton *valider = new QPushButton;
    valider->setText(tr("Valider"));
    valider->setIcon(QIcon::fromTheme("dialog-ok", QIcon(":/menus/images/ok.png")));
    valider->setToolTip(tr("Enregistrer la configuration"));
    connect(valider, SIGNAL(clicked()), this, SLOT(enregistre_config()));
    QGridLayout *settingsWidgetLayout = new QGridLayout(configure_fen);

    //Connect QStackedWidget to QListWidget
    connect(settingsList, SIGNAL(currentRowChanged(int)), settingsView, SLOT(setCurrentIndex(int)));

    QListWidgetItem *itemEdit = new QListWidgetItem(QIcon::fromTheme("document-edit", QIcon(":/programme/images/textedit.png")), tr("Édition"));
    QListWidgetItem *itemGeneral = new QListWidgetItem(QIcon::fromTheme("applications-development", QIcon(":/programme/images/configure.png")), tr("Général"));
    QListWidgetItem *itemRaccourcis = new QListWidgetItem(tr("Raccourcis"));
    settingsList->addItem(itemEdit);
    settingsList->addItem(itemGeneral);
    settingsList->addItem(itemRaccourcis);

    settingsWidgetLayout->addWidget(settingsList, 0, 0, 1, 1);
    settingsWidgetLayout->addWidget(settingsView, 0, 1, 1, 2);
    settingsWidgetLayout->addWidget(valider, 1, 1);
    settingsWidgetLayout->addWidget(fermer, 1, 2);

    QLabel *affiche_outils, *affiche_taille_police, *affiche_nom_police, *label_fichiers_vides, *label_alertes, *label_word, *label_dicos, *label_orthographe, *label_timer, *label_saving, *label_last_dir, *label_theme, *label_autocorrection, *label_icons, *label_titre, *label_reset;
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
    label_last_dir = new QLabel(tr("Sélectionner par défaut le dernier répertoire utilisé"));
    label_theme = new QLabel(tr("Thème d'icônes"));
    label_titre = new QLabel(tr("Désactiver les titres lors du retour à la ligne"));
    label_autocorrection = new QLabel(tr("Activer l'autocorrection"));
    label_icons = new QLabel(tr("Masquer les actions pour lesquelles le thème n'a pas d'icônes"));
    label_reset = new QLabel(tr("Réinitialiser les préférences"));
    label_alertes->setToolTip(tr("Cette action va active/désactive les alertes du programme"));
    label_word->setToolTip(tr("Activer une mise en page type \"Word\""));
    label_dicos->setToolTip(tr("Change la langue du dictionnaire par défaut (modifiable pour le document courant via l'option du menu \"Outils\")"));
    label_orthographe->setToolTip(tr("Active le surlignement automatique des erreurs dans le document courant"));
    label_timer->setToolTip(tr("Intervalle de temps avant de lancer une sauvegarde automatique du document."));
    label_theme->setToolTip(tr("Le thème définit les icones du programme."));
    label_autocorrection->setToolTip(tr("Active l'autocorrection pour les éléments listés dans le panel"));
    label_icons->setToolTip(tr("Ne masque que les icônes de la barre d'outils.  Les actions restent disponibles dans les menus"));
    label_titre->setToolTip(tr("Désactive automatiquement les styles de titre lors du retour à la ligne"));
    label_reset->setToolTip(tr("Réinitialise toutes les préférences"));
    label_last_dir->setToolTip(tr("Cette option ouvre par défaut le dernier dossier utilisé lors de l'ouverture d'un fichier.  Le dossier n'est gardé en mémoire que durant la session courante, il revient donc au dossier par défaut à chaque redémarrage du programme."));

    checkbox_onglets = new QCheckBox;
    checkbox_fichiers_vides = new QCheckBox;
    checkbox_word = new QCheckBox;
    checkbox_orthographe = new QCheckBox;
    checkbox_autocorrection = new QCheckBox;
    checkbox_icons = new QCheckBox;
    checkbox_titre = new QCheckBox;
    checkbox_dir = new QCheckBox;
    //On y met la configuration déjà existante :
    checkbox_onglets->setChecked(settings->getSettings(Onglets).toBool());
    checkbox_fichiers_vides->setChecked(settings->getSettings(FichiersVides).toBool());
    checkbox_orthographe->setChecked(settings->getSettings(Orthographe).toBool());
    checkbox_word->setChecked(settings->getSettings(Word).toBool());
    checkbox_autocorrection->setChecked(settings->getSettings(Autocorrection).toBool());
    checkbox_icons->setChecked(settings->getSettings(ToolbarIcons).toBool());
    checkbox_titre->setChecked(settings->getSettings(DelTitre).toBool());
    checkbox_dir->setChecked(settings->getSettings(UseDir).toBool());

    alertes = new QComboBox;
    alertes->addItem(tr("Aucunes"), LOW);
    alertes->addItem(tr("Indispensables"), MEDIUM);
    alertes->addItem(tr("Toutes"), HIGH);

    saving_edit = new QLineEdit;
    saving_edit->setText(settings->getSettings(Enregistrement).toString());
    saving_edit->setEnabled(false);
    QPushButton *changeSaving = new QPushButton("…");
    changeSaving->setMaximumWidth(25);
    connect(changeSaving, SIGNAL(clicked()), this, SLOT(returnDir()));

    liste_dicos = new QComboBox;
    QStringList dicos = OrthManager::getDicos();
    //On récupère le dico actuel
    QString nom_dico = settings->getSettings(Dico).toString();
    for(int i=0; i<dicos.size(); i++){
        QString temp = dicos.at(i);
        temp.resize((temp.size()-4));
        liste_dicos->addItem(temp);
        //On présélectionne la langue actuelle
        if(temp == nom_dico){
            liste_dicos->setCurrentIndex(i);
        }
    }

    liste_themes = new QComboBox;
    QStringList themePath = QIcon::themeSearchPaths();
    QDir dossier;
    for(int i=0; i<themePath.size(); i++){
        dossier.setPath(themePath.at(i));
        liste_themes->addItems(dossier.entryList(QDir::Dirs | QDir::NoDotAndDotDot));
    }
    for(int i=0; i<liste_themes->count(); i++){
        if(liste_themes->itemText(i) == settings->getSettings(Theme).toString()){
            liste_themes->setCurrentIndex(i);
            break;
        }
    }


    //Lecture des valeurs
    int result = settings->getSettings(Alertes).toInt();
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
        ErrorManager instance_erreur(settings->getSettings(Alertes).toInt());
        instance_erreur.Erreur_msg(tr("Exception dans la valeur de l'alerte"), QMessageBox::Warning);
    }

    taille_police_default = new QSpinBox;
    taille_police_default->setValue(settings->getSettings(Taille).toInt());
    spinbox_timer = new QSpinBox;
    spinbox_timer->setMaximum(1000);
    spinbox_timer->setMinimum(1);
    spinbox_timer->setValue(settings->getSettings(Timer).toInt());
    police_default = new QFontComboBox;
    police_default->setCurrentFont(settings->getSettings(Police).value<QFont>());

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
    layoutEdition->addWidget(label_autocorrection, 5, 0);
    layoutEdition->addWidget(checkbox_autocorrection, 5, 1);
    layoutEdition->addWidget(label_titre, 6, 0);
    layoutEdition->addWidget(checkbox_titre, 6, 1);

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
    layoutGeneral->addLayout(layoutSaving, 4, 1);
    layoutGeneral->addWidget(label_last_dir, 5, 0);
    layoutGeneral->addWidget(checkbox_dir, 5, 1);
    layoutGeneral->addWidget(label_theme, 6, 0);
    layoutGeneral->addWidget(liste_themes, 6, 1);
    layoutGeneral->addWidget(label_icons, 7, 0);
    layoutGeneral->addWidget(checkbox_icons, 7, 1);
    layoutGeneral->addWidget(label_reset, 8, 0);
    layoutGeneral->addWidget(button_reset, 8, 1);

    //---------------------------------------------------
    //Raccourcis
    //---------------------------------------------------
    QWidget *fen_raccourcis = new QWidget(settingsView);
    settingsView->addWidget(fen_raccourcis);
    QGridLayout *layoutRaccourcis = new QGridLayout(fen_raccourcis);
    QLabel *racc_titre, *racc_info, *racc_info_info, *racc_gras, *racc_italique, *racc_souligne, *racc_ouvrir, *racc_save, *racc_new;
    racc_titre = new QLabel(tr("<h3>Raccourcis de DadaWord</h3>"));
    racc_info = new QLabel(tr("Majuscule : <i>Shift</i>, Contrôle : <i>Ctrl</i>, Conjonction de touches : <i>+</i>"));
    racc_info_info = new QLabel(tr("Exemple : <i>Ctrl+Shift+F2"));
    racc_gras = new QLabel(tr("Gras"));
    racc_italique = new QLabel(tr("Italique"));
    racc_souligne = new QLabel(tr("Souligné"));
    racc_ouvrir = new QLabel(tr("Ouvrir"));
    racc_save = new QLabel(tr("Enregistrer"));
    racc_new = new QLabel(tr("Nouveau"));
    lr_gras = new QLineEdit;  lr_italique = new QLineEdit;  lr_souligne = new QLineEdit, lr_ouvrir = new QLineEdit, lr_save = new QLineEdit, lr_new = new QLineEdit;
    lr_gras->setText(settings->getSettings(RGras).toString());
    lr_italique->setText(settings->getSettings(RItalique).toString());
    lr_souligne->setText(settings->getSettings(RSouligne).toString());
    lr_ouvrir->setText(settings->getSettings(ROuvrir).toString());
    lr_save->setText(settings->getSettings(REnregistrer).toString());
    lr_new->setText(settings->getSettings(RNouveau).toString());
    layoutRaccourcis->addWidget(racc_titre, 0, 0, 1, 4, Qt::AlignHCenter);
    layoutRaccourcis->addWidget(racc_info, 1, 0, 1, 4, Qt::AlignLeft);
    layoutRaccourcis->addWidget(racc_info_info, 2, 0, 1, 4, Qt::AlignLeft);
    layoutRaccourcis->addWidget(racc_gras, 3, 0);
    layoutRaccourcis->addWidget(lr_gras, 3, 1);
    layoutRaccourcis->addWidget(racc_italique, 3, 2);
    layoutRaccourcis->addWidget(lr_italique, 3, 3);
    layoutRaccourcis->addWidget(racc_souligne, 4, 0);
    layoutRaccourcis->addWidget(lr_souligne, 4, 1);
    layoutRaccourcis->addWidget(racc_ouvrir, 4, 2);
    layoutRaccourcis->addWidget(lr_ouvrir, 4, 3);
    layoutRaccourcis->addWidget(racc_save, 5, 0);
    layoutRaccourcis->addWidget(lr_save, 5, 1);
    layoutRaccourcis->addWidget(racc_new, 5, 2);
    layoutRaccourcis->addWidget(lr_new, 5, 3);


    configure_fen->move((QApplication::desktop()->width() - configure_fen->width())/2, (QApplication::desktop()->height() - configure_fen->height())/2);
    configure_fen->show();
    return;
}

void Outils::enregistre_config(){
    //S'il y a eu un changement, on affiche une alerte.
    /*if((settings->getSettings(Onglets).toBool() && !checkbox_onglets->isChecked()) || (!settings->getSettings(Onglets).toBool() && checkbox_onglets->isChecked()) || (settings->getSettings(Word).toBool() && !checkbox_word->isChecked()) || (!settings->getSettings(Word).toBool() && checkbox_word->isChecked())){
        //Avertissement
        if(settings->getSettings(Alertes).toInt() != LOW){
            QMessageBox::information(0, tr("Configuration modifiée"), tr("Attention, la modification de certains paramètres ne sera prise en compte que lors du redémarrage du programme."));
        }
    }*/
    if(settings->getSettings(Alertes).toInt() == HIGH && settings->getSettings(Theme).toString() != liste_themes->currentText() && !liste_themes->currentText().contains("dadaword", Qt::CaseInsensitive)){
        QMessageBox::warning(0, tr("Thème changé"), tr("Vous avez choisi un thème qui n'a pas été vérifié.\nIl se peut qu'il manque certaines icônes."));
    }//Fin des "alertes"

    settings->setSettings(Onglets, checkbox_onglets->isChecked());
    settings->setSettings(FichiersVides, checkbox_fichiers_vides->isChecked());
    settings->setSettings(Police, police_default->currentFont());
    settings->setSettings(Taille, taille_police_default->value());
    settings->setSettings(Alertes, alertes->itemData(alertes->currentIndex()));
    settings->setSettings(Orthographe, checkbox_orthographe->isChecked());
    settings->setSettings(Word, checkbox_word->isChecked());
    settings->setSettings(Dico, liste_dicos->currentText());
    settings->setSettings(Timer, spinbox_timer->value());
    if(!saving_edit->text().isEmpty())
        settings->setSettings(Enregistrement, saving_edit->text());
    if(!liste_themes->currentText().isEmpty())
        settings->setSettings(Theme, liste_themes->currentText());
    settings->setSettings(ToolbarIcons, checkbox_icons->isChecked());
    settings->setSettings(DelTitre, checkbox_titre->isChecked());
    settings->setSettings(UseDir, checkbox_dir->isChecked());
    settings->setSettings(RGras, lr_gras->text());
    settings->setSettings(RItalique, lr_italique->text());
    settings->setSettings(RSouligne, lr_souligne->text());
    settings->setSettings(RNouveau, lr_new->text());
    settings->setSettings(ROuvrir, lr_ouvrir->text());
    settings->setSettings(REnregistrer, lr_save->text());

    //On émet le signal
    emit settingsUpdated();

    configure_fen->close();
    return;
}

//Affiche la fenêtre de gestion du log
void Outils::affiche_log(){
    //Création de la fenêtre de configuration
    log_fen = new QWidget;
    log_fen->setWindowModality(Qt::ApplicationModal);

    //Paramètres de la fenêtre
    log_fen->setWindowIcon(QIcon::fromTheme("text-x-log", QIcon(":/menu/images/log.png")));
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
    close_window->setIcon(QIcon::fromTheme("dialog-close", QIcon(":/menus/images/sortir.png")));
    clean_log->setText(tr("Nettoyer le log"));
    clean_log->setIcon(QIcon::fromTheme("edit-clear", QIcon(":/menus/images/balai.png")));
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
    ErrorManager instance_erreur;

    //On broie l'ancien fichier de log
    fichier.resize(0);

    for(int i=(liste_erreurs.size() - ITEMS_LOG ); i<liste_erreurs.size(); i++){
        QString message = liste_erreurs.at(i);
        message = message.remove(0, 24);
        instance_erreur.Erreur_msg(message, QMessageBox::Ignore);
    }
    if(settings->getSettings(Alertes).toInt() == HIGH){
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
