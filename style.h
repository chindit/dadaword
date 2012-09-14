#ifndef STYLE_H
#define STYLE_H

#include <QCheckBox>
#include <QColor>
#include <QDialog>
#include <QFont>
#include <QFontComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QSignalMapper>
#include <QSpinBox>
#include <QStringList>

#include "erreur.h"

class Style : public QDialog
{
    Q_OBJECT

public:
    explicit Style(QWidget *parent = 0);
    
signals:
    void styles_changed();
    
public slots:
    void affiche_fen();

private slots:
    void ajoute_style(); //À RÉÉCRIRE
    void change_couleur(int bouton);
    void enregistre_style();
    void supprime_style();

private:
    QList<QPushButton* > color_foreground, color_background;
    QList<QCheckBox* > boxGras, boxSouligne, boxItalique;
    QList<QFontComboBox* > boxPolice;
    QList<QSpinBox* > boxSize;
    QStringList liste_styles;
    int id_modif, nb_styles;
    
};

#endif // STYLE_H
