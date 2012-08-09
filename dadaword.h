/*
  Développeur : David Lumaye (littletiger58@gmail.com)
  Date : 01/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#ifndef DADAWORD_H
#define DADAWORD_H

#include <QApplication>
#include <QBuffer>
#include <QColorDialog>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QFont>
#include <QFontComboBox>
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
#include <QScrollBar>
#include <QSignalMapper>
#include <QSpinBox>
#include <QStatusBar>
#include <QTextCodec>
#include <QTextDocumentWriter>
#include <QTextEdit>
#include <QTextList>
#include <QTextTable>
#include <QToolBar>
#include <QToolButton>

#include "constantes.h"
#include "erreur.h"
#include "fen_puces.h"
#include "highlighterhtml.h"
#include "outils.h"
#include "opendocument.h"
#include "spellchecker.h"

class DadaWord : public QMainWindow
{
    Q_OBJECT
    
public:
    DadaWord(QWidget *parent = 0);
    ~DadaWord();
    void cree_iu();

private slots:
    void affiche_about();
    void change_police(QFont nouvelle_police);
    void graisse_police(bool etat);
    void italique_police(bool etat);
    void souligne_police(bool etat);
    void change_taille(int taille);
    void apercu_avant_impression();
    void enregistrement(QMdiSubWindow* fenetre_active = 0, bool saveas = false);
    void ouvrir_fichier(const QString &fichier = "null");
    void print(QPrinter *p);
    void export_odt();
    void create_liste_puce(const int ordonne = 0);
    bool eventFilter(QObject *, QEvent *);
    void incremente_puce();
    void desincremente_puce();
    void ouvre_onglet(bool fichier = false, QString titre = "null");
    void indicateur_modifications();
    void changement_focus(QMdiSubWindow *fenetre_activee);
    void fermer_fichier();
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
    void close_tab_button(int index);
    void slot_lecture_seule();
    void creer_tableau();
    void tableau_add(const int &pos);
    void tableau_remove(const int &pos);
    void to_plain_text();
    void hide_toolbar(const int identifiant);
    void make_full_screen();
    void recherche();
    void make_search(const int from);
    void hide_searchbar(bool transfert = false);
    void statistiques();
    void change_style(int style);
    void verif_orthographe();
    void orth_ignore();
    void orth_dico();
    void orth_remplace(QString mot);
    void orth_remplace_all();
    void orth_langue();
    void orth_stop();
    void couper();
    void copier();
    void coller();
    void mode_surecriture();
    void html_highlight();

private:
    void create_menus();
    void alerte_enregistrement(QMdiSubWindow *fenetre);
    QMdiSubWindow* find_onglet();
    QTextEdit* find_edit();

    QMdiArea *zone_centrale;
    QTextDocument *doc_principal;
    QAction *enregistrer;
    QToolBar *puces;
    QToolBar *barre_tableau;
    QString *titre_doc;
    QFontComboBox *choix_police;
    QSpinBox *taille_police;
    QAction *gras;
    QAction *italique;
    QAction *souligne;
    QColor *couleur_actuelle;
    QAction *edition_redo;
    QAction *edition_undo;
    QAction *incremente_puce_bouton;
    QAction *desincremente_puce_bouton;
    QAction *lecture_seule;
    QAction *to_text;
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
    QAction *colore_html;
    //Orthographe
    QToolBar *barre_orthographe;
    QComboBox *orth_suggest;
    QLabel *orth_mot;
    QTextCursor pos_orth;
    QString orth_erreur;
    QString dictPath;
    QStringList list_skip;
    HighlighterHtml *instance;

};

#endif // DADAWORD_H
