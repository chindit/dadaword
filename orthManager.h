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
    explicit OrthManager(QWidget *parent, QTextEdit *contenu);
    ~OrthManager();
    
private slots:
    void checkWord();
    void ignore();
    void addDico();
    void ignoreDef();
    void remplacer();
    void remplacerTout(QString remplace = "");

private:
    Ui::OrthManager *ui;
    SpellChecker* correcteur;
    QTextCursor pos_orth;
    QStringList list_skip;
    QString word;
};

#endif // ORTHMANAGER_H
