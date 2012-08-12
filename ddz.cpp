#include "ddz.h"

DDZ::DDZ()
{
}

bool DDZ::enregistre(QString fichier, QString contenu){
    //Paramètres de fonction
    Erreur instance_erreur;
    Outils instance_outils;

    //Création de l'instance
    QuaZip ddz_document(fichier);
    //Codec
    ddz_document.setFileNameCodec("UTF-8");

    //On ouvre en écrasant/créant
    if(!ddz_document.open(QuaZip::mdCreate)){
        //On a besoin du QObject parce qu'on ne dérive pas de la classe
        instance_erreur.Erreur_msg(QObject::tr("Une erreur est survenue lors de l'ouverture en écriture du fichier DDZ"), QMessageBox::Ignore);
        return false;
    }

    //On remplit le fichier avec le contenu
    QuaZipFile ddz_contenu(&ddz_document);
    QString nom_fichier = fichier.remove(0, (instance_outils.compte_caracteres(fichier)+1));
    //Le nom du fichier est le même que le nom de l'archive globale si ce n'est que l'extention change
    nom_fichier.remove(nom_fichier.size()-4, nom_fichier.size()).append(".ddw");
    ddz_contenu.open(QIODevice::WriteOnly, QuaZipNewInfo(nom_fichier));

    //On remplit le fichier
    QTextStream flux(&ddz_contenu);
    flux << contenu;
    ddz_contenu.close();

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
            QuaZipFile ddz_image(&ddz_document); //Instance pour écrire l'image
            QFile fichier_image(list.at(i));
            QString liste_temp = list.at(i);
            QString nom_fichier = liste_temp.remove(0, (instance_outils.compte_caracteres(liste_temp)+1));
            ddz_contenu.open(QIODevice::WriteOnly, QuaZipNewInfo(nom_fichier));
            char c; //Sert pour le transfert des données
            if(fichier_image.open(QIODevice::ReadOnly)){
                while(fichier_image.getChar(&c) && ddz_image.putChar(c));
                if(ddz_image.getZipError() != UNZ_OK) {
                    instance_erreur.Erreur_msg(QObject::tr("DDZ : Erreur lors de l'écriture des images"), QMessageBox::Ignore);
                    return false;
                }
                fichier_image.close();
                ddz_image.close();
            }
            else{
                instance_erreur.Erreur_msg(QObject::tr("Impossible d'ouvrir le fichier image"), QMessageBox::Ignore);
                return false;
            }
        }
    }

    //On ferme tout
    ddz_document.close();


    return true;
}
