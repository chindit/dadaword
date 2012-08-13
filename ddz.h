#ifndef FICHIERSDDZ_H
#define FICHIERSDDZ_H

//Includes
#include "dadaword.h"
#include "opendocument.h"
#include "qzipwriter.h"
#include <QImageReader>

class DDZ
{
public:
    DDZ();
    bool enregistre(QString fichier, QString contenu);
    QString ouvre(QString nom);
};

#endif // FICHIERSDDZ_H
