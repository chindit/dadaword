#include "specialchars.h"

specialChars::specialChars(QWidget *parent) :
    QDialog(parent)
{
}

void specialChars::showWindow(QList<QChar> listRecent){
    setWindowTitle(tr("Insérer un caractère spécial"));
    setWindowIcon(QIcon(":/programme/images/dadaword.gif"));
    setMinimumWidth(475);
    setMinimumHeight(375);
    setWindowModality(Qt::ApplicationModal);

    QVBoxLayout *layout = new QVBoxLayout(this);
    tableau = new QTableWidget(817, 16, this);
    for(int i=0; i<16; i++){
        tableau->setColumnWidth(i, 25);
    }
    connect(tableau, SIGNAL(cellClicked(int,int)), this, SLOT(updateTable(int,int)));

    //On remplit le tableau
    QTableWidgetItem *item[817*16];
    int caractere = 0x0000;

    for(int i=0; i<817; i++){
        for(int j=0; j<16; j++){
            item[i*j] = new QTableWidgetItem(QChar(caractere));
            tableau->setItem(i, j, item[i*j]);
            caractere++;
        }
    }
    layout->addWidget(tableau);

    liste = listRecent;
    recentChars = new QTableWidget(1, 10, this);
    for(int i=0; i<10; i++){
        QTableWidgetItem *item = new QTableWidgetItem;
        if(i < liste.size()){
            item->setText(listRecent.at(i));
        }
        recentChars->setItem(0, i, item);
        recentChars->setColumnWidth(i, 25);
    }
    recentChars->setMaximumSize(275, 60);
    connect(recentChars, SIGNAL(cellClicked(int,int)), this, SLOT(updateTable2(int,int)));
    QLabel *labelRecentChars = new QLabel(tr("Caractères récemment utilisés"));
    layout->addWidget(labelRecentChars);
    layout->addWidget(recentChars);
    lineChars = new QLineEdit;
    layout->addWidget(lineChars);
    QPushButton *validate = new QPushButton(QIcon(":/menus/images/ok.png"), "OK");
    connect(validate, SIGNAL(clicked()), this, SLOT(close()));
    layout->addWidget(validate, 0, Qt::AlignRight);
    exec();

    return;
}

void specialChars::updateTable(int row, int coll){
    if(row < 0){
        QTableWidgetItem *item = recentChars->item(0, coll);
        lineChars->setText(lineChars->text()+item->text());
        return;
    }
    QTableWidgetItem *item = tableau->item(row, coll);
    lineChars->setText(lineChars->text()+item->text());
    liste.prepend(item->text().at(0));
    if(liste.size() == 11){
        liste.removeLast();
    }
    for(int i=0; i<10; i++){
        QTableWidgetItem *temp = new QTableWidgetItem;
        if(i < liste.size()){
            temp->setText(liste.at(i));
        }
        recentChars->setItem(0, i, temp);
    }
    recentChars->show();
}

void specialChars::updateTable2(int row, int coll){
    updateTable(-1, coll); //If we send a negative row, updateTable'll know we're adding from recents
}

QString specialChars::getChars(){
    return lineChars->text();
}

QList<QChar> specialChars::getList(){
    return liste;
}
