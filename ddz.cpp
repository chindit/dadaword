#include "ddz.h"

DDZ::DDZ()
{
}

bool DDZ::enregistre(QString fichier, QString contenu){
    //Paramètres de fonction
    Erreur instance_erreur;
    Outils instance_outils;

    QZipWriter ddz_global(fichier, QIODevice::WriteOnly);


    //On remplit le fichier avec le contenu
    QString nom_fichier = fichier.split("/").last();
    //Le nom du fichier est le même que le nom de l'archive globale si ce n'est que l'extention change
    nom_fichier.remove(nom_fichier.size()-4, nom_fichier.size()).append(".ddw");


    QByteArray array_contenu;
    array_contenu.append(contenu);
    ddz_global.addFile(nom_fichier, array_contenu);

    //On regarde s'il y avait des images
    int nb_images = contenu.count("<img src=");

    //Il y a des images, on rentre dans la boucle
    if(nb_images > 0){
        //Définition de la RegExp
        QRegExp regexp_images("<img[^>]*src=\"([^\"]*)");

        //Récupération des images
        int pos = 0; QStringList list;
        while ((pos = regexp_images.indexIn(contenu, pos)) != -1){
            list << regexp_images.cap(1);
            pos += regexp_images.matchedLength();
        }

        if(nb_images != list.size()){
            //On a pas trouvé toutes les images (ou on en a trouvé trop, ce qui est pire)
            instance_erreur.Erreur_msg(QObject::tr("Problème lors de la détection d'images -> annulation de la sauvegarde"), QMessageBox::Ignore);
            return false;
        }

        //Si on est ici, c'est que tout roule
        //On ajoute les images détectées au zip
        for(int i=0; i<list.size(); i++){
            QString liste_temp = list.at(i);
            QString nom_fichier = liste_temp.split("/").last();
            //On récupère l'extention
            QString extention = list.at(i).section('.', -1);
            QImageReader image(list.at(i), extention.toUpper().toStdString().c_str());
            ddz_global.addFile(nom_fichier, image.device());
        }
    }

    //On ferme tout
    ddz_global.close();


    return true;
}

QString DDZ::ouvre(QString nom){
    //Variables globales
    QString contenu;
    Erreur instance_erreur;

    //---------------------------------------------
    //Extraction de l'archive
    //---------------------------------------------

    //Instance QZipReader
    QZipReader ddz_global(nom, QIODevice::ReadOnly);
    //Extraction
    ddz_global.extractAll(QDir::tempPath());


    //---------------------------------------------
    //Ouverture du fichier principal (DDW)
    //---------------------------------------------
    QString nom_fichier = nom.split("/").last();
    nom_fichier.remove(nom_fichier.size()-4, nom_fichier.size()).append(".ddw").prepend(QDir::tempPath()+"/");
    QFile file(nom_fichier);
    if(file.open(QFile::ReadOnly)){
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            contenu = contenu + line + "\n";
        }
        file.close();
    }
    else{
        instance_erreur.Erreur_msg(QObject::tr("DDZ : Erreur lors de la lecture du contenu du document."), QMessageBox::Ignore);
        return "NULL";
    }

    //-----------------------------------------------------
    //Renommage des images contenues (selon l'OS)
    //-----------------------------------------------------
    int nb_images = contenu.count("<img src=");

    //Il y a des images, on rentre dans la boucle
    if(nb_images > 0){
        //Définition de la RegExp
        QRegExp regexp_images("<img[^>]*src=\"([^\"]*)");

        //Récupération des images
        int pos = 0; QStringList list;
        while ((pos = regexp_images.indexIn(contenu, pos)) != -1){
            list << regexp_images.cap(1);
            pos += regexp_images.matchedLength();
        }

        if(nb_images != list.size()){
            //On a pas trouvé toutes les images (ou on en a trouvé trop, ce qui est pire)
            instance_erreur.Erreur_msg(QObject::tr("Problème lors de la détection d'images -> annulation de l'ouverture"), QMessageBox::Ignore);
            return "NULL";
        }

        //On change le chemin des images
        for(int i=0; i<list.size(); i++){
            QString nom_image = list.at(i).split("/").last();
            contenu.replace(list.at(i), QDir::tempPath()+QDir::separator()+nom_image);
        }
    }

    return contenu;
}
