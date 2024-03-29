/*
Développeur : David Lumaye (littletiger58.aro-base.gmail.com)
Date : 17/05/13
Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
Merci de ne pas supprimer cette notice.
*/

#include "autoload.h"

autoLoad::autoLoad() :
    QDialog(0){
}

autoLoad::~autoLoad(){
}

bool autoLoad::hasFilesNames(){
    QStringList dossiers = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    QString dossier = dossiers.first();
    dossier.append("/autosave");
    QDir autosave(dossier);
    names = autosave.entryList();

    names.removeAt(0);
    names.removeAt(0);

    if(!names.isEmpty()){
            showFiles();
    }
    return (names.isEmpty()) ? false : true;
}

void autoLoad::showFiles(){
    items = new QListWidgetItem[names.size()];

    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("DadaWord : Récupération de fichiers"));
    setWindowIcon(QIcon(":/programme/images/dadaword.png"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *title = new QLabel(tr("<h1>Récupération de documents</h1>"));
    QLabel *explanation = new QLabel(tr("DadaWord s'est mal fermé lors de la dernière exécution.  Fort heureusement, les fichiers suivants ont pu être récupérés.\nSi vous souhaitez récupérer le document, cochez la case et validez.\nVous pourrez ensuite enregistrer le document récupéré sous le nom que vous souhaitez."));
    QListWidget *filesList = new QListWidget;
    QPushButton *go = new QPushButton(QIcon::fromTheme("dialog-close"), tr("Continuer"));
    go->setMaximumWidth(100);
    connect(go, SIGNAL(clicked()), this, SLOT(close()));
    for(int i=0; i<names.size(); i++){
        items[i].setText((names.at(i).contains("DDWubIntMs", Qt::CaseSensitive)) ? "Document non-enregistré" : names.at(i));
        items[i].setCheckState(Qt::Unchecked);
        filesList->addItem(&items[i]);
    }

    layout->addWidget(title, 0, Qt::AlignHCenter);
    layout->addWidget(explanation);
    layout->addWidget(filesList);
    layout->addWidget(go, 0, Qt::AlignRight);

    exec();
}

QStringList autoLoad::getFilesNames(){
    QStringList dossiers = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    QString dossier = dossiers.first();
    dossier.append("/autosave/");
    QStringList filesToRestore;
    for(int i=0; i<names.size(); i++){
        if(items[i].checkState() == Qt::Checked){
            filesToRestore.append(dossier+names.at(i));
        }
        else{
            //L'utilisateur ne veut pas garder -> on vire
            QFile fichier(dossier+names.at(i));
            fichier.remove();
        }
    }
    return filesToRestore;
}
