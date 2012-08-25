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
    
public slots:
    void affiche_fen();

private slots:
    void ajoute_style();
    void change_couleur(int bouton);
    void enregistre_style();

private:
    QLineEdit *line_edite_nom_style;
    QFontComboBox *combo_police;
    QSpinBox *box_taille;
    QCheckBox *checkbox_gras, *checkbox_souligne, *checkbox_italique;
    QPushButton *color_foreground, *color_background;
    
};

#endif // STYLE_H
