/*
  Développeur : David Lumaye (littletiger58@gmail.com)
  Date : 01/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#ifndef OPENDOCUMENT_H
#define OPENDOCUMENT_H

#include <QMessageBox>
#include <QObject>
#include <QTextBrowser>
#include <QTextStream>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtXml>
#include <iostream>

//Librairies Zip
#include "qzipreader.h"

//Classes Perso
#include "erreur.h"
#include "dadaword.h"

class OpenDocument : public QObject
{
    Q_OBJECT
public:
    explicit OpenDocument(QObject *parent = 0);
    ~OpenDocument();
    QString ouvre_odt(QString nom);
    
signals:
    
public slots:

private:
    void read_xml(QString fichier);
    void read_styles(QDomElement e);
    bool style_paragraphe(QString nom, QDomElement e);
    bool style_graphics(QString nom, QDomElement e);
    void read_body(QDomElement e);
    bool contenu_paragraphe(QDomElement e, QTextCursor &cursor, bool puces = false, bool h_item = false, bool tableau = false);
    bool contenu_puces(QDomElement e, QTextCursor &curseur, int niveau = 1, QString style = "null");
    bool contenu_tableaux(QDomElement e, QTextCursor &curseur);
    QTextCharFormat cree_bloc_format(QString nom);
    QTextBlockFormat cree_bloc_format2(QString nom);
    QTextImageFormat cree_image_format(QString nom);
    bool traite_span(QTextCharFormat format, QTextCursor &curseur, QDomElement e, bool puces = false, bool tableau = false);
    bool traite_lien(QTextCursor &curseur, QDomElement e, QTextCharFormat format);
    bool traite_image(QTextCursor &curseur, QDomElement e, QString nom);
    QString nettoye_code(QString code);

    QList<QMultiMap<QString, QString> > styles;
    QTextDocument *document;
    QString nom_odt;
    QString contenu_puce;
    int p_total, p_current;
    QStringList ligne_tableau;
    QString case_tableau;
    
};

#endif // OPENDOCUMENT_H
