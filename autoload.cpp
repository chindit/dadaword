#include "autoload.h"

autoLoad::autoLoad() :
    QDialog(0){
}

autoLoad::~autoLoad(){
}

bool autoLoad::hasFilesNames(){
    QDir autosave = QDir::homePath()+"/.dadaword/autosave";
    names = autosave.entryList(QDir::NoDotAndDotDot);

    if(!names.isEmpty()){;
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
    QStringList filesToRestore;
    for(int i=0; i<names.size(); i++){
        if(items[i].checkState() == Qt::Checked){
            filesToRestore.append(QDir::homePath()+"/.dadaword/autosave/"+names.at(i));
        }
        else{
            //L'utilisateur ne veut pas garder -> on vire
            QFile fichier(QDir::homePath()+"/.dadaword/autosave/"+names.at(i));
            fichier.remove();
        }
    }
    return filesToRestore;
}
