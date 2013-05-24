#include "orthManager.h"
#include "ui_orthManager.h"

OrthManager::OrthManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OrthManager)
{
    ui->setupUi(this);
}

OrthManager::~OrthManager()
{
    delete ui;
}
