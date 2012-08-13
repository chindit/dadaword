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
    QString nom_fichier = fichier.remove(0, (instance_outils.compte_caracteres(fichier)+1));
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
            QString nom_fichier = liste_temp.remove(0, (instance_outils.compte_caracteres(liste_temp)+1));
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
