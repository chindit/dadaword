#include "orthManager.h"
#include "ui_orthManager.h"

OrthManager::OrthManager(QWidget *parent, QTextEdit *contenu) : QDialog(parent), ui(new Ui::OrthManager){
    ui->setupUi(this);
    ui->contenu_texte->setCursor(contenu->cursor());
    ui->contenu_texte->setDocument(contenu->document());

    QString userDict = QDir::homePath() + "/.dadaword/perso.dic";
    QString dictPath = "/usr/share/hunspell/fr_BE";
    //Vérification des liens symboliques
    QFileInfo testDico(dictPath);
    if(testDico.isSymLink())
        dictPath = testDico.symLinkTarget();

    correcteur = new SpellChecker(dictPath, userDict);

    //Connexion des boutons aux slots
    //Groupe 1 : boutons du haut
    connect(ui->bouton_ignore, SIGNAL(clicked()), this, SLOT(checkWord()));
    connect(ui->bouton_ignore_tout, SIGNAL(clicked()), this, SLOT(ignore()));
    connect(ui->bouton_ajout_dico, SIGNAL(clicked()), this, SLOT(addDico()));
    connect(ui->bouton_ignore_def, SIGNAL(clicked()), this, SLOT(ignoreDef()));
    //Goupe 2 : boutons du bas
    connect(ui->bouton_remplacer, SIGNAL(clicked()), this, SLOT(remplacer()));

    checkWord();
}

OrthManager::~OrthManager(){
    delete ui;
}

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
        if(!word.isEmpty() && !correcteur->spell(word) && !list_skip.contains(word)){
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
            if(suggestions.size() > 1)
                ui->label_2->setText(tr("Suggestions :"));
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
        //if(settings->getSettings(Alertes).toInt() == HIGH){
            QMessageBox::information(this, tr("Terminé"), tr("La vérification orthographique est terminée."));
            this->close();
        }
}

//Ignore un mot
void OrthManager::ignore(){
    list_skip.append(word);
    checkWord();
    return;
}

//Ajoute un mot au dictionnaire
void OrthManager::addDico(){
    if(!word.isEmpty() && !word.isNull()){
        correcteur->addToUserWordlist(word);
        checkWord();
    }
    return;
}

//Ignore définitivement un mot pour ce document
void OrthManager::ignoreDef(){
    QMessageBox::information(this, "Fonctionnalité non-implémentée", "Malheureusement, cette fonctionnalité n'est pas implémentée pour le moment, mais nous y travaillons.");
    return;
}

//Remplace un mot
void OrthManager::remplacer(){
    QTextCursor temp;
    QModelIndexList selected = ui->liste_corrections->selectionModel()->selectedIndexes();
    if(selected.count() > 0){
        QString mot  = selected.at(0).data().toString();
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
        checkWord();
    }
    return;
}
