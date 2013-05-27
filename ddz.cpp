#include "ddz.h"

DDZ::DDZ()
{
}

bool DDZ::enregistre(QString fichier, QString contenu, QString langue, QStringList annexes, QStringList ignore){
    //Paramètres de fonction
    ErrorManager instance_erreur;

    QZipWriter ddz_global(fichier, QIODevice::WriteOnly);

    //Création d'un XML global pour les préférences
    QDomDocument preferences;
    QDomElement xmlLangue = preferences.createElement("langue");
    QDomText xmlLangueTexte = preferences.createTextNode(langue);
    xmlLangue.appendChild(xmlLangueTexte);
    preferences.appendChild(xmlLangue);

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
        //ddz_global.addDirectory("images");
        for(int i=0; i<list.size(); i++){
            QString liste_temp = list.at(i);
            QString nom_fichier = liste_temp.split("/").last();
            //nom_fichier.prepend("images/");
            //On récupère l'extention
            QString extention = list.at(i).section('.', -1);
            QImageReader image(list.at(i), extention.toUpper().toStdString().c_str());
            ddz_global.addFile(nom_fichier, image.device());
        }
    }

    //Traitement des annexes
    if(annexes.size() > 0){
        QDomNode xmlAnnexe = preferences.createElement("annexes");
        for(int i=0; i<annexes.size(); i++){
            QFile fichier(annexes.at(i));
            fichier.open(QFile::ReadOnly);
            ddz_global.addFile(annexes.at(i).split("/").last(), fichier.readAll());
            fichier.close();
            //Ajout des annexes au XML global
            QDomElement thisAnnexe = preferences.createElement("annexe");
            QDomText thisText = preferences.createTextNode(annexes.at(i).split("/").last());
            thisAnnexe.appendChild(thisText);
            xmlAnnexe.appendChild(thisAnnexe);
            preferences.appendChild(xmlAnnexe);
        }
    }

    //Traitement des mots à ignorer définitivement (ajout dans le XML de config)
    if(ignore.size() >= 1){
        QDomNode xmlIgnore = preferences.createElement("orthographe");
        for(int i=0; i<ignore.count(); i++){
            QDomElement motIgnore = preferences.createElement("mot");
            QDomText motTexte = preferences.createTextNode(ignore.at(i));
            motIgnore.appendChild(motTexte);
            xmlIgnore.appendChild(motIgnore);
            preferences.appendChild(xmlIgnore);
        }
    }

    //Enregistrement des préférences
    QDomNode noeud = preferences.createProcessingInstruction("xml","version=\"1.0\"");
    preferences.insertBefore(noeud, preferences.firstChild());
    QByteArray array_prefs;
    array_prefs.append(preferences.toString());
    ddz_global.addFile("config.xml", array_prefs);

    //On ferme tout
    ddz_global.close();


    return true;
}

QStringList DDZ::ouvre(QString nom){
    //Variables globales
    QString contenu;
    QStringList retour;
    ErrorManager instance_erreur;

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
        contenu = in.readAll();
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
    int nb_images = retour.at(0).count("<img src=");

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
            QString temp = retour.at(0);
            temp.replace(list.at(i), QDir::tempPath()+"/"+nom_image);
            retour.removeFirst();
            retour.prepend(temp);
        }
    }

    //Ouverture de la configuration
    QFile fileConfig(QDir::tempPath()+"/config.xml");
    QDomDocument config;
    if(fileConfig.open(QFile::ReadOnly)){
        config.setContent(&fileConfig);
        fileConfig.close();
    }
    else{
        instance_erreur.Erreur_msg(QObject::tr("DDZ : Erreur lors de la lecture du contenu du document."), QMessageBox::Ignore);
        retour.append("NULL");
        return retour;
    }

    QDomElement racine = config.documentElement();
    //Lecture de la langue
    QString langue = racine.childNodes().at(0).nodeValue();
    QRegExp is_dico("^[a-z]{2}_[A-Z]{2}$");
    if(is_dico.exactMatch(langue)){
        retour.append(langue);
    }
    else{
        retour.append("default");
    }

    QDomNodeList liste_annexes = racine.elementsByTagName("annexe");
    if(!liste_annexes.isEmpty()){
        for(int i=0; i<liste_annexes.count(); i++){
            retour.append(QDir::tempPath()+"/"+liste_annexes.at(i).toElement().text());
        }
    }

    return retour;
}
