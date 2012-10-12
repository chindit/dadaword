#include "settingsManager.h"

SettingsManager::SettingsManager(QObject *parent) :
    QObject(parent)
{
    names = new QString[16];
    settings = new QVariant[16];

    names[Onglets] = "onglets";
    names[FichiersVides] = "fichiersVides";
    names[Dico] = "dico";
    names[Police] = "police";
    names[Taille] = "taille";
    names[Alertes] = "alertes";
    names[Orthographe] = "orthographe";
    names[Word] = "word";
    names[Timer] = "timer";
    names[Enregistrement] = "enregistrement";
    names[Theme] = "themes";
    names[Cles] = "ClesRemplacements";
    names[Valeurs] = "ValeursRemplacement";
    names[Autocorrection] = "Autocorrection";
    names[ToolbarIcons] = "showIconsToolbar";
    names[FichiersRecents] = "fichiersRecents";

    loadSettings();

}

QVariant SettingsManager::getSettings(Setting s){
    return settings[s];
}

void SettingsManager::setSettings(Setting s, QVariant v){
    if(settings[s] != v){
        settings[s] = v;
        //Enregistrement
        QSettings options("DadaWord", "dadaword");
        options.setValue(names[s], settings[s]);
    }
    else{
        return;
    }
}

void SettingsManager::loadSettings(){
    QSettings options("DadaWord", "dadaword");
    settings[Onglets] = options.value(names[Onglets], false);
    settings[FichiersVides] = options.value(names[FichiersVides], false);
    settings[Dico] = options.value(names[Dico], "fr_BE");
    settings[Police] = options.value(names[Police], QFont("Ubuntu"));
    settings[Taille] = options.value(names[Taille], 12);
    settings[Alertes] = options.value(names[Alertes], HIGH);
    settings[Orthographe] = options.value(names[Orthographe], true);
    settings[Word] = options.value(names[Word], true);
    settings[Timer] = options.value(names[Timer], 300);
    settings[Enregistrement] = options.value(names[Enregistrement], QDir::homePath());
    settings[Theme] = options.value(names[Theme], "DadaWord");
    settings[Cles] = options.value(names[Cles]);
    settings[Valeurs] = options.value(names[Valeurs]);
    settings[Autocorrection] = options.value(names[Autocorrection], true);
    settings[ToolbarIcons] = options.value(names[ToolbarIcons], false);
    settings[FichiersRecents] = options.value(names[FichiersRecents]);
    return;
}

