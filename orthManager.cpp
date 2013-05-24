#include "orthManager.h"
#include "ui_orthManager.h"

OrthManager::OrthManager(QWidget *parent, QTextEdit *contenu) :
    QDialog(parent),
    ui(new Ui::OrthManager)
{
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
    // create a new cursor to walk through the text
    QTextCursor cursor(ui->contenu_texte->document());
    connect(ui->bouton_ignore, SIGNAL(clicked()), this, SLOT(checkWord()));

    checkWord();
        //Masquage de la barre d'outils
        //barre_orthographe->hide();

        //Et on efface le surlignage
        //QTextCharFormat highlightFormat;
        //highlightFormat.setBackground(QBrush(QColor("#ffffff")));
        //QTextEdit::ExtraSelection ess;
        //QList<QTextEdit::ExtraSelection> essList;
        //essList << ess;
        //ui->contenu_texte->setExtraSelections(essList);
        //QCoreApplication::processEvents();
        //ess.format = highlightFormat;

        //On efface pos_orth
        //pos_orth.movePosition(QTextCursor::Start);
}

OrthManager::~OrthManager()
{
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

QStringList list_skip;
    //Booléen d'erreur qui stope la boucle
    bool erreur = false;

    while(!cursor.atEnd() && !erreur) {
        QCoreApplication::processEvents();
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
        QString word = cursor.selectedText();

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

            //On met à jour le mot dans la toolbar
            //orth_mot->setText("<b>"+word+"</b>");
            //orth_mot->setAlignment(Qt::AlignHCenter);
            //On nettoye la QComboBox
            //orth_suggest->clear();

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
        }
}
