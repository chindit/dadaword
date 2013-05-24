#include "orthManager.h"
#include "ui_orthManager.h"

OrthManager::OrthManager(QWidget *parent, QTextEdit *contenu) :
    QDialog(parent),
    ui(new Ui::OrthManager)
{
    ui->setupUi(this);
    ui->contenu_texte->setCursor(contenu->cursor());
    ui->contenu_texte->setDocument(contenu->document());
}

OrthManager::~OrthManager()
{
    delete ui;
}
