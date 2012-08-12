#ifndef FICHIERSDDZ_H
#define FICHIERSDDZ_H

//Includes
#include "dadaword.h"
#include "opendocument.h"
#include "qzipwriter.h"
//#include "quazip/quazipnewinfo.h"

class DDZ
{
public:
    DDZ();
    bool enregistre(QString fichier, QString contenu);
};

#endif // FICHIERSDDZ_H
