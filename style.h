#ifndef STYLE_H
#define STYLE_H

#include <QCheckBox>
#include <QColor>
#include <QColorDialog>
#include <QDialog>
#include <QFont>
#include <QFontComboBox>
#include <QGridLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSettings>
#include <QSignalMapper>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStringList>

#include "errorManager.h"

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
    void disableItem();

private:
    QList<QPushButton* > color_foreground, color_background;
    QList<QCheckBox* > boxGras, boxSouligne, boxItalique, boxSavePolice, boxSaveSize, boxSaveForeground, boxSaveBackground;
    QList<QFontComboBox* > boxPolice;
    QList<QSpinBox* > boxSize;
    QStringList liste_styles;
    int id_modif, nb_styles;
    
};

#endif // STYLE_H
