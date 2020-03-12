#ifndef SPELL_CHECKER_WIDGET_H
#define SPELL_CHECKER_WIDGET_H

#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QModelIndexList>
#include <QStandardItem>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QTextEdit>
#include <QTextFrame>

#include <errorManager.h>
#include <settingsManager.h>
#include <spellchecker.h>

namespace Ui {
    class SpellCheckerWidget;
}

class SpellCheckerWidget : public QDialog {
Q_OBJECT

public:
    explicit SpellCheckerWidget(const QString &dictionaryName, QWidget *parent = nullptr);

    ~SpellCheckerWidget() override;
    void showWindow(QTextEdit *contenu);
    QStringList getListSkip(bool definitive = false);
    bool isCorrectWord(QString word);
    QStringList getSuggestList(QString word);
    void setTextCursor(QTextCursor cursor);
    QString getDico();
    static QStringList
    getDicos(); //Comme son nom l'indique, cette fonction retourne TOUS les dictionnaires disponibles et pas seulement l'actuel
    void checkAll(QTextEdit *contenu);
    void setMotsIgnores(QStringList liste);
    static bool initPersonalDictionary();

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
    Ui::SpellCheckerWidget *ui;
    SpellChecker *correcteur;
    QTextCursor pos_orth;
    QStringList list_skip;
    QStringList list_skip_definitively;
    QString word;
    QString currentDictionary;

signals:
    void langueChangee();
};

#endif // SPELL_CHECKER_WIDGET_H
