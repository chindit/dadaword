#include "style.h"

//Constructeur
Style::Style(QWidget *parent) :
    QDialog(parent){
    //Constructeur
    //On initialise les styles s'ils n'existent pas

    bool conversion = false;
    QSettings settings("Dadaword", "dadaword");
    nb_styles = settings.value("nb_styles").toInt(&conversion);
    id_modif = -225;

    if(!conversion || nb_styles < 7){
        //On procède à la création des styles
        int taille_defaut = settings.value("taille").toInt();
        QFont police_defaut = settings.value("police").value<QFont>();

        //Nombre de styles
        settings.setValue("nb_styles", 7);
        //Nom des styles
        liste_styles << tr("Défaut") << tr("Titre 1") << tr("Titre 2") << tr("Titre 3") << tr("Titre 4") << tr("Titre 5") << tr("Titre 6");
        settings.setValue("noms_styles", liste_styles);

        //Défaut
        settings.beginGroup("Défaut");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", taille_defaut);
        settings.setValue("gras", QFont::Normal);
        settings.setValue("italique", false);
        settings.setValue("souligne", false);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::white));
        settings.endGroup();

        //Titre 1
        settings.beginGroup("Titre 1");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 24);
        settings.setValue("gras", QFont::Bold);
        settings.setValue("italique", false);
        settings.setValue("souligne", false);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::white));
        settings.endGroup();

        //Titre 2
        settings.beginGroup("Titre 2");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 20);
        settings.setValue("gras", QFont::Bold);
        settings.setValue("italique", true);
        settings.setValue("souligne", false);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::white));
        settings.endGroup();

        //Titre 3
        settings.beginGroup("Titre 3");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 16);
        settings.setValue("gras", QFont::Bold);
        settings.setValue("italique", false);
        settings.setValue("souligne", true);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::white));
        settings.endGroup();

        //Titre 4
        settings.beginGroup("Titre 4");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 15);
        settings.setValue("gras", QFont::Bold);
        settings.setValue("italique", true);
        settings.setValue("souligne", true);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::white));
        settings.endGroup();

        //Titre 5
        settings.beginGroup("Titre 5");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 14);
        settings.setValue("gras", QFont::Normal);
        settings.setValue("italique", true);
        settings.setValue("souligne", true);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::white));
        settings.endGroup();

        //Titre 6
        settings.beginGroup("Titre 6");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 13);
        settings.setValue("gras", QFont::Normal);
        settings.setValue("italique", false);
        settings.setValue("souligne", true);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::white));
        settings.endGroup();
    }
    else{
        liste_styles = settings.value("noms_styles").toStringList();
    }
}

//Affiche la fenêtre du style
void Style::affiche_fen(){
    //On vérifie si la fenêtre est déjà initialisée
    if(this->layout() != 0){
        show();
        return;
    }

    //Sinon on la crée
    setWindowTitle(tr("Gestion des styles - Dadaword"));
    setWindowIcon(QIcon(":/programme/images/dadaword.gif"));

    QSettings settings("Dadaword", "dadaword");

    QListWidget *stylesList = new QListWidget(this);
    stylesList->setMaximumWidth(100);
    QStackedWidget *stylesView = new QStackedWidget(this);
    QPushButton *closeStyle = new QPushButton(QIcon(":/menus/images/exit.png"), tr("Fermer la fenêtre"));
    connect(closeStyle, SIGNAL(clicked()), this, SLOT(close()));
    QPushButton *saveStyle = new QPushButton(QIcon(":/menus/images/filesave.png"), tr("Enregistrer"));
    saveStyle->setToolTip(tr("Enregistrer les styles"));
    connect(saveStyle, SIGNAL(clicked()), this, SLOT(enregistre_style()));
    QPushButton *newStyle = new QPushButton(QIcon(":/programme/images/ajouter.png"), tr("Nouveau style"));
    QPushButton *deleteStyle = new QPushButton(QIcon(":/menus/images/sortir.png"), tr("Supprimer"));
    deleteStyle->setToolTip(tr("Supprime le style actuel"));
    QGridLayout *stylesWidgetLayout = new QGridLayout(this);
    stylesWidgetLayout->addWidget(stylesList, 0, 0);
    stylesWidgetLayout->addWidget(stylesView, 0, 1);
    QHBoxLayout *underLayout = new QHBoxLayout;
    underLayout->addWidget(newStyle);
    underLayout->addWidget(saveStyle);
    underLayout->addWidget(deleteStyle);
    underLayout->addWidget(closeStyle);
    stylesWidgetLayout->addLayout(underLayout, 1, 0, 1, 2, Qt::AlignHCenter);
    //Connect QStackedWidget to QListWidget
    connect(stylesList, SIGNAL(currentRowChanged(int)), stylesView, SLOT(setCurrentIndex(int)));
    connect(deleteStyle, SIGNAL(clicked()), this, SLOT(supprime_style()));
    connect(newStyle, SIGNAL(clicked()), this, SLOT(ajoute_style()));

    //-------------------------------------------------
    // Boucle d'initialisations
    //-------------------------------------------------
    for(int i=0; i<nb_styles; i++){
        //Création de l'item
        QListWidgetItem *itemEdit = new QListWidgetItem(liste_styles.at(i));
        stylesList->addItem(itemEdit);

        //Création du contenu
        QWidget *style = new QWidget(stylesView);
        stylesView->addWidget(style);
        QGridLayout *layoutStyle = new QGridLayout(style);
        QLabel *nom = new QLabel("<h3>"+liste_styles.at(i)+"</h3>");
        QLabel *label_police, *label_taille, *label_gras, *label_italique, *label_souligne, *label_foreground, *label_background;
        label_police = new QLabel(tr("Police"));
        label_taille = new QLabel(tr("Taille"));
        label_gras = new QLabel(tr("Gras"));
        label_italique = new QLabel(tr("Italique"));
        label_souligne = new QLabel(tr("Souligné"));
        label_foreground = new QLabel(tr("Couleur du texte"));
        label_background = new QLabel(tr("Couleur d'arrière-plan"));

        QPushButton *buttonForeground = new QPushButton;
        QPushButton *buttonBackground = new QPushButton;
        color_foreground.append(buttonForeground);
        color_background.append(buttonBackground);
        color_background.at(i)->setText(tr("Transparent"));
        color_foreground.at(i)->setText("#000000");

        QSignalMapper *mappeur = new QSignalMapper;
        QSignalMapper *mappeur2 = new QSignalMapper;
        connect(buttonForeground, SIGNAL(clicked()), mappeur, SLOT(map()));
        mappeur->setMapping(buttonForeground, 0);
        connect(mappeur, SIGNAL(mapped(int)), this, SLOT(change_couleur(int)));
        connect(buttonBackground, SIGNAL(clicked()), mappeur2, SLOT(map()));
        mappeur2->setMapping(buttonBackground, 1);
        connect(mappeur2, SIGNAL(mapped(int)), this, SLOT(change_couleur(int)));

        QCheckBox *boxBold = new QCheckBox;
        QCheckBox *boxItalic = new QCheckBox;
        QCheckBox *boxUnderline = new QCheckBox;
        boxGras.append(boxBold);
        boxSouligne.append(boxUnderline);
        boxItalique.append(boxItalic);
        QFontComboBox *comboPolice = new QFontComboBox;
        boxPolice.append(comboPolice);
        QSpinBox *spinSize = new QSpinBox;
        boxSize.append(spinSize);


        settings.beginGroup(liste_styles.at(i));
        boxPolice.at(i)->setCurrentFont(settings.value("police").value<QFont>());
        boxSize.at(i)->setValue(settings.value("taille").toInt());
        if(settings.value("gras").toInt() == QFont::Bold){
            boxGras.at(i)->setChecked(true);
        }
        boxItalique.at(i)->setChecked(settings.value("italique").toBool());
        boxSouligne.at(i)->setChecked(settings.value("souligne").toBool());
        color_foreground.at(i)->setText(settings.value("foreground").value<QColor>().name());
        color_background.at(i)->setText(settings.value("background").value<QColor>().name());
        settings.endGroup();

        //Ajout dans le layout
        layoutStyle->addWidget(nom, 0, 0, 1, 2, Qt::AlignHCenter);
        layoutStyle->addWidget(label_police, 1, 0);
        layoutStyle->addWidget(boxPolice.at(i), 1, 1);
        layoutStyle->addWidget(label_taille, 2, 0);
        layoutStyle->addWidget(boxSize.at(i), 2, 1);
        layoutStyle->addWidget(label_gras, 3, 0);
        layoutStyle->addWidget(boxGras.at(i), 3, 1);
        layoutStyle->addWidget(label_souligne, 4, 0);
        layoutStyle->addWidget(boxSouligne.at(i), 4, 1);
        layoutStyle->addWidget(label_italique, 5, 0);
        layoutStyle->addWidget(boxItalique.at(i), 5, 1);
        layoutStyle->addWidget(label_foreground, 6, 0);
        layoutStyle->addWidget(color_foreground.at(i), 6, 1);
        layoutStyle->addWidget(label_background, 7, 0);
        layoutStyle->addWidget(color_background.at(i), 7, 1);

    }

    exec();

    return;
}

//Crée un nouveau style
void Style::ajoute_style(){
    Erreur instance_erreur;

    QString styleName = QInputDialog::getText(this, tr("Nom du style"), tr("Nom du style : "));
    if(styleName.isEmpty()){
        instance_erreur.Erreur_msg(tr("Vous devez fournir un nom de style.\nAnnulation."), QMessageBox::Information);
        return;
    }
    //Ajout à la liste des styles
    liste_styles.append(styleName);

    //On ajoute au QListWidget et au QStackedWidget
    QStackedWidget *stackedWidget = this->findChild<QStackedWidget* >();
    QListWidget *listWidget = this->findChild<QListWidget* >();

    //Si on les détecte pas, on se casse
    if(listWidget == 0 || stackedWidget == 0){
        instance_erreur.Erreur_msg(tr("Impossible de récupérer le contenu de la fenêtre de style"), QMessageBox::Critical);
        return;
    }

    nb_styles++;

    //Ajout au QListWidget
    QListWidgetItem *itemEdit = new QListWidgetItem(styleName);
    listWidget->addItem(itemEdit);

    //Ajout au QStackedWidget
    QWidget *style = new QWidget(stackedWidget);
    stackedWidget->addWidget(style);
    QGridLayout *layoutStyle = new QGridLayout(style);
    QLabel *nom = new QLabel("<h3>"+styleName+"</h3>");
    QLabel *label_police, *label_taille, *label_gras, *label_italique, *label_souligne, *label_foreground, *label_background;
    label_police = new QLabel(tr("Police"));
    label_taille = new QLabel(tr("Taille"));
    label_gras = new QLabel(tr("Gras"));
    label_italique = new QLabel(tr("Italique"));
    label_souligne = new QLabel(tr("Souligné"));
    label_foreground = new QLabel(tr("Couleur du texte"));
    label_background = new QLabel(tr("Couleur d'arrière-plan"));

    QPushButton *buttonForeground = new QPushButton;
    QPushButton *buttonBackground = new QPushButton;
    color_foreground.append(buttonForeground);
    color_background.append(buttonBackground);
    color_background.at(nb_styles-1)->setText("#FFFFFF");
    color_foreground.at(nb_styles-1)->setText("#000000");

    QSignalMapper *mappeur = new QSignalMapper;
    QSignalMapper *mappeur2 = new QSignalMapper;
    connect(buttonForeground, SIGNAL(clicked()), mappeur, SLOT(map()));
    mappeur->setMapping(buttonForeground, 0);
    connect(mappeur, SIGNAL(mapped(int)), this, SLOT(change_couleur(int)));
    connect(buttonBackground, SIGNAL(clicked()), mappeur2, SLOT(map()));
    mappeur2->setMapping(buttonBackground, 1);
    connect(mappeur2, SIGNAL(mapped(int)), this, SLOT(change_couleur(int)));

    QCheckBox *boxBold = new QCheckBox;
    QCheckBox *boxItalic = new QCheckBox;
    QCheckBox *boxUnderline = new QCheckBox;
    boxGras.append(boxBold);
    boxSouligne.append(boxUnderline);
    boxItalique.append(boxItalic);
    QFontComboBox *comboPolice = new QFontComboBox;
    boxPolice.append(comboPolice);
    QSpinBox *spinSize = new QSpinBox;
    boxSize.append(spinSize);

    //Ajout dans le layout
    layoutStyle->addWidget(nom, 0, 0, 1, 2, Qt::AlignHCenter);
    layoutStyle->addWidget(label_police, 1, 0);
    layoutStyle->addWidget(boxPolice.at(nb_styles-1), 1, 1);
    layoutStyle->addWidget(label_taille, 2, 0);
    layoutStyle->addWidget(boxSize.at(nb_styles-1), 2, 1);
    layoutStyle->addWidget(label_gras, 3, 0);
    layoutStyle->addWidget(boxGras.at(nb_styles-1), 3, 1);
    layoutStyle->addWidget(label_souligne, 4, 0);
    layoutStyle->addWidget(boxSouligne.at(nb_styles-1), 4, 1);
    layoutStyle->addWidget(label_italique, 5, 0);
    layoutStyle->addWidget(boxItalique.at(nb_styles-1), 5, 1);
    layoutStyle->addWidget(label_foreground, 6, 0);
    layoutStyle->addWidget(color_foreground.at(nb_styles-1), 6, 1);
    layoutStyle->addWidget(label_background, 7, 0);
    layoutStyle->addWidget(color_background.at(nb_styles-1), 7, 1);

    //On affiche le style nouvellement créé
    stackedWidget->setCurrentWidget(style);

    return;
}

//Slot de changement de couleur
void Style::change_couleur(int bouton){

    QColor couleur = QColorDialog::getColor();
    QStackedWidget *stackedWidget = this->findChild<QStackedWidget* >();

    if(bouton == 0){
        color_foreground.at(stackedWidget->currentIndex())->setText(couleur.name());
    }
    else if(bouton == 1){
        color_background.at(stackedWidget->currentIndex())->setText(couleur.name());
    }
    else{
        Erreur instance_erreur;
        instance_erreur.Erreur_msg(tr("Style:: Exception dans le changement de couleur : pas de bouton spécifié"), QMessageBox::Warning);
        return;
    }
    return;
}

//Enregistre le style nouvellement crée
void Style::enregistre_style(){

    //S'il y a un champ vide, on arrête
    for(int i=0; i<nb_styles; i++){
        if(boxPolice.at(i)->currentText().isEmpty() || boxPolice.at(i)->currentText().isNull() || boxSize.at(i)->value() <= 0){
            QMessageBox::warning(this, tr("Champs non-remplis"), tr("Veuillez compléter les champs \"Police\" et \"Taille\" avant de poursuivre.\nMerci"));
            return;
        }
    }

    //S'il y a un bug, on arrête
    if(liste_styles.count() != nb_styles){
        Erreur instance_erreur;
        instance_erreur.Erreur_msg(tr("Une erreur est survenue lors de l'enregistrement des styles, une valeur erronée de l'index a sans doute été fournie."), QMessageBox::Warning);
        return;
    }

    QSettings settings("Dadaword", "dadaword");
    settings.setValue("nb_styles", nb_styles);
    settings.setValue("noms_styles", liste_styles);

    for(int i=0; i<nb_styles; i++){
        settings.beginGroup(liste_styles.at(i));
        settings.setValue("police", boxPolice.at(i)->currentFont());
        settings.setValue("taille", boxSize.at(i)->value());
        if(boxGras.at(i)->isChecked()){
            settings.setValue("gras", QFont::Bold);
        }
        else{
            settings.setValue("gras", QFont::Normal);
        }
        settings.setValue("italique", boxItalique.at(i)->isChecked());
        settings.setValue("souligne", boxSouligne.at(i)->isChecked());
        settings.setValue("foreground", QColor(color_foreground.at(i)->text()));
        if(color_background.at(i)->text() == tr("Transparent")){
            settings.setValue("background", QColor(Qt::transparent));
        }
        else{
            settings.setValue("background", QColor(color_background.at(i)->text()));
        }
        settings.endGroup();
    }

    if(settings.value("alertes").toInt() == HIGH){
        QMessageBox::information(this, tr("Enregistrement effectué"), tr("L'enregistrement des styles s'est correctement effectué"));
    }
    return;
}

//Supprime un style
void Style::supprime_style(){

    QStackedWidget *activeWidget = this->findChild<QStackedWidget* >();
    int reponse = QMessageBox::question(this, tr("Supprimer un style"), tr("Voulez vous vraiment supprimer le style %1 ?").arg(liste_styles.at(activeWidget->currentIndex())), QMessageBox::Yes | QMessageBox::No);
    if(reponse == QMessageBox::Yes){
        if(activeWidget->currentIndex() > 6){
            QSettings settings("Dadaword", "dadaword");

            //On supprime le style
            settings.beginGroup(liste_styles.at(activeWidget->currentIndex()));
            settings.remove("");
            settings.endGroup();

            liste_styles.removeAt(activeWidget->currentIndex());
            nb_styles--;

            //Mise à jour des settings
            settings.setValue("nb_styles", nb_styles);
            settings.setValue("noms_styles", liste_styles);

            //Suppression du QStackedWidget
            activeWidget->removeWidget(activeWidget->currentWidget());
            QListWidget *listWidget = this->findChild<QListWidget* >();
            listWidget->removeItemWidget(listWidget->currentItem());
            //On affiche le style par défaut
            activeWidget->setCurrentIndex(0);
            listWidget->setCurrentRow(0);

        }
        else{
            Outils instance_outils;
            if(instance_outils.lire_config("alertes").toInt() == HIGH){
                QMessageBox::warning(this, tr("Impossible de supprimer le style"), tr("Il s'agit d'un style par défaut, vous ne pouvez pas le supprimer.\nPar contre, vous pouvez le modifier comme vous le souhaitez."));
                return;
            }
        }
    }
    else if(reponse == QMessageBox::No){
        //Annulation du processus
        return;
    }
    else{
        Erreur instance;
        instance.Erreur_msg(tr("Exception dans la boite de confirmation de la suppression de style"), QMessageBox::Critical);
        return;
    }

    return;
}
