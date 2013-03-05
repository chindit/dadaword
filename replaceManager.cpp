#include "replaceManager.h"

ReplaceManager::ReplaceManager(QWidget *parent) :
    QDialog(parent)
{
    manSettings = new SettingsManager;
}

/*ReplaceManager::~ReplaceManager(){
    //delete manSettings;
}*/

void ReplaceManager::showWindow(){
    if(!this->windowTitle().isEmpty()){
        QMessageBox::information(this, "o", "oui");
    }
    else{
        QMessageBox::information(this, "n", "non");
    }

    if(this->windowTitle().isEmpty()){
        setWindowModality(Qt::ApplicationModal);
        setWindowTitle(tr("Autocorrection"));
        setWindowIcon(QIcon(":/programme/images/dadaword.gif"));

        QLabel *title, *key, *value;
        title = new QLabel(tr("<h1>Options d'autocorrection</h1>"));
        key = new QLabel(tr("Remplacer :"));
        value = new QLabel(tr("Par :"));

        layout = new QGridLayout(this);
        layout->addWidget(title, 0, 0, 1, 2, Qt::AlignHCenter);
        layout->addWidget(key, 1, 0);
        layout->addWidget(value, 1, 1);
        QStringList replacementKeys = manSettings->getSettings(Cles).toStringList();
        QStringList replacementValues = manSettings->getSettings(Valeurs).toStringList();
        if(replacementKeys.size() != replacementValues.size()){
            ErrorManager erreurs;
            erreurs.Erreur_msg(tr("Impossible de charger les clés de remplacement, fichier corrompu."), QMessageBox::Warning);
            return;
        }
        else{
            for(int i=0; i<replacementKeys.size(); i++){
                QLineEdit *leKey = new QLineEdit(replacementKeys.at(i));
                QLineEdit *leValue = new QLineEdit(replacementValues.at(i));
                lineKeys.append(leKey);
                lineValues.append(leValue);
                layout->addWidget(lineKeys.at(i), i+2, 0);
                layout->addWidget(lineValues.at(i), i+2, 1);
            }
            addLineButton = new QPushButton(QIcon::fromTheme("edit-add", QIcon(":/programme/images/ajouter.png")), tr("Ajouter une clé"));
            saveAllButton = new QPushButton(QIcon::fromTheme("dialog-ok", QIcon(":/menus/images/ok.png")), tr("Fermer"));
            layout->addWidget(addLineButton, replacementKeys.size()+3, 0);
            layout->addWidget(saveAllButton, replacementValues.size()+3, 1);
            connect(addLineButton, SIGNAL(clicked()), this, SLOT(addLine()));
            connect(saveAllButton, SIGNAL(clicked()), this, SLOT(saveKeys()));
        }
    }

    exec();
}

void ReplaceManager::addLine(){
    QLineEdit *leKey = new QLineEdit;
    QLineEdit *leValue = new QLineEdit;
    lineKeys.append(leKey);
    lineValues.append(leValue);

    //Refresh layout
    for(int i=0; i<lineKeys.size(); i++){
        layout->addWidget(lineKeys.at(i), i+2, 0);
        layout->addWidget(lineValues.at(i), i+2, 1);
    }
    layout->addWidget(addLineButton, lineKeys.size()+3, 0);
    layout->addWidget(saveAllButton, lineValues.size()+3, 1);
    show();
}

void ReplaceManager::saveKeys(){
    if(lineKeys.size() != lineValues.size()){
        ErrorManager instance;
        instance.Erreur_msg(tr("Nombre de clés incorrectes dans la liste d'autocorrections.\nAucune valeur ne sera enregistrée"), QMessageBox::Warning);
        return;
    }
    QStringList replacementKeys;
    QStringList replacementValues;
    for(int i=0; i<lineKeys.size(); i++){
        if(!lineKeys.at(i)->text().isEmpty() && !lineValues.at(i)->text().isEmpty()){
            replacementKeys.append(lineKeys.at(i)->text());
            replacementValues.append(lineValues.at(i)->text());
        }
    }
    manSettings->setSettings(Cles, replacementKeys);
    manSettings->setSettings(Valeurs, replacementValues);
    close();
    deleteLater();
    return;
}
