#include "orthManager.h"
#include "ui_orthManager.h"

OrthManager::OrthManager(QString dictionnaire, QWidget *parent) : QDialog(parent), ui(new Ui::OrthManager){
    QString dictPath;
    dicoActuel = dictionnaire;
    settings = new SettingsManager;

    QStringList dossiers = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    QString repertoire = dossiers.first();
    #ifdef Q_OS_WIN
        ErrorManager erreur;
        dictPath = repertoire+"/hunspell/"+dictionnaire;
        QFile test_dico(dictPath+".dic");
        if(!test_dico.exists()){
            //On téléchage les dicos
            int qdown = QMessageBox::question(this, tr("Téléchargement des dictionnaires"), tr("Les dictionnaires pour la correction orthographique n'ont pas étés détectés.<br />Voulez-vous les télécharger?<br /><b>N.B.</b>L'absence des dictionnaires rendra la correction orthographique inopérante"), QMessageBox::Yes | QMessageBox::No);
            if(qdown == QMessageBox::Yes){
                QDir temp(repertoire+"/hunspell/");
                if(!temp.exists() && !temp.mkdir(repertoire+"/hunspell/")){
                    erreur.Erreur_msg(tr("Impossible de créer le repertoire de dictionnaires"), QMessageBox::Warning);
                }
                else{
                    QNetworkAccessManager nw_manager;
                    QNetworkRequest request(QUrl("https://raw.github.com/chindit/dadaword/master/liste_dicos.txt"));
                    QNetworkReply *reponse = nw_manager.get(request);
                    QEventLoop eventLoop;
                    QObject::connect(reponse, SIGNAL(finished()), &eventLoop, SLOT(quit()));
                    eventLoop.exec();
                    QNetworkReply *reply_dico;
                    QDialog *progression = new QDialog(this);
                    progression->setWindowTitle(tr("Téléchargement des dictionnaires"));
                    QLabel *titleProgress, *actDownDic;
                    titleProgress = new QLabel(tr("Progression du téléchagrement des dictionnaires"));
                    actDownDic = new QLabel;
                    QProgressBar *progressDownDic = new QProgressBar;
                    QVBoxLayout *layoutProgress = new QVBoxLayout(progression);
                    layoutProgress->addWidget(titleProgress);
                    layoutProgress->addWidget(progressDownDic);
                    layoutProgress->addWidget(actDownDic);
                    setWindowIcon(QIcon(":/programme/images/dadaword.gif"));
                    int posDown = 1;
                    float maxDico = reponse->readLine().trimmed().toFloat();
                    progressDownDic->setRange(0, 100);
                    progressDownDic->setValue(0);
                    progression->show();
                    while(!reponse->atEnd()){
                        QString baseUrl = "https://raw.github.com/chindit/dadaword/master/dicos/";
                        QString nom_dico = reponse->readLine();
                        baseUrl = baseUrl.append(nom_dico.trimmed());
                        QNetworkAccessManager get_file;
                        QNetworkRequest get_dico(QUrl("https://raw.github.com/chindit/dadaword/master/dicos/"+nom_dico.trimmed()));
                        reply_dico = get_file.get(get_dico);
                        QEventLoop wait_dico;
                        QObject::connect(reply_dico, SIGNAL(finished()), &wait_dico, SLOT(quit()));
                        wait_dico.exec();
                        QFile save_dico(repertoire+"/hunspell/"+nom_dico.trimmed());
                        if(!save_dico.open(QIODevice::WriteOnly)){
                            QMessageBox::critical(0, "this", "Échec");
                        }
                        save_dico.write(reply_dico->readAll());
                        save_dico.close();
                        progressDownDic->setValue((posDown/maxDico)*100);
                        posDown++;
                        //Màj du nom
                        actDownDic->setText(nom_dico.trimmed());
                        progression->show();
                    }
                    progression->close();
                    delete progression;
                }
            }
        }
    #else
        dictPath = "/usr/share/hunspell/"+dictionnaire;
    #endif

    //Vérification des liens symboliques
    QFileInfo testDico(dictPath);
    if(testDico.isSymLink())
        dictPath = testDico.symLinkTarget();

    //Création de l'instance du correcteur
    correcteur = new SpellChecker(dictPath, this->setUserDict());
}

OrthManager::~OrthManager(){
    delete ui;
}

void OrthManager::showWindow(QTextEdit *contenu){
    ui->setupUi(this);
    if(settings->getSettings(Word).toBool()){
        QTextFrame *tf = contenu->document()->rootFrame();
        QTextFrameFormat tff = tf->frameFormat();
        tff.setMargin(0);
        tf->setFrameFormat(tff);
    }
    ui->contenu_texte->setCursor(contenu->cursor());
    ui->contenu_texte->setDocument(contenu->document());

    //Bouton de langue
    ui->bouton_langue->setFlat(true);
    ui->bouton_langue->setText(this->getDico().split("/").last().split(".").first());
    connect(ui->bouton_langue, SIGNAL(clicked()), this, SLOT(setDico()));

    //Connexion des boutons aux slots
    //Groupe 1 : boutons du haut
    connect(ui->bouton_ignore, SIGNAL(clicked()), this, SLOT(checkWord()));
    connect(ui->bouton_ignore_tout, SIGNAL(clicked()), this, SLOT(ignore()));
    connect(ui->bouton_ajout_dico, SIGNAL(clicked()), this, SLOT(addDico()));
    connect(ui->bouton_ignore_def, SIGNAL(clicked()), this, SLOT(ignoreDef()));
    //Goupe 2 : boutons du bas
    connect(ui->bouton_remplacer, SIGNAL(clicked()), this, SLOT(remplacer()));
    connect(ui->liste_corrections, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(remplacer()));
    connect(ui->bouton_remplacer_tout, SIGNAL(clicked()), this, SLOT(remplacerTout()));
    connect(ui->bouton_autocorrection, SIGNAL(clicked()), this, SLOT(autocorrection()));
    connect(ui->bouton_options, SIGNAL(clicked()), this, SLOT(options()));

    checkWord();
    return;
}

//Vérifie l'orthographe
void OrthManager::checkWord(){
    QTextCursor cursor(ui->contenu_texte->document());

    //Sélection de texte (pour le surlignage)
    QTextEdit::ExtraSelection es;

    //On regarde si on est déjà en train de vérifier:
    if(!pos_orth.isNull() && !pos_orth.atStart()){
        pos_orth.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
        cursor = pos_orth;
    }
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QBrush(QColor("#ff6060")));
    highlightFormat.setForeground(QBrush(QColor("#000000")));

    //Booléen d'erreur qui stope la boucle
    bool erreur = false;

    while(!cursor.atEnd() && !erreur) {
        QCoreApplication::processEvents();
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
        word = cursor.selectedText();

        // Workaround for better recognition of words
        // punctuation etc. does not belong to words
        while(!word.isEmpty() && !word.at(0).isLetter() && cursor.anchor() < cursor.position()) {
            int cursorPos = cursor.position();
            cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
            cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
            word = cursor.selectedText();
        }

        //Nettoyage des éléments qui suivent
        while(!word.isEmpty() && !word.at(word.size()-1).isLetter()){
            word = word.remove(word.size()-1, word.size());
        }

        //Il n'y a erreur QUE si le mot n'est pas vide, n'est pas présent dans le dictionnaire ET n'est pas ignoré
        if(!word.isEmpty() && !correcteur->spell(word) && !this->getListSkip().contains(word)){
            //Affichage de la barre d'outils,
            //barre_orthographe->show();

            QTextCursor tmpCursor(cursor);
            tmpCursor.setPosition(cursor.anchor());

            // highlight the unknown word
            es.cursor = cursor;
            es.format = highlightFormat;

            QList<QTextEdit::ExtraSelection> esList;
            esList << es;
            ui->contenu_texte->setExtraSelections(esList);
            QCoreApplication::processEvents();

            //On met à jour la liste de suggestions
            QStringList suggestions = correcteur->suggest(word);
            //orth_suggest->setEnabled(true);
            QStandardItemModel* liste = new QStandardItemModel();
            for(int i=0; i<suggestions.size(); i++){
                QStandardItem* item = new QStandardItem(suggestions.at(i));
                    liste->appendRow(item);
            }
            if(suggestions.size() > 1){
                ui->label_2->setText(tr("Suggestions :"));
            }
            ui->liste_corrections->setModel(liste);
            if(suggestions.size() == 0){
                //Vu qu'il n'y a rien, on l'indique
                QStandardItem* item = new QStandardItem(tr("Aucune suggestion"));
                liste->appendRow(item);
                ui->liste_corrections->setEnabled(false);
            }

            //On coupe la boucle
            erreur = true;
            //Transfent de variable en cas de besoin pour le dico
            //orth_erreur = word;
        }
        //On met à jour le curseur
        pos_orth = cursor;
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
    }
    //Dans tous les cas, on laisse le curseur visible
    ui->contenu_texte->setTextCursor(cursor);
    ui->contenu_texte->ensureCursorVisible();

    if(!erreur){//Si "erreur" est faux, c'est qu'on a atteint la fin du document
            QMessageBox::information(this, tr("Terminé"), tr("La vérification orthographique est terminée."));
            this->hide();
            this->close();
        }
}

//Ignore un mot
void OrthManager::ignore(QString mot){
    if(!mot.isEmpty()){
        list_skip.append(mot);
    }
    else{
        list_skip.append(word);
        checkWord();
    }
    return;
}

//Ajoute un mot au dictionnaire
void OrthManager::addDico(QString mot){
    if(!mot.isEmpty()){
        correcteur->addToUserWordlist(mot);
    }
    else{
        if(!word.isEmpty() && !word.isNull()){
            correcteur->addToUserWordlist(word);
            checkWord();
        }
    }
    return;
}

//Ignore définitivement un mot pour ce document
void OrthManager::ignoreDef(QString mot){
    if(mot.isEmpty()){
        list_skip_definitively.append(word);
        checkWord();
    }
    else{
        list_skip_definitively.append(mot);
    }
    return;
}

//Remplace un mot
void OrthManager::remplacer(QString nmot){
    QTextCursor temp;
    QString mot;
    QModelIndexList selected;
    if(!nmot.isEmpty()){
        mot = nmot;
    }
    else{
        selected = ui->liste_corrections->selectionModel()->selectedIndexes();
    }
    if(selected.count() > 0 || !mot.isEmpty()){
        if(mot.isEmpty()){
            mot  = selected.at(0).data().toString();
        }
        temp = pos_orth;
        QString word = temp.selectedText();
        while(!word.isEmpty() && !word.at(0).isLetter() && temp.anchor() < temp.position()) {
            int cursorPos = temp.position();
            temp.setPosition(temp.anchor() + 1, QTextCursor::MoveAnchor);
            temp.setPosition(cursorPos, QTextCursor::KeepAnchor);
            word = temp.selectedText();
        }
        temp.removeSelectedText();
        temp.insertText(mot);
        temp.movePosition(QTextCursor::NextWord);
        if(nmot.isEmpty()){
            checkWord();
        }
    }
    return;
}

//Remplace toutes les occurences d'un mot
void OrthManager::remplacerTout(){
    int nb_remplacements = 0;
    QModelIndexList selected = ui->liste_corrections->selectionModel()->selectedIndexes();
    if(selected.count() == 0){
        return;
    }
    // save the position of the current cursor
    QTextCursor oldCursor = ui->contenu_texte->textCursor();

    // create a new cursor to walk through the text
    QTextCursor cursor(ui->contenu_texte->document());

    if(cursor.atEnd()){
        cursor.movePosition(QTextCursor::Start);
    }

    //Parcours de tout le document
    while(!cursor.atEnd()) {
        QCoreApplication::processEvents();
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
        QString Sword = cursor.selectedText();

        //Nettoyage des éléments qui précèdent
        while(!Sword.isEmpty() && !Sword.at(0).isLetter() && cursor.anchor() < cursor.position()) {
            int cursorPos = cursor.position();
            cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
            cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
            Sword = cursor.selectedText();
        }

        //Nettoyage des éléments qui suivent
        while(!Sword.isEmpty() && !Sword.at(Sword.size()-1).isLetter()){
            Sword = Sword.remove(Sword.size()-1, Sword.size());
        }

        //Remplacement du mot
        if(Sword == word){
            cursor.removeSelectedText();
            cursor.insertText(selected.at(0).data().toString());
            nb_remplacements++; //On incrémente le nombre de remplacements
        }

        //On met à jour le curseur
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
    }

    //On remet en place le vieux curseur
    ui->contenu_texte->setTextCursor(oldCursor);

    //On affiche le nombre de remplacements
    QMessageBox::information(this, "Remplacement terminé", QString("Le mot «%1» a été remplacé %2 fois").arg(word).arg(nb_remplacements));
    return;
}

//Retourne la liste des mots à ignorer
QStringList OrthManager::getListSkip(bool definitive){
    if(definitive)
        return list_skip_definitively;
    QStringList temp;
    temp = list_skip;
    for(int i=0; i<list_skip_definitively.count(); i++){
        temp.append(list_skip_definitively.at(i));
    }
    return temp;
}

//Vérifie si le mot envoyé est correctement orthographié
bool OrthManager::isCorrectWord(QString word){
    return correcteur->spell(word);
}

//Retourne la liste des suggestions pour le mot erroné
QStringList OrthManager::getSuggestList(QString word){
    QStringList suggestions;
    if(!correcteur->spell(word)){
        suggestions = correcteur->suggest(word);
    }
    return suggestions;
}

//Transfère le curseur d'une classe à l'autre
void OrthManager::setTextCursor(QTextCursor cursor){
    pos_orth = cursor;
}

//Ajoute le mot à corriger à l'autocorrection
void OrthManager::autocorrection(QString remplacement){
    SettingsManager settings;
    ErrorManager erreur;
    QTextCursor cursor;
    cursor = pos_orth;
    bool make_remplace = false;
    if(remplacement.isEmpty()){
        QModelIndexList selected = ui->liste_corrections->selectionModel()->selectedIndexes();
        if(selected.count() == 0){
            return;
        }
        remplacement = selected.at(0).data().toString();
        make_remplace = true;
    }
    cursor.select(QTextCursor::WordUnderCursor);
    QString mot  = cursor.selectedText();
    if(!mot.isEmpty() && !remplacement.isEmpty()){
        QStringList listeCles = settings.getSettings(Cles).toStringList();
        QStringList listeValeurs = settings.getSettings(Valeurs).toStringList();
        if(listeCles.size() != listeValeurs.size()){
            erreur.Erreur_msg(tr("Paires de remplacements invalide, impossible d'ajouter l'autocorrection"), QMessageBox::Warning);
            return;
        }
        if(listeCles.contains(mot, Qt::CaseInsensitive)){
            erreur.Erreur_msg(tr("Un clé nommée %1 existe déja.  L'autocorrection ne sera donc pas mise en place").arg(mot), QMessageBox::Information);
            return;
        }
        listeCles.append(mot);
        listeValeurs.append(remplacement);
        settings.setSettings(Cles, listeCles);
        settings.setSettings(Valeurs, listeValeurs);
        if(make_remplace){
            remplacer();
        }
    }
    else{
        erreur.Erreur_msg(tr("Le mot à remplacer est vide, l'autocorrection ne sera donc pas mise en place."), QMessageBox::Warning);
        return;
    }
    return;
}

//Change le dictionnaire actif
void OrthManager::setDico(QString langue){

    QComboBox *liste = new QComboBox;
    if(langue.isEmpty()){
        QDialog *fen = new QDialog;
        fen->setWindowTitle(tr("Langue du correcteur"));
        fen->setWindowModality(Qt::ApplicationModal);
        QLabel *titre, *actuelle, *choix;
        QPushButton *valider = new QPushButton(tr("Valider"));
        titre = new QLabel("<h1>Langue du correcteur<h1>");
        actuelle = new QLabel(tr("Dictionnaire actuel : ")+this->getDico().split("/").last().split(".").first());
        choix = new QLabel(tr("Nouvelle langue"));

        QStringList liste_dicos = this->getDicos();
        for(int i=0; i<liste_dicos.size(); i++){
            QString temp = liste_dicos.at(i);
            temp.resize((temp.size()-4));
            liste->addItem(temp);
            //On présélectionne la langue actuelle
            if(temp == this->getDico().split("/").last().split(".").first()){
                liste->setCurrentIndex(i);
            }
        }
        valider->setIcon(QIcon::fromTheme("dialog-ok", QIcon(":/menus/images/ok.png")));
        QGridLayout *layout = new QGridLayout;
        layout->addWidget(titre, 0, 0, 1, 2, Qt::AlignHCenter);
        layout->addWidget(actuelle, 1, 0);
        layout->addWidget(choix, 2, 0);
        layout->addWidget(liste, 2, 1);
        layout->addWidget(valider, 3, 0, 1, 2, Qt::AlignHCenter);
        connect(valider, SIGNAL(clicked()), fen, SLOT(close()));
        fen->setLayout(layout);
        fen->exec();
        QCoreApplication::processEvents();
    }

    QString nouvelleLangue = (langue.isEmpty()) ? liste->currentText() : langue;
    QDir dossier;
#ifdef Q_OS_WIN
    QStringList dossiers = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    QString repertoire = dossiers.first();
    dossier.setPath(repertoire+"/hunspell/");
#else
    dossier.setPath("/usr/share/hunspell/");
#endif
    //On met à jour le dico
    if(dicoActuel != nouvelleLangue){
        dicoActuel = dossier.path()+"/"+nouvelleLangue+".dic";
        //Vérification des liens symboliques
        QFileInfo testDico(dicoActuel);
        if(testDico.isSymLink())
            dicoActuel = testDico.symLinkTarget();

        //On re-déclare le correcteur
        delete correcteur;

        correcteur = new SpellChecker(dicoActuel, this->setUserDict());
        emit langueChangee();
    }
    return;
}

//Renvoie le dictionnaire actuellement utilisé (chemin absolu)
QString OrthManager::getDico(){
    return dicoActuel;
}

//Établi le chemin vers le dictionnaire personnel selon l'OS
QString OrthManager::setUserDict(){
    QString userDict, fallbackDict;
#ifdef Q_OS_WIN
    userDict = QDir::homePath() + "/AppData/Roaming/LibreOffice/4/user/wordbook/standard.dic";
    fallbackDict = QDir::homePath() + "/AppData/Local/DadaWord/standard.dic";
#else
    userDict = QDir::homePath() + "/.config/libreoffice/4/user/wordbook/standard.dic";
    fallbackDict = QDir::homePath() + "/.dadaword/perso.dic";
#endif
    if(!QFile::exists(userDict)){
        userDict = fallbackDict;
    }
    return userDict;
}


//Affiche les options orthographiques
void OrthManager::options(){
    QMessageBox::information(this, tr("Fonctionnalité non-implémentée"), tr("Malheureusement, cette fonctionnalité n'est pas encore disponible, mais ce n'est qu'une question de temps ;-)"));
}

//Souligne tous les mots mal orthographiés (lancement à l'initialisation)
void OrthManager::checkAll(QTextEdit *contenu){
    QTextCharFormat erreurs;
    QColor couleur(Qt::red);
    erreurs.setUnderlineColor(couleur);
    erreurs.setUnderlineStyle(QTextCharFormat::WaveUnderline);

    QTextCursor cursor = contenu->textCursor();
    cursor.movePosition(QTextCursor::Start);
    QList<QTextEdit::ExtraSelection> esList;

    while(!cursor.atEnd()) {
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
        word = cursor.selectedText();

        // Workaround for better recognition of words
        // punctuation etc. does not belong to words
        while(!word.isEmpty() && !word.at(0).isLetter() && cursor.anchor() < cursor.position()) {
            int cursorPos = cursor.position();
            cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
            cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
            word = cursor.selectedText();
        }

        //Nettoyage des éléments qui suivent
        while(!word.isEmpty() && !word.at(word.size()-1).isLetter()){
            word = word.remove(word.size()-1, word.size());
        }

        //Il n'y a erreur QUE si le mot n'est pas vide, n'est pas présent dans le dictionnaire ET n'est pas ignoré
        if(!word.isEmpty() && !this->isCorrectWord(word) && !this->getListSkip().contains(word, Qt::CaseInsensitive)){

            QTextCursor tmpCursor(cursor);
            tmpCursor.setPosition(cursor.anchor());

            // highlight the unknown word
            QTextEdit::ExtraSelection es;
            es.cursor = cursor;
            es.format = erreurs;
            esList << es;
        }
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
    }
    contenu->setExtraSelections(esList);
    return;
}

//Transfère les mots ignorés au correcteur
void OrthManager::setMotsIgnores(QStringList liste){
    list_skip_definitively = liste;
}

//Retourne la liste de tous les dictionnaires disponibles
QStringList OrthManager::getDicos(){
    QDir dossier;
#ifdef Q_OS_WIN
    QStringList dossiers = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    QString repertoire = dossiers.first();
    dossier.setPath(repertoire+"/hunspell/");
#else
    dossier.setPath("/usr/share/hunspell");
#endif
    QStringList extentions;
    extentions << "*.dic";
    QStringList liste_dicos = dossier.entryList(extentions);

    return liste_dicos;
}
