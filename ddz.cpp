#include "ddz.h"

DDZ::DDZ()
{
}

bool DDZ::enregistre(QString fichier, QString contenu, QStringList annexes){
    //Paramètres de fonction
    Erreur instance_erreur;

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

    if(annexes.size() > 0){
        QString liste_annexes;
        for(int i=0; i<annexes.size(); i++){
            QFile fichier(annexes.at(i));
            fichier.open(QFile::ReadOnly);
            ddz_global.addFile(annexes.at(i).split("/").last(), fichier.readAll());
            fichier.close();
            liste_annexes+=annexes.at(i).split("/").last()+",";
        }
        liste_annexes = liste_annexes.left(liste_annexes.size()-1);
        //On ajoute un fichier descriptif


        QByteArray array_annexes;
        array_annexes.append(liste_annexes);
        ddz_global.addFile("annexes.txt", array_annexes);
    }

    //On ferme tout
    ddz_global.close();


    return true;
}

QStringList DDZ::ouvre(QString nom){
    //Variables globales
    QString contenu;
    QStringList retour;
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
        retour.append(contenu);
    }
    else{
        instance_erreur.Erreur_msg(QObject::tr("DDZ : Erreur lors de la lecture du contenu du document."), QMessageBox::Ignore);
        retour.append("NULL");
        return retour;
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
            retour.append("NULL");
            return retour;
        }

        //On change le chemin des images
        for(int i=0; i<list.size(); i++){
            QString nom_image = list.at(i).split("/").last();
            contenu.replace(list.at(i), QDir::tempPath()+QDir::separator()+nom_image);
        }
    }

    QFile index_annexes(QDir::tempPath()+"/annexes.txt");
    if(index_annexes.exists()){
        //Il y a des annexes
        index_annexes.open(QFile::ReadOnly);
        QString liste = index_annexes.readLine();
        index_annexes.close();
        QStringList liste_annexes = liste.split(",");
        if(liste_annexes.size() > 0){
            for(int i=0; i<liste_annexes.size(); i++){
                QString temp = liste_annexes.at(i);
                retour.append(temp.prepend(QDir::tempPath()+"/"));
            }
            //Une fois qu'on a lu le fichier, on le supprime
            if(!index_annexes.remove()){
                instance_erreur.Erreur_msg(QObject::tr("Impossible de supprimer le fichier d'annexes.  Il se peut que le système soit inopérant."), QMessageBox::Warning);
            }
            return retour;
        }
        else{
            instance_erreur.Erreur_msg(QObject::tr("Erreur lors de la détection des annexes"), QMessageBox::Ignore);
            return retour;
        }
    }

    return retour;
}
