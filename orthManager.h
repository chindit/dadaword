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
    QStringList getListSkip();
    bool isCorrectWord(QString word);
    QStringList getSuggestList(QString word);
    void setTextCursor(QTextCursor cursor);
    QString getDico();

public slots:
    void autocorrection(QString remplacement = "");
    void remplacer(QString nmot = "");
    void setDico(QString langue = "");
    
private slots:
    void checkWord();
    void ignore(QString mot = "");
    void addDico(QString mot = "");
    void ignoreDef();
    void remplacerTout();
    void options();

private:
    QString setUserDict();

    Ui::OrthManager *ui;
    SpellChecker* correcteur;
    QTextCursor pos_orth;
    QStringList list_skip;
    QString word;
    QString dicoActuel;

signals:
    void langueChangee();
};

#endif // ORTHMANAGER_H
