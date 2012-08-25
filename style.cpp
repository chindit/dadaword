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
    exec();

    return;
}
