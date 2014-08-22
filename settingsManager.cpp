#include "settingsManager.h"

SettingsManager::SettingsManager(QObject *parent) :
    QObject(parent)
{
    names = new QString[25];
    settings = new QVariant[25];

    names[Onglets] = "onglets";
    names[FichiersVides] = "fichiersVides";
    names[Dico] = "dico";
    names[Police] = "police";
    names[Taille] = "taille";
    names[Alertes] = "alertes";
    names[Orthographe] = "orthographe";
    names[OrthographeTexte] = "orthographe_texte";
    names[Word] = "word";
    names[Timer] = "timer";
    names[Enregistrement] = "enregistrement";
    names[Theme] = "themes";
    names[Cles] = "ClesRemplacements";
    names[Valeurs] = "ValeursRemplacement";
    names[Autocorrection] = "Autocorrection";
    names[ToolbarIcons] = "showIconsToolbar";
    names[FichiersRecents] = "fichiersRecents";
    names[DelTitre] = "SupprimerTitre";
    names[UseDir] = "UtiliserDernierDossier";
    names[RGras] = "raccourci_gras";
    names[RItalique] = "raccourci_italique";
    names[RSouligne] = "raccourci_souligne";
    names[RNouveau] = "raccourci_nouveau";
    names[ROuvrir] = "raccourci_ouvrir";
    names[REnregistrer] = "raccourci_enregistrer";

    loadSettings();

}

QVariant SettingsManager::getSettings(Setting s){
    return settings[s];
}

void SettingsManager::setSettings(Setting s, QVariant v){
    if(settings[s] != v){
        settings[s] = v;
        //Enregistrement
        QSettings options("DadaDesktop", "dadaword");
        options.setValue(names[s], settings[s]);
    }
    else{
        return;
    }
}

void SettingsManager::loadSettings(){
    QSettings options("DadaDesktop", "dadaword");
    settings[Onglets] = options.value(names[Onglets], false);
    settings[FichiersVides] = options.value(names[FichiersVides], false);
    settings[Dico] = options.value(names[Dico], "fr_BE");
    settings[Police] = options.value(names[Police], QFont("Ubuntu"));
    settings[Taille] = options.value(names[Taille], 12);
    settings[Alertes] = options.value(names[Alertes], HIGH);
    settings[Orthographe] = options.value(names[Orthographe], true);
    settings[OrthographeTexte] = options.value(names[OrthographeTexte], false);
    settings[Word] = options.value(names[Word], true);
    settings[Timer] = options.value(names[Timer], 300);
    settings[Enregistrement] = options.value(names[Enregistrement], QDir::homePath());
    settings[Theme] = options.value(names[Theme], "DadaWord");
    settings[Cles] = options.value(names[Cles]);
    settings[Valeurs] = options.value(names[Valeurs]);
    settings[Autocorrection] = options.value(names[Autocorrection], true);
    settings[ToolbarIcons] = options.value(names[ToolbarIcons], false);
    settings[FichiersRecents] = options.value(names[FichiersRecents]);
    settings[DelTitre] = options.value(names[DelTitre], true);
    settings[UseDir] = options.value(names[UseDir], true);
    settings[RGras] = options.value(names[RGras], "Ctrl+B");
    settings[RItalique] = options.value(names[RItalique], "Ctrl+I");
    settings[RSouligne] = options.value(names[RSouligne], "Ctrl+U");
    settings[ROuvrir] = options.value(names[ROuvrir], "Ctrl+O");
    settings[RNouveau] = options.value(names[RNouveau], "Ctrl+N");
    settings[REnregistrer] = options.value(names[REnregistrer], "Ctrl+S");
    return;
}

//Supprime toutes les préférences
void SettingsManager::resetSettings(){
    int resultat = QMessageBox::question(0, tr("Suppression des préférences"), tr("Attention : cette action supprimera <strong>toutes</strong> vos préférences.<br />Etes-vous sur de vouloir continuer?"), QMessageBox::Yes|QMessageBox::No);
    if(resultat == QMessageBox::Yes){
        QSettings options("DadaDesktop", "dadaword");
        options.clear();
        if(this->getSettings(Alertes).toInt() == HIGH)
            QMessageBox::information(0, tr("Suppression effectuée"), tr("Vos préférences ont été supprimées avec succès.  La suppression sera pleinement effective après redémarrage de DadaWord"));
    }
    return;
}

