#include "autoload.h"

autoLoad::autoLoad(QWidget *parent) :
    QDialog(parent){
}

autoLoad::~autoLoad(){
    //On supprime les fichiers
    for(int i=0; i<names.size(); i++){
        QFile fichier(QDir::homePath()+"/.dadaword/autosave/"+names.at(i));
        QString test = QDir::homePath()+"/.dadaword/autosave/"+names.at(i);
        fichier.remove();
    }
}

bool autoLoad::hasFilesNames(){
    QDir autosave = QDir::homePath()+"/.dadaword/autosave";qDebug("1");
    names = autosave.entryList();qDebug("2");
    if(names.size() >= 2){
        names.removeFirst();qDebug("3");
        names.removeFirst();qDebug("4");
    }
    else{
        return false;
    }
    if(!names.isEmpty()){qDebug("5");
        showFiles();
    }
    return (names.isEmpty()) ? false : true;
}

void autoLoad::showFiles(){
    items = new QListWidgetItem[names.size()];

    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("DadaWord : Récupération de fichiers"));
    setWindowIcon(QIcon(":/programme/images/dadaword.gif"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *title = new QLabel(tr("<h1>Récupération de documents</h1>"));
    QLabel *explanation = new QLabel(tr("DadaWord s'est mal fermé lors de la dernière exécution.  Fort heureusement, les fichiers suivants ont pu être récupérés.\nSi vous souhaitez récupérer le document, cochez la case et validez.\nVous pourrez ensuite enregistrer le document récupéré sous le nom que vous souhaitez."));
    QListWidget *filesList = new QListWidget;
    QPushButton *go = new QPushButton(QIcon(":/menus/images/ok.png"), tr("Continuer"));
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
    QStringList filesToRestore;
    for(int i=0; i<names.size(); i++){
        if(items[i].checkState() == Qt::Checked){
            filesToRestore.append(QDir::homePath()+"/.dadaword/autosave/"+names.at(i));
        }
    }
    return filesToRestore;
}