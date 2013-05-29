#include "puces.h"
#include "ui_puces.h"

Puces::Puces(QWidget *parent) : QDialog(parent), ui(new Ui::Puces){
    ui->setupUi(this);
    globalState = true;
    connect(ui->bouton_exit, SIGNAL(clicked()), this, SLOT(close2()));
    connect(ui->bouton_valide, SIGNAL(clicked()), this, SLOT(close()));
}

void Puces::close2(){
    globalState = false;
    this->close();
}

Puces::~Puces()
{
    delete ui;
}

bool Puces::isClosed(){
    return !globalState;
}

QString Puces::getRadio(){
    //On renvoie le radio qui est coché
        if(ui->radioButton->isChecked()){
            return "QTextListFormat::ListDecimal";
        }
        else if(ui->radioButton_2->isChecked()){
            return "QTextListFormat::ListLowerAlpha";
        }
        else if(ui->radioButton_3->isChecked()){
            return "QTextListFormat::ListUpperAlpha";
        }
        else if(ui->radioButton_4->isChecked()){
            return "QTextListFormat::ListLowerRoman";
        }
        else if(ui->radioButton_5->isChecked()){
            return "QTextListFormat::ListUpperRoman";
        }
        else{
            ErrorManager instance_erreur;
            instance_erreur.Erreur_msg(tr("Erreur lors de la détection du choix de liste : aucun style n'a été coché"), QMessageBox::Information);
            return "";
        }
}
