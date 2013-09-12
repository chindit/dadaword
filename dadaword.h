/*
  Développeur : David Lumaye (littletiger58.aro-base.gmail.com)
  Date : 02/04/13
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#ifndef DADAWORD_H
#define DADAWORD_H

#include <QApplication>
#include <QBuffer>
#include <QColorDialog>
#include <QDateTime>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QFont>
#include <QFontComboBox>
#include <QFontDialog>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintPreviewDialog>
#include <QProgressBar>
#include <QScrollArea>
#include <QSignalMapper>
#include <QSpinBox>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTextCodec>
#include <QTextDocumentWriter>
#include <QTextEdit>
#include <QTextList>
#include <QTextTable>
#include <QToolBar>
#include <QToolButton>
#include <QWidgetAction>
#include <iostream>
#include <math.h>

#include "autoload.h"
#include "outils.h"
#include "constantes.h"
#include "ddz.h"
#include "errorManager.h"
#include "puces.h"
#include "highlighter.h"
#include "opendocument.h"
#include "orthManager.h"
#include "replaceManager.h"
#include "settingsManager.h"
#include "specialchars.h"
#include "spellchecker.h"
#include "style.h"

class DadaWord : public QMainWindow
{
    Q_OBJECT
    
public:
    DadaWord(QWidget *parent = 0);
    ~DadaWord();
    void createUI();

public slots:
    void reloadStyles();

signals:
    void deleteAnnexes();

private slots:
    void showAbout();
    void changeFont(QFont nouvelle_police);
    void changeFont(QString police);
    void changeFont();
    void fontWeight(bool etat);
    void fontFamily(bool etat);
    void fontUndeline(bool etat);
    void changeSize(int taille);
    void changeSize();
    void printPreview();
    void save(QMdiSubWindow* fenetre_active = 0, bool saveas = false, bool autosave = false);
    void saveAll();
    void autoSave();
    void openFile(const QString &fichier = "null", bool autosave = false);
    void print(QPrinter *p);
    void odtExport(QString nom = "");
    void createList(const int ordonne = 0);
    bool eventFilter(QObject *, QEvent *);
    void incrementList();
    bool desincrementList();
    void openTab(bool fichier = false, QString titre = "null");
    void indicateur_modifications();
    void changement_focus(QMdiSubWindow *fenetre_activee);
    void closeAll();
    void change_couleur(const int &value);
    void make_undo();
    void make_redo();
    void slot_undo(bool etat);
    void slot_redo(bool etat);
    void enregistrer_sous();
    void change_align(const int &value);
    void add_image();
    void imprimer();
    void export_pdf();
    void close_tab_button(int index = -1);
    void slot_lecture_seule();
    void creer_tableau();
    void tableau_add(const int &pos);
    void tableau_remove(const int &pos);
    void to_plain_text();
    void hide_toolbar(const int identifiant);
    void make_full_screen();
    void recherche(bool remplacer = false);
    void make_search(const int from);
    void hide_searchbar(bool transfert = false);
    void statistiques();
    void change_style(int style);
    void verif_orthographe();
    void orth_autocorrection(QString remplacement);
    void couper();
    void copier();
    void coller();
    void mode_surecriture();
    void html_highlight();
    void affiche_menu_perso();
    void call_remplace();
    void curseur_change();
    void set_interligne(int interligne);
    void add_annexe();
    void rm_annexe();
    void make_rm_annexe(QString annexe);
    void show_annexes();
    void ouvre_programme(QString fichier);
    void openById(int id);
    void insertSpecialChars();
    void setSuperScript(bool etat);
    void setSubScript(bool etat);
    void hide_menubar();
    void changeEncode(int encodage);
    void updateLangue();
    void changeDetected(QString file);
    void updateTab(QString file);
    //Only for Windows
    void has_maj();

private:
    void create_menus();
    void saveAlert(QMdiSubWindow *fenetre);
    QMdiSubWindow* find_onglet();
    QTextEdit* find_edit();

    Outils *outils;
    SettingsManager *settings;
    ErrorManager *erreur;
    //SpellChecker *correcteur;
    QMdiArea *zone_centrale;
    QAction *enregistrer;
    QAction *enregistrerTout;
    QMenu *menu_format;
    QToolBar *puces;
    QToolBar *barre_tableau;
    QString *titre_doc;
    QFontComboBox *choix_police;
    QSpinBox *taille_police;
    QAction *gras;
    QAction *italique;
    QAction *souligne;
    QAction *fichier_fermer;
    QAction *fichier_closeAll;
    QColor *couleur_actuelle;
    QAction *edition_redo;
    QAction *edition_undo;
    QAction *incrementList_bouton;
    QAction *desincrementList_bouton;
    QAction *lecture_seule;
    QAction *to_text;
    QAction *exposant;
    QAction *sousExposant;
    QStatusBar *barre_etat;
    QAction *affichage_default;
    QAction *affichage_puces;
    QToolBar *barre_standard;
    QAction *affichage_tableau;
    QAction *full_screen;
    QDialog *dialog_recherche;
    QCheckBox *checkbox_case;
    QCheckBox *checkbox_mots;
    QLineEdit *champ_recherche, *champ_recherche2;
    QToolBar *barre_recherche;
    QAction *affichage_recherche;
    QString type_liste;
    QToolBar *bar_format;
    QComboBox *nom_format;
    QAction *affichage_format;
    QPushButton *status_is_modified;
    QPushButton *status_surecriture;
    QPushButton *status_langue;
    QAction *colore_html;

    QFileSystemWatcher *listFiles;
    QTextCursor pos_orth;
    QString lastDir;
    QString justSaved;
    HtmlHighlighter *instance;
    QTextCursor pos_recherche;
    QLineEdit *le_remplace;
    QAction *add_ddz_annexe;
    QAction *rm_ddz_annexe;
    QComboBox *ddz_annexes;
    QList< QStringList > liste_annexes;
    QToolBar *barre_edition;
    QAction *affichage_edition;

};

#endif // DADAWORD_H
