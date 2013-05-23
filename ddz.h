#ifndef FICHIERSDDZ_H
#define FICHIERSDDZ_H

//Includes
#include "dadaword.h"
#include "opendocument.h"
#include "qzipwriter.h"
#include "qzipreader.h"
#include <QImageReader>

class DDZ
{
public:
    explicit DDZ();
    bool enregistre(QString fichier, QString contenu, QString langue, QStringList annexes);
    QStringList ouvre(QString nom);
};

#endif // FICHIERSDDZ_H
