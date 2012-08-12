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
        instance_erreur.Erreur_msg(QObject::tr("Une erreur est survenue lors de l'ouverture en écriture du fichier DDZ"), QMessageBox::Warning);
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

    //On regarde s'il y avait des images
    int nb_images = contenu.count("<img src=");


    //On ferme tout
    ddz_contenu.close();
    ddz_document.close();


    return true;
}
