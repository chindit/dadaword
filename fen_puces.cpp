/*
  Développeur : David Lumaye (littletiger58.aro-base.gmail.com)
  Date : 01/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#include "fen_puces.h"

fen_puces::fen_puces(QWidget *parent) :
    QDialog(parent)
{
    QDialog *puce = new QDialog;
    puce->setWindowModality(Qt::ApplicationModal);

    puce->setWindowTitle(tr("Type de liste"));
    puce->setWindowIcon(QIcon(":/menus/images/liste_ordonnee.png"));

    QVBoxLayout *layout_puce = new QVBoxLayout;
    QGroupBox *groupe = new QGroupBox(tr("Style de liste"));
    radio1 = new QRadioButton(tr("&Liste décimale"));
    radio2 = new QRadioButton(tr("L&ettres minuscules"));
    radio3 = new QRadioButton(tr("Le&ttres majuscules"));
    radio4 = new QRadioButton(tr("&Chiffres romains minuscules"));
    radio5 = new QRadioButton(tr("C&hiffres romains majuscules"));

    //On en coche un par défaut
    radio1->setChecked(true);

    layout_puce->addWidget(radio1);
    layout_puce->addWidget(radio2);
    layout_puce->addWidget(radio3);
    layout_puce->addWidget(radio4);
    layout_puce->addWidget(radio5);
    groupe->setLayout(layout_puce);
    groupe->move(5, 5);//Pour ne pas toucher les bords
    QVBoxLayout *layout_principal = new QVBoxLayout;
    layout_principal->addWidget(groupe);

    QPushButton *valider = new QPushButton;
    valider->setText(tr("Valider"));
    valider->setIcon(QIcon(":/menus/images/ok.png"));
    layout_principal->addWidget(valider, 1, Qt::AlignHCenter);
    connect(valider, SIGNAL(clicked()), puce, SLOT(close()));

    puce->setLayout(layout_principal);
    //On affiche
    puce->exec();
}

const QString fen_puces::get_radio(){

    //On renvoie le radio qui est coché
    if(radio1->isChecked()){
        return "QTextListFormat::ListDecimal";
    }
    else if(radio2->isChecked()){
        return "QTextListFormat::ListLowerAlpha";
    }
    else if(radio3->isChecked()){
        return "QTextListFormat::ListUpperAlpha";
    }
    else if(radio4->isChecked()){
        return "QTextListFormat::ListLowerRoman";
    }
    else if(radio5->isChecked()){
        return "QTextListFormat::ListUpperRoman";
    }
    else{
        ErrorManager instance_erreur;
        instance_erreur.Erreur_msg(tr("Erreur lors de la détection du choix de liste : aucun style n'a été coché"), QMessageBox::Information);
        return "";
    }
}
