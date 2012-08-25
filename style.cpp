#include "style.h"

//Constructeur
Style::Style(QWidget *parent) :
    QDialog(parent){
    //Constructeur
    //On initialise les styles s'ils n'existent pas

    bool conversion = false;
    QStringList liste_noms;
    QSettings settings("Dadaword", "dadaword");
    int nb_styles = settings.value("nb_styles").toInt(&conversion);

    if(!conversion || nb_styles < 7){
        //On procède à la création des styles
        int taille_defaut = settings.value("taille").toInt();
        QFont police_defaut = settings.value("police").value<QFont>();

        //Nombre de styles
        settings.setValue("nb_styles", 7);
        //Nom des styles
        liste_noms << tr("Défaut") << tr("Titre 1") << tr("Titre 2") << tr("Titre 3") << tr("Titre 4") << tr("Titre 5") << tr("Titre 6");
        settings.setValue("noms_styles", liste_noms);

        //Défaut
        settings.beginGroup("Défaut");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", taille_defaut);
        settings.setValue("gras", QFont::Normal);
        settings.setValue("italique", false);
        settings.setValue("souligne", false);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::transparent));
        settings.endGroup();

        //Titre 1
        settings.beginGroup("Titre 1");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 24);
        settings.setValue("gras", QFont::Bold);
        settings.setValue("italique", false);
        settings.setValue("souligne", false);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::transparent));
        settings.endGroup();

        //Titre 2
        settings.beginGroup("Titre 2");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 20);
        settings.setValue("gras", QFont::Bold);
        settings.setValue("italique", true);
        settings.setValue("souligne", false);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::transparent));
        settings.endGroup();

        //Titre 3
        settings.beginGroup("Titre 3");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 16);
        settings.setValue("gras", QFont::Bold);
        settings.setValue("italique", false);
        settings.setValue("souligne", true);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::transparent));
        settings.endGroup();

        //Titre 4
        settings.beginGroup("Titre 4");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 15);
        settings.setValue("gras", QFont::Bold);
        settings.setValue("italique", true);
        settings.setValue("souligne", true);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::transparent));
        settings.endGroup();

        //Titre 5
        settings.beginGroup("Titre 5");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 14);
        settings.setValue("gras", QFont::Normal);
        settings.setValue("italique", true);
        settings.setValue("souligne", true);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::transparent));
        settings.endGroup();

        //Titre 6
        settings.beginGroup("Titre 6");
        settings.setValue("police", police_defaut);
        settings.setValue("taille", 13);
        settings.setValue("gras", QFont::Normal);
        settings.setValue("italique", false);
        settings.setValue("souligne", true);
        settings.setValue("foreground", QColor(Qt::black));
        settings.setValue("background", QColor(Qt::transparent));
        settings.endGroup();
    }
}

//Affiche la fenêtre du style
void Style::affiche_fen(){
    QSettings settings("Dadaword", "dadaword");


    int nb_styles = settings.value("nb_styles").toInt();
    QStringList liste_noms = settings.value("noms_styles").toStringList();

    //Création des éléments de la fenêtre
    QGridLayout *layout = new QGridLayout;
    setLayout(layout);
    QLabel *titre = new QLabel(tr("<h1>Gestion des styles</h1>"));    titre->setMargin(25);
    QLabel *noms[nb_styles], *titre_nom, *titre_modifier, *titre_supprimer;
    titre_nom = new QLabel("<b>Nom</b>");
    titre_modifier = new QLabel("<b>Modifier</b>");
    titre_supprimer = new QLabel("<b>Supprimer</b>");
    QPushButton *modifier[nb_styles], *supprimer[nb_styles], *this_exit, *this_validate, *this_add;
    this_exit = new QPushButton(QIcon(":/menus/images/exit.png"), tr("Fermer"));
    this_validate = new QPushButton(QIcon(":/menus/images/ok.png"), tr("Valider"));
    this_add = new QPushButton(QIcon(":/programme/images/ajouter.png"), tr("Nouveau style"));

    connect(this_exit, SIGNAL(clicked()), this, SLOT(close()));
    connect(this_add, SIGNAL(clicked()), this, SLOT(ajoute_style()));

    layout->addWidget(titre, 0, 0, 1, 3, Qt::AlignHCenter);
    layout->addWidget(titre_nom, 1, 0);
    layout->addWidget(titre_modifier, 1, 1);
    layout->addWidget(titre_supprimer, 1, 2);

    //Initialisations
    for(int i=0; i<nb_styles; i++){
        noms[i] = new QLabel;
        noms[i]->setText(liste_noms.at(i));
        modifier[i] = new QPushButton;
        modifier[i]->setIcon(QIcon(":/programme/images/edit.png"));
        modifier[i]->setText(tr("Modifier"));
        supprimer[i] = new QPushButton;
        supprimer[i]->setIcon(QIcon(":/menus/images/sortir.png"));
        supprimer[i]->setText(tr("Supprimer"));
        if(i < 7){
            //Interdiction de supprimer les styles par défaut
            supprimer[i]->setEnabled(false);
        }
        //On ajoute au layout
        layout->addWidget(noms[i], i+2, 0);
        layout->addWidget(modifier[i], i+2, 1);
        layout->addWidget(supprimer[i], i+2, 2);
    }
    //Ajout des boutons
    layout->addWidget(this_add, nb_styles+2, 1, 1, 2, Qt::AlignHCenter);
    QHBoxLayout *layout_horizontal = new QHBoxLayout;
    layout_horizontal->addWidget(this_validate);
    layout_horizontal->addWidget(this_exit);
    layout->addLayout(layout_horizontal, nb_styles+3, 0, 1, 3, Qt::AlignHCenter);

    setWindowTitle(tr("Gestion des styles - Dadaword"));
    setWindowIcon(QIcon(":/programme/images/dadaword.gif"));
    setAttribute(Qt::WA_DeleteOnClose);
    exec();

    return;
}

//Crée un nouveau style
void Style::ajoute_style(){
    QDialog *add_style = new QDialog();
    add_style->setWindowIcon(QIcon(":/programme/dadaword.gif"));
    add_style->setWindowTitle(tr("Ajouter un nouveau style - Dadaword"));
    add_style->setWindowModality(Qt::ApplicationModal);

    QGridLayout *layout = new QGridLayout;
    add_style->setLayout(layout);

    //Initialisations
    QLabel *titre, *label_nom, *label_police, *label_taille, *label_gras, *label_italique, *label_souligne, *label_foreground, *label_background;
    titre = new QLabel(tr("<h3>Créer un nouveau style</h3>"));
    label_nom = new QLabel(tr("Nom du style"));
    label_police = new QLabel(tr("Police"));
    label_taille = new QLabel(tr("Taille"));
    label_gras = new QLabel(tr("Gras"));
    label_italique = new QLabel(tr("Italique"));
    label_souligne = new QLabel(tr("Souligné"));
    label_foreground = new QLabel(tr("Couleur du texte"));
    label_background = new QLabel(tr("Couleur d'arrière-plan"));

    line_edite_nom_style = new QLineEdit;
    combo_police = new QFontComboBox;
    box_taille = new QSpinBox;
    checkbox_gras = new QCheckBox;
    checkbox_souligne = new QCheckBox;
    checkbox_italique = new QCheckBox;
    color_foreground = new QPushButton;
    color_background = new QPushButton;
    QPalette palette_background;
    palette_background.setColor(QPalette::Window, Qt::transparent);
    color_background->setPalette(palette_background);
    QPalette palette_foreground;
    palette_foreground.setColor(QPalette::WindowText, Qt::black);
    color_foreground->setText(tr("Couleur"));
    color_foreground->setPalette(palette_foreground);

    QPushButton *valider = new QPushButton(QIcon(":/menus/images/ok.png"), tr("Valider"));
    QPushButton *annuler = new QPushButton(QIcon(":/menus/images/stop.png"), tr("Annuler"));
    connect(annuler, SIGNAL(clicked()), add_style, SLOT(close()));

    //On ajoute tout au layout
    layout->addWidget(titre, 0, 0, 1, 2, Qt::AlignHCenter);
    layout->addWidget(label_nom, 1, 0);
    layout->addWidget(line_edite_nom_style, 1, 1);
    layout->addWidget(label_police, 2, 0);
    layout->addWidget(combo_police, 2, 1);
    layout->addWidget(label_taille, 3, 0);
    layout->addWidget(box_taille, 3, 1);
    layout->addWidget(label_gras, 4, 0);
    layout->addWidget(checkbox_gras, 4, 1);
    layout->addWidget(label_souligne, 5, 0);
    layout->addWidget(checkbox_souligne, 5, 1);
    layout->addWidget(label_italique, 6, 0);
    layout->addWidget(checkbox_italique, 6, 1);
    layout->addWidget(label_foreground, 7, 0);
    layout->addWidget(color_foreground, 7, 1);
    layout->addWidget(label_background, 8, 0);
    layout->addWidget(color_background, 8, 1);
    layout->addWidget(valider, 9, 0);
    layout->addWidget(annuler, 9, 1);

    //Exécution
    add_style->exec();

    return;
}
