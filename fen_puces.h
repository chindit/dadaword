/*
  Développeur : David Lumaye (littletiger58.aro-base.gmail.com)
  Date : 01/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#ifndef FEN_PUCES_H
#define FEN_PUCES_H

#include <QDialog>
#include <QRadioButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QString>
#include <QTextListFormat>
#include "errorManager.h"

class fen_puces : public QDialog
{
    Q_OBJECT
public:
    explicit fen_puces(QWidget *parent = 0);
    const QString get_radio();
    
signals:
    
public slots:

private:
     QRadioButton *radio1;
     QRadioButton *radio2;
     QRadioButton *radio3;
     QRadioButton *radio4;
     QRadioButton *radio5;
    
};

#endif // FEN_PUCES_H
