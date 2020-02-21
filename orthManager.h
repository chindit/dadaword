#ifndef ORTHMANAGER_H
#define ORTHMANAGER_H

#include <QDialog>
#include <QModelIndexList>
#include <QStandardItem>

#include "dadaword.h"
#include "spellchecker.h"

namespace Ui {
class OrthManager;
}

class OrthManager : public QDialog
{
    Q_OBJECT
    
public:
    explicit OrthManager(QString dictionnaire, QWidget *parent = 0);
    ~OrthManager();
    void showWindow(QTextEdit *contenu);
    QStringList getListSkip(bool definitive = false);
    bool isCorrectWord(QString word);
    QStringList getSuggestList(QString word);
    void setTextCursor(QTextCursor cursor);
    QString getDico();
    static QStringList getDicos(); //Comme son nom l'indique, cette fonction retourne TOUS les dictionnaires disponibles et pas seulement l'actuel
    void checkAll(QTextEdit *contenu);
    void setMotsIgnores(QStringList liste);
    static bool initPersonalDictionnary();

public slots:
    void autocorrection(QString remplacement = "");
    void remplacer(QString nmot = "");
    void setDico(QString langue = "");
    void ignoreDef(QString mot = "");
    
private slots:
    void checkWord();
    void ignore(QString mot = "");
    void addDico(QString mot = "");
    void remplacerTout();
    void options();

private:
    QString setUserDict();

    SettingsManager *settings;
    Ui::OrthManager *ui;
    SpellChecker* correcteur;
    QTextCursor pos_orth;
    QTextEdit *copyEdit;
    QStringList list_skip;
    QStringList list_skip_definitively;
    QString word;
    QString dicoActuel;

signals:
    void langueChangee();
};

#endif // ORTHMANAGER_H
