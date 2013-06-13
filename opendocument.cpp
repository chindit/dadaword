/*
  Développeur : David Lumaye (littletiger58.aro-base.gmail.com)
  Date : 01/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#include "opendocument.h"

//Constructeur
OpenDocument::OpenDocument(QObject *parent) :
    QObject(parent)
{
}

//Destructeur
OpenDocument::~OpenDocument(){
    //On vire les éléments qu'on a créé pour éviter les fuites de mémoire
    delete document;
    //On vire la QProgressBar
    //DadaWord::visibility_progress_bar(false);
}

QString OpenDocument::ouvre_odt(QString nom){
    //Variables globales
    QString contenu = "";
    ErrorManager instance_erreur;
    document = new QTextDocument;
    nom_odt = nom;
    contenu_puce = "";

    //---------------------------------------------
    //Extraction de l'archive
    //---------------------------------------------
    //Instance QZipReader
    QZipReader odt_global(nom, QIODevice::ReadOnly);
    //Extraction
    odt_global.extractOne(QDir::tempPath(), "content.xml");

    //---------------------------------------------
    //Ouverture du fichier principal (DDW)
    //---------------------------------------------
    QFile contenu_odt(QDir::tempPath()+QDir::separator()+"content.xml");
    if(!contenu_odt.exists()){
        instance_erreur.Erreur_msg(tr("ODT : échec de la sélection du contenu"), QMessageBox::Ignore);
        return "NULL"; //Erreur, on quitte
    }
    if(!contenu_odt.open(QFile::ReadOnly)){
        instance_erreur.Erreur_msg(tr("ODT : échec de l'ouverture du fichier de contenu"), QMessageBox::Ignore);
        return "NULL"; //Erreur, on quitte
    }

    QTextStream contents_xml(&contenu_odt);
    //On stocke le contenu dans le QString, c'est plus manipulable
    contenu = contents_xml.readAll();
    //Fermeture du fichier
    contenu_odt.close();
    //Fermeture du ZIP
    odt_global.close();

    //Avant de tout renvoyer à la fonction, il faut traiter le XML en HTML (youpie! :-( )
    read_xml(contenu);
    return document->toHtml();
}

void OpenDocument::read_xml(QString fichier){
    //Comptage des <p>
    p_total = (fichier.count("text:p")/2); // /2 parce qu'on ne compte pas la fermeture
    p_current = 0;

    //Initialisation des variables utiles pour la fonction
    //QString fichier_html;

    QDomDocument *dom = new QDomDocument();
    dom->setContent(fichier);

    QDomElement e = dom->documentElement();
    QDomElement content_styles = e.firstChildElement("office:automatic-styles");

    //On lit les styles
    read_styles(content_styles);

    //On lit le contenu
    QDomElement content_body = e.firstChildElement("office:body");
    QDomElement content_text = content_body.firstChildElement("office:text");
    read_body(content_text);

    //Renvoi du XML traité
    //return fichier_html;
    return;
}

//Lecture des styles du document
void OpenDocument::read_styles(QDomElement e){

    //Déclaration de l'instance d'erreurs
    ErrorManager instance_erreur;

    if(e.isNull()){
        return; //QDomElement invalide, on se casse
    }

    QDomNode enfants = e.firstChildElement("style:style");
    QDomElement elem = enfants.toElement();

    while(!enfants.isNull()){
        //On parcourt le fichier jusqu'au bout

        //Si c'est un élément
        if(enfants.isElement()){
            elem = enfants.toElement();//Get Elem.

            //Si c'est un style
            if(elem.tagName() == "style:style"){
                QString type = elem.attribute("style:family");
                QString nom = elem.attribute("style:name");
                if(type == "paragraph"){
                    if(!style_paragraphe(nom, elem.firstChildElement("style:text-properties"))){
                        instance_erreur.Erreur_msg(tr("Erreur lors de la lecture du style de paragraphe"), QMessageBox::Ignore);
                    }
                }
                else if(type == "text"){
                    //Même chose que les paragraphes
                    if(!style_paragraphe(nom, elem.firstChildElement("style:text-properties"))){
                        instance_erreur.Erreur_msg(tr("Erreur lors de la lecture du style de texte"), QMessageBox::Ignore);
                    }
                }
                else if(type == "graphic"){
                    //Goooo dans la nouvelle fonction "style_graphics" (sur le modèle de "style_paragraphe»
                    if(!style_graphics(nom, elem.firstChildElement("style:graphic-properties"))){
                        instance_erreur.Erreur_msg(tr("ODT : Erreur lors de la lecture du style de graphiques"), QMessageBox::Ignore);
                    }
                }
                else{
                    instance_erreur.Erreur_msg(tr("Propriété de style inconnue"), QMessageBox::Ignore);
                }
            }
        }

        //On passe au suivant
        enfants = enfants.nextSibling();
    }

    return;
}

//Style de paragraphes
bool OpenDocument::style_paragraphe(QString nom, QDomElement e){
    //Si le nom est vide, on se casse
    if(nom.isEmpty() || nom.isNull()){
        return false;
    }

    //Si on est pas positionné au bon endroit, on se casse
    if(e.tagName() != "style:text-properties"){
        return false;
    }

    QMultiMap <QString, QString> map_temp;
    map_temp.insert("name", nom);
    //On récupère manuellement les éventuels arguments
    //Police
    if(e.hasAttribute("style:font-name")){
        map_temp.insert("font-name", e.attribute("style:font-name"));
    }
    //Taille
    if(e.hasAttribute("fo:font-size")){
        map_temp.insert("font-size", e.attribute("fo:font-size"));
    }
    //Gras
    if(e.hasAttribute("fo:font-weight")){
        map_temp.insert("font-weight", e.attribute("fo:font-weight"));
    }
    //Style
    if(e.hasAttribute("fo:font-style")){
        map_temp.insert("font-style", e.attribute("fo:font-style"));
    }
    //Souligne
    if(e.hasAttribute("style:text-underline-style")){
        map_temp.insert("text-underline-style", e.attribute("style:text-underline-style"));
    }
    //Surlignage
    if(e.hasAttribute("fo:background-color")){
        map_temp.insert("background-color", e.attribute("fo:background-color"));
    }
    //Couleur de texte
    if(e.hasAttribute("fo:color")){
        map_temp.insert("color", e.attribute("fo:color"));
    }

    //On vérifie s'il n'y a pas de style de paragraphes :
    QDomNode parent = e.parentNode();
    QDomElement elem = parent.toElement();
    parent = elem.firstChildElement("style:paragraph-properties");
    if(!parent.isNull()){//Si c'est NULL, c'est qu'il n'y en a pas
        //On récupère l'alignement
        elem = parent.toElement();
        if(elem.hasAttribute("fo:text-align")){
            map_temp.insert("text-align", elem.attribute("fo:text-align"));
        }
    }

    //On vérifie si le style de paragraphe est lié à un style de puce
    parent = e.parentNode();
    elem = parent.toElement();
    if(elem.hasAttribute("style:list-style-name")){
        map_temp.insert("style-puces", elem.attribute("style:list-style-name"));
    }
    //On vérifie si le style n'est pas un <hX>
    parent = e.parentNode();
    elem = parent.toElement();
    if(elem.hasAttribute("style:parent-style-name")){
        map_temp.insert("style-h", elem.attribute("style:parent-style-name"));
    }

    //On insère tout dans la méga-QList
    styles.append(map_temp);



    return true;
}

//Lecture des styles de paragraphes
bool OpenDocument::style_graphics(QString nom, QDomElement e){
    //Si le nom est vide, on se casse
    if(nom.isEmpty() || nom.isNull()){
        return false;
    }

    //Si on est pas positionné au bon endroit, on se casse
    if(e.tagName() != "style:graphic-properties"){
        return false;
    }

    QMultiMap <QString, QString> map_temp;
    map_temp.insert("name", nom);
    //On récupère manuellement les éventuels arguments
    //Position horizontale
    if(e.hasAttribute("style:horizontal-pos")){
        map_temp.insert("horizontal-pos", e.attribute("style:horizontal-pos"));
    }

    //On oublie pas de mettre le style dans la QList générale des styles
    styles.append(map_temp);

    return true;
}

//Lecture du contenu du document
void OpenDocument::read_body(QDomElement e){
    ErrorManager instance_erreur;
    QTextCursor curseur(document);
    curseur.movePosition(QTextCursor::End);

    QDomNode enfants = e.firstChild();

    //On parcours tout le document
    while(!enfants.isNull()){
        QDomElement elem = enfants.toElement();
        QString test = elem.tagName();
        if(elem.tagName() == "text:p"){
            if(!contenu_paragraphe(elem, curseur)){
                instance_erreur.Erreur_msg(tr("ODT : Une erreur est survenue lors de la lecture d'un paragraphe"), QMessageBox::Warning);
            }
        }
        else if(elem.tagName() == "text:list"){
            if(!contenu_puces(elem, curseur)){
                instance_erreur.Erreur_msg(tr("ODT : Une erreur est survenue lors de la lecture d'une liste à puces; elle ne sera pas affichée"), QMessageBox::Warning);
            }
            else{
                QTextCursor curseur(document);
                curseur.movePosition(QTextCursor::End);
                curseur.insertBlock();
                curseur.movePosition(QTextCursor::PreviousBlock);
                curseur.insertHtml(contenu_puce);
                contenu_puce = "";
            }
        }
        else if(elem.tagName() == "table:table"){
            if(!contenu_tableaux(elem, curseur)){
                instance_erreur.Erreur_msg(tr("ODT : Une erreur est survenue lors de la lecture des tableaux; il est donc probable qu'il ne s'affiche pas."), QMessageBox::Warning);
            }
        }
        else if(elem.tagName() == "text:h"){
            contenu_paragraphe(elem, curseur, false, true);
        }
        else if(elem.tagName() == "text:sequence-decls"){
            //On s'en fout, c'est juste pour ne pas générer d'erreur
        }
        else{
            instance_erreur.Erreur_msg(tr("ODT : Type de contenu non-détecté!  Il ne sera pas affiché : %1").arg(elem.tagName()), QMessageBox::Warning);
        }

        enfants = enfants.nextSibling();
    }

    return;
}

//Lecture des paragraphes
bool OpenDocument::contenu_paragraphe(QDomElement e, QTextCursor &curseur, bool puces, bool h_item, bool tableau){
    ErrorManager instance_erreur;
    p_current++;
    case_tableau = "";
    //On change la QProgressBar
    //chargement->

    QString nom_style = e.attribute("text:style-name", "default");
    QTextCharFormat format = cree_bloc_format(nom_style);

    //On récupère le format de bloc
    QTextBlockFormat format_bloc = cree_bloc_format2(nom_style);
    //On ajoute un marginTop de 5 pour plus de lisibilité
    format_bloc.setTopMargin(2);

    //Style spécifique aux puces
    if(puces || h_item){
        int id_style = -1;
        for(int i=0; i<styles.size(); i++){
            //On parcourt les styles
            if(id_style >= 0){
                break;
            }
            for(int j=0; j<styles.at(i).size(); j++){
                //On rentre dans le QMultiMap
                if(puces){
                    if(styles.at(i).value("style-puces") == nom_style){
                        id_style = i;
                        //On sort de la boucle
                        break;
                    }
                }
                else{
                    //Ce ne peut être que le "h_item" sinon la boucle ne se déclencherait pas
                    if(styles.at(i).value("style-h") == nom_style){
                        id_style = i;
                        //On se casse
                        break;
                    }
                }
            }
        }
        if(id_style != -1){
            //On merge le style
            format.merge(cree_bloc_format(styles.at(id_style).value("style-puces")));
        }
    }

    //On applique le format au curseur
    curseur.setCharFormat(format);
    if(!tableau){
        curseur.beginEditBlock();
    }
    curseur.setBlockCharFormat(format);
    curseur.setBlockFormat(format_bloc);

    if(puces){
        contenu_puce.append("<li>");
        //On vérifie la taille
        int taille = format.fontPointSize();
        if(taille == 0){
            //Il y a eu un bug lors de la sélection du style, on applique la taille par défaut
            format.setFontPointSize(12);
        }
    }

    //Maintenant on lit les <span>
    QDomNode enfants = e.firstChild();
    while(!enfants.isNull()){
        if(enfants.isElement()){
            QDomElement type = enfants.toElement();

            //On parcours le type d'élément
            if(type.tagName() == "text:span"){
                traite_span(format, curseur, type, puces, tableau);
            }
            else if(type.tagName() == "text:a"){ //Il s'agit d'un lien
                traite_lien(curseur, type, format);
            }
            else if(type.tagName() == "text:line-break"){

            }
            else if(type.tagName() == "text:s"){
                curseur.insertText(QString(" "));
            }
            else if(type.tagName() == "text:tab"){
                curseur.insertText(QString("    "));
            }
            else if(type.tagName() == "draw:frame"){
                QDomNode enfants_image = type.firstChild();
                QString style_image = type.attribute("draw:style-name");
                if(enfants_image.toElement().tagName() == "draw:image"){
                    if(!traite_image(curseur, enfants_image.toElement(), style_image)){
                        instance_erreur.Erreur_msg(tr("ODT : Erreur lors de la lecture des images (return false)"), QMessageBox::Ignore);
                    }
                }
            }
            else if(type.tagName() == "text:list"){
                if(!contenu_puces(type, curseur)){
                    instance_erreur.Erreur_msg(tr("ODT : Une erreur est survenue lors de la lecture d'une liste à puces; elle ne sera pas affichée"), QMessageBox::Warning);
                }
                else{
                    QTextCursor curseur(document);
                    curseur.movePosition(QTextCursor::End);
                    curseur.movePosition(QTextCursor::PreviousBlock);
                    curseur.insertHtml(contenu_puce);
                    contenu_puce = "";
                }
            }
            else if(type.tagName() == "text:soft-page-break"){

            }
            else{
                instance_erreur.Erreur_msg(tr("ODT: Type de contenu non supporté : %1").arg(type.tagName()), QMessageBox::Ignore);
            }
        }
        else if(enfants.isText()){
            //On gére le texte
            if(!puces && !tableau){
                curseur.insertText(enfants.nodeValue(), format);
            }
            else if(tableau){
               case_tableau.append(enfants.nodeValue());
            }
            else{
                //Insertion du contenu des puces si on est dans un "p"
                //On récupère le style par défaut
                QTextDocument *temp = new QTextDocument;
                QTextCursor curseur(temp);
                curseur.insertText(enfants.nodeValue(), format);
                contenu_puce.append(nettoye_code(temp->toHtml()));
                delete temp;
            }

        }
        else{
            instance_erreur.Erreur_msg(tr("ODT : type de données non supporté"), QMessageBox::Ignore);
        }
        enfants = enfants.nextSibling();
    }

    //On a fini la boucle OU il n'y avait pas de <span>

    //On récupère le contenu
    if(!puces && !tableau){
        curseur.insertText("\n");
    }
    if(puces){
        contenu_puce.append("</li>");
    }
    if(!tableau){
        curseur.endEditBlock();
    }
    if(tableau){
        ligne_tableau.append(case_tableau);
    }
    //std::cout << e.text().toStdString() << std::endl;
    return true;
}

//Lecture des puces et numérotations
bool OpenDocument::contenu_puces(QDomElement e, QTextCursor &curseur, int niveau, QString style){
    //Erreurs
    ErrorManager instance_erreur;
    //Nom de style
    QString nom_style;

    if(niveau == 1 && contenu_puce == ""){
        nom_style = e.attribute("text:style-name");
    }
    else{
        nom_style = style;
    }
    //Si c'est vide, on se casse
    if(nom_style.isNull() || nom_style.isEmpty()){
        instance_erreur.Erreur_msg(tr("ODT : style de puces invalide, annulation"), QMessageBox::Ignore);
        return false;
    }

    //Début de liste
    contenu_puce.append("<ul>");

    //Création des formats
    QTextCharFormat char_style = cree_bloc_format(nom_style);
    QTextListFormat list_format;

    //indentation
    list_format.setIndent(niveau);

    QDomNode enfants = e.firstChild();
    while(!enfants.isNull()){
        QDomElement elem = enfants.toElement();
        if(elem.isElement()){
            /*if(elem.tagName() == "text:list"){ //Nouvelle liste
                //On repasse par la fonction
                contenu_puces(elem, (niveau+1));
            }*/
            if(elem.tagName() == "text:list-item"){//Ce ne peut être que ça
                QDomNode contenu = elem.firstChild();
                if(contenu.toElement().tagName() == "text:p"){
                    contenu_paragraphe(contenu.toElement(), curseur, true);
                }
                else if(contenu.toElement().isText()){
                    contenu_puce += "<li>";
                    contenu_puce += contenu.nodeValue();
                    contenu_puce += "</li>";

                }
                else if(contenu.toElement().tagName() == "text:list"){
                    //On ne fait rien, ce sera traité par le "if" suivant.
                    //Ce «else if» sert juste à éviter le message d'erreur du «else»
                }
                else{
                    instance_erreur.Erreur_msg(tr("ODT : Puces : exception dans le contenu des puces"), QMessageBox::Ignore);
                }
                QDomNode sous_enfants = enfants.firstChildElement("text:list");

                //Détection d'éventuels sous-nœuds
                if(!sous_enfants.isNull()){
                    //Quoi que ce soit, ce n'est pas un <p>, on le rebalance à la fonction
                    contenu_puces(sous_enfants.toElement(), curseur, (niveau+1));
                }
            }
        }
        enfants = enfants.nextSibling();
    }

    contenu_puce += "</ul>";
    return true;
}

//Lecture des tableaux
bool OpenDocument::contenu_tableaux(QDomElement e, QTextCursor &curseur){
    //Création de l'instance d'erreur
    ErrorManager instance_erreur;
    QList<QStringList> full_table;

    QDomNode enfants = e.firstChild();
    while(!enfants.isNull()){
        if(enfants.isElement()){
            QDomElement type = enfants.toElement();
            //On parcours le type d'élément

            //Table-column ne sert à rien.  L'important, c'est table-row et table-cell
            /*if(type.tagName() == "table:table-column"){//Tout roule
                QDomNode ligne = type.firstChildElement("table:table-row");
                QDomElement elem_ligne = ligne.toElement();
                QMessageBox::critical(0, "t", elem_ligne.tagName());
                if(elem_ligne.tagName() == "table:table-row"){
                    QMessageBox::information(0, "t", "On z'y est!");
                }
                //QMessageBox::information(0, "col", "<col></col>");
            }*/
            if(type.tagName() == "table:table-row"){
                if(!type.hasChildNodes()){
                    instance_erreur.Erreur_msg(tr("ODT : aucune cellule détectée dans le tableau"), QMessageBox::Ignore);
                    return false;
                }
                //NADA
                else{
                    //Si on a trouvé du contenu
                    QDomNode node_cell = type.firstChildElement("table:table-cell");
                    while(!node_cell.isNull()){
                        //Normalement, ce sont de simples paragraphes, donc on les envoie à la fonction de lecture des paragraphes
                        contenu_paragraphe(node_cell.firstChild().toElement(), curseur, false, false, true);
                        node_cell = node_cell.nextSibling();
                    }
                    //Si on est ici, c'est qu'on a fini de lire la ligne du tableau -> on l'insère dans la QList
                    full_table.append(ligne_tableau);
                    //On vide la QStringList
                    ligne_tableau.clear();
                }
            }
        }
        //On passe au suivant (sinon, boucle infinie, ouille!)
        enfants = enfants.nextSibling();
    }
    //Si on est ici, c'est que le tableau est rempli.
    //On le fourgue dans le curseur et hop, à l'affichage
    //HA!  Ça a marché du premier coup!!!  Je m'améliore :D
    //On insère le tableau en HTML comme ça on gère en même temps les anomalies de cases (cases soudées/divisées)
    QString mon_tableau = "<table border=\"1\">";
    for(int i=0; i<full_table.size(); i++){
        mon_tableau.append("<tr>");
        for(int j=0; j<full_table.at(i).size(); j++){
            mon_tableau.append("<td>");
            mon_tableau.append(full_table.at(i).at(j));
            mon_tableau.append("</td>");
        }
        mon_tableau.append("</tr>");
    }
    //On ferme le tableau et on l'insère
    mon_tableau.append("</table>");
    curseur.insertHtml(mon_tableau);
    return true;
}

//Création du format
QTextCharFormat OpenDocument::cree_bloc_format(QString nom){
    ErrorManager instance_erreur;
    SettingsManager settings;
    QTextCharFormat format;
    int id_style = -1;

    for(int i=0; i<styles.size(); i++){
        //On parcourt les styles
        if(id_style >= 0){
            break;
        }
        for(int j=0; j<styles.at(i).size(); j++){
            //On rentre dans le QMultiMap
            if(styles.at(i).value("name") == nom){
                id_style = i;
                //On sort de la boucle
                break;
            }
        }
    }

    if(id_style < 0){
        //On a pas trouvé de style -> on applique le style par défaut
        QString font_temp = settings.getSettings(Police).toString();
        int taille_temp = settings.getSettings(Taille).toInt();
        QFont police_default(font_temp);
        format.setFont(police_default);
        format.setFontPointSize(taille_temp);
        instance_erreur.Erreur_msg(tr("ODT : format non trouvé : application du style par défaut"), QMessageBox::Ignore);
    }
    else{
        //On applique le style trouvé
        //Police
        if(styles.at(id_style).contains("font-name")){
            QFont police(styles.at(id_style).value("font-name"));
            format.setFont(police);
        }
        //Taille
        if(styles.at(id_style).contains("font-size")){
            QString taille = styles.at(id_style).value("font-size");
            taille = taille.remove("pt");
            qreal taille_num = taille.toInt();
            format.setFontPointSize(taille_num);
        }
        else{
            //On indique la taille par défaut sinon il y a des bugs
            //format.setFontPointSize(settings.getSettings(Taille).toInt());
        }
        //Gras
        if(styles.at(id_style).contains("font-weight")){
            QString gras = styles.at(id_style).value("font-weight");
            if(gras == "bold"){
                format.setFontWeight(QFont::Bold);
            }
            else if(gras == "normal"){
                format.setFontWeight(QFont::Normal);
            }
            else{
                instance_erreur.Erreur_msg(tr("ODT : Type de graisse non supporté"), QMessageBox::Ignore);
                format.setFontWeight(QFont::Normal);
            }
        }
        //Style
        if(styles.at(id_style).contains("font-style")){
            QString italique = styles.at(id_style).value("font-style");
            if(italique == "italic"){
                format.setFontItalic(true);
            }
            else if(italique == "normal"){
                format.setFontItalic(false);
            }
            else{
                instance_erreur.Erreur_msg(tr("ODT : erreur lors de la réception de l'italique"), QMessageBox::Ignore);
                //On désactive l'italique par défaut
                format.setFontItalic(false);
            }
        }
        //Souligne
        if(styles.at(id_style).contains("text-underline-style")){
            if(styles.at(id_style).value("text-underline-style") == "solid"){
                format.setFontUnderline(true);
            }
            else if(styles.at(id_style).value("text-underline-style") == "none"){
                format.setFontUnderline(false);
            }
            else{
                instance_erreur.Erreur_msg(tr("Type de soulignement non pris en charge -> désactivation"), QMessageBox::Ignore);
                format.setFontUnderline(false);
            }
        }
        //Surlignage
        if(styles.at(id_style).contains("background-color")){
            QString test_couleur = styles.at(id_style).value("background-color");
            if(test_couleur != "transparent"){ //Si la valeur est "transparent", on ne fait rien, c'est déjà la couleur par défaut.
                format.setBackground(QBrush(QColor(styles.at(id_style).value("background-color"))));
            }
        }
        //Couleur de texte
        if(styles.at(id_style).contains("color")){
            format.setForeground(QBrush(QColor(styles.at(id_style).value("color"))));
        }
    }

    return format;
}

QTextBlockFormat OpenDocument::cree_bloc_format2(QString nom){

    QTextBlockFormat format;
    int id_style = -1;

    for(int i=0; i<styles.size(); i++){
        //On parcourt les styles
        if(id_style >= 0){
            break;
        }
        for(int j=0; j<styles.at(i).size(); j++){
            //On rentre dans le QMultiMap
            if(styles.at(i).value("name") == nom){
                id_style = i;
                //On sort de la boucle
                break;
            }
        }
    }

    if(id_style < 0){
        //On a pas trouvé de style -> on applique le style par défaut
        format.setAlignment(Qt::AlignLeft);
    }
    else{
        if(styles.at(id_style).contains("text-align") || styles.at(id_style).contains("horizontal-pos")){
            if(styles.at(id_style).value("text-align") == "start" || styles.at(id_style).value("horizontal-pos") == "start"){
                format.setAlignment(Qt::AlignLeft);
            }
            else if(styles.at(id_style).value("text-align") == "end" || styles.at(id_style).value("horizontal-pos") == "end"){
                format.setAlignment(Qt::AlignRight);
            }
            else if(styles.at(id_style).value("text-align") == "center" || styles.at(id_style).value("horizontal-pos") == "center"){
                format.setAlignment(Qt::AlignCenter);
            }
            else if(styles.at(id_style).value("text-align") == "justify" || styles.at(id_style).value("horizontal-pos") == "justify"){
                format.setAlignment(Qt::AlignJustify);
            }
            else{
                ErrorManager instance_erreur;
                instance_erreur.Erreur_msg("ODT : Alignement non trouvé", QMessageBox::Ignore);
            }
        }
    }
    return format;
}

//Crée le format d'image
QTextImageFormat OpenDocument::cree_image_format(QString nom){
    QTextImageFormat format;

    //On sélectionne le bon style
    int id_style = -1;

    for(int i=0; i<styles.size(); i++){
        //On parcourt les styles
        if(id_style >= 0){
            break;
        }
        for(int j=0; j<styles.at(i).size(); j++){
            //On rentre dans le QMultiMap
            if(styles.at(i).value("name") == nom){
                id_style = i;
                //On sort de la boucle
                break;
            }
        }
    }

    if(id_style < 0){
        //Il y a eu un bug!  On se casse
        ErrorManager instance_erreur;
        instance_erreur.Erreur_msg(tr("ODT : style d'image introuvable : %1").arg(nom), QMessageBox::Ignore);
        //return false;
    }
    else{
        //Tout est bon, on y va!
        //Centrage
        //On ne le lit pas, c'est du ressort de QTextBlockFormat
    }

    return format;
}

//Fonction qui traite les <span> (styles internes aux paragraphes)
bool OpenDocument::traite_span(QTextCharFormat format, QTextCursor &curseur, QDomElement e, bool puces, bool tableau){

    SettingsManager settings;
    ErrorManager instance_erreur;
    QTextCharFormat format_span;
    if(e.tagName() == "text:span"){
        QString nom_format = e.attribute("text:style-name");
        format_span = cree_bloc_format(nom_format);
        //On merge le format
        format.merge(format_span);
        if(!format.hasProperty(QTextFormat::FontPointSize)){
            format.setFontPointSize(settings.getSettings(Taille).toInt());
        }
    }
    else{
        instance_erreur.Erreur_msg(tr("ODT : <span> invalide"), QMessageBox::Ignore);
    }

    //Maintenant on lit les <span>
    QDomNode enfants = e.firstChild();

    while(!enfants.isNull()){
        QDomNode sous_enfants = enfants.firstChild();

        //Détection d'éventuels sous-nœuds
        if(!sous_enfants.isNull() && sous_enfants.isElement()){
            //Si c'est une note, on se défausse
            if(sous_enfants.toElement().tagName() == "text:note-citation"){
                //On ne fait rien, les notes de bas de page ne sont pas encore gérées.
            }
            else{
                //Quoi que ce soit, ce n'est pas un <p>, on le rebalance à la fonction
                traite_span(format, curseur, e);
            }
        }

        //Type d'élément
        if(enfants.isElement()){
            QDomElement elem = enfants.toElement();

            if(elem.tagName() == "text:line-break"){
                curseur.insertText(QString(QChar::LineSeparator));
            }
            else if(elem.tagName() == "text:span"){
                traite_span(format, curseur, e, puces, tableau);
            }
            else if(elem.tagName() == "draw:frame"){

            }
            else if(elem.tagName() == "text:s"){
                curseur.insertText(QString(" "));
            }
            else if(elem.tagName() == "text:a"){
                traite_lien(curseur, elem, format);
            }
            else if(elem.tagName() == "text:tab"){
                curseur.insertText(QString("    "));
            }
            else{
                instance_erreur.Erreur_msg(tr("ODT : type de <span> non détecté"), QMessageBox::Ignore);
            }
        }
        else if(enfants.isText()){
            //On gére le texte
            QStringList contenu = enfants.nodeValue().split("\n");
            for(int i=0; i<contenu.size(); i++){
                if(puces){
                    //On récupère le style par défaut
                    QTextDocument *temp = new QTextDocument;
                    QTextCursor curseur2(temp);
                    curseur2.insertText(contenu.at(i), format);
                    contenu_puce.append(nettoye_code(temp->toHtml()));
                    delete temp;
                }
                else if(tableau){
                    QTextDocument *temp = new QTextDocument;
                    QTextCursor curseur2(temp);
                    curseur2.insertText(contenu.at(i), format);
                    case_tableau.append(nettoye_code(temp->toHtml()));
                }
                else{
                    curseur.insertText(contenu.at(i), format);
                }
            }
        }
        enfants = enfants.nextSibling();
    }
    return true;
}

bool OpenDocument::traite_lien(QTextCursor &curseur, QDomElement e, QTextCharFormat format){

    ErrorManager instance_erreur;
    if(e.tagName() != "text:a"){
        instance_erreur.Erreur_msg(tr("ODT : lien invalide: skipping!"), QMessageBox::Ignore);
        return false; //On se casse
    }

    QDomNode enfants = e.firstChild();

    //On récupère le lien
    format.setAnchor(true);
    format.setAnchorHref(e.attribute("xlink:href"));
    format.setForeground(QBrush(QColor("#4920FF")));
    format.setToolTip(e.attribute("xlink:href"));

    curseur.insertText(enfants.nodeValue(), format);

    return true;
}

bool OpenDocument::traite_image(QTextCursor &curseur, QDomElement e, QString nom){
    ErrorManager instance_erreur;
    QString file = "";

    if(e.tagName() != "draw:image"){
        instance_erreur.Erreur_msg(tr("ODT : Mauvais paramètre envoyé : fonction de lecture d'image"), QMessageBox::Ignore);
        return false;
    }

    //On détecte si elle est en local ou en HTTP
    if(e.attribute("xlink:href").contains("http")){//Internet
        //On va piocher l'image sur Internet

        //On nettoye l'URL
        QString url_temp = e.attribute("xlink:href");
        if(url_temp.indexOf("?", 0) > 0){
            //Il y en a -> on vire tout ce qui est après
            int pos_depart = url_temp.indexOf("?", 0); int fin = url_temp.size();
            url_temp = url_temp.remove(pos_depart, fin);
        }
        QUrl url_image(url_temp);
        if(!url_image.isValid()){
            instance_erreur.Erreur_msg(tr("ODT : Erreur lors du téléchargement d'éléments distants"), QMessageBox::Ignore);
            return false;
        }

        QNetworkAccessManager nw_manager;
        QNetworkRequest request(url_image);
        QNetworkReply *reponse = nw_manager.get(request);
        QEventLoop eventLoop;
        QObject::connect(reponse, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();
        QImage image;
        image.loadFromData(reponse->readAll());
        QString ext = url_image.toString();
        ext = ext.remove(0, (ext.size()-4));
        qsrand(QDateTime::currentDateTime ().toTime_t ());
        file = QDir::tempPath()+QDir::separator()+QString::number(qrand())+".png";
        if(!image.isNull()){
            if(!image.save(file, "PNG")){
                //!sauvegarde
                instance_erreur.Erreur_msg(tr("ODT : Sauvegarde échouée de l'image téléchargée"), QMessageBox::Ignore);
                return false;
            }
        }
        else{
            //Image nulle
            instance_erreur.Erreur_msg(tr("ODT : Téléchargement échoué d'une image"), QMessageBox::Ignore);
            return false;
        }

        delete reponse;
    }
    else if(e.attribute("xlink:href").contains("Pictures")){//Local
        //On extrait le fichier
        QZipReader image(nom_odt, QIODevice::ReadOnly);
        Outils instance_outils;
        QString clean_fichier = e.attribute("xlink:href");
        clean_fichier = clean_fichier.split("/").last();
        image.extractOne(QDir::tempPath(), clean_fichier);
        image.close();

        //On sélectionne le fichier
        file = QDir::tempPath()+QDir::separator()+clean_fichier;

    }
    else{ //C'est pas normal ça : on se casse
        instance_erreur.Erreur_msg(tr("ODT : source d'image non trouvée"), QMessageBox::Ignore);
        return false;
    }


    //On affiche l'image
    QTextImageFormat format_image;
    format_image = cree_image_format(nom);
    format_image.setName(file);
    //On récupère d'éventuelles spécifications de taille
    QDomNode enfants = e.parentNode();
    e = enfants.toElement();
    //Width de l'image
    if(e.hasAttribute("svg:width")){
        QString width = e.attribute("svg:width");
        double value = width.left(width.length() - 2).toDouble();
        format_image.setWidth(value*28.3465058);
    }
    //Height de l'image
    if(e.hasAttribute("svg:height")){
        QString height = e.attribute("svg:height");
        double value = height.left(height.length() - 2).toDouble();
        format_image.setHeight(value*28.34650058);
    }

    QTextBlockFormat format_block_image;
    format_block_image = cree_bloc_format2(nom);
    curseur.insertBlock();
    curseur.movePosition(QTextCursor::PreviousBlock);
    curseur.setBlockFormat(format_block_image);
    curseur.insertImage(format_image);
    curseur.movePosition(QTextCursor::End);

    return true;
}

//Nettoyage du code pour le contenu des puces
QString OpenDocument::nettoye_code(QString code){
    QString resultat;

    //Soit <span>, soit <p>
    if(code.contains("<span")){
        //On supprime le début du code
        int depart = code.indexOf("<span", 0);
        resultat = code.remove(0, depart);

        //Puis la fin
        depart = code.indexOf("</span>", 0);
        int fin = code.size();
        resultat = resultat.remove((depart+7), fin);
    }
    else if(code.contains("<p")){
        //On supprime le début du code
        int depart = code.indexOf("<p", 0);
        resultat = code.remove(0, depart);

        //Puis la fin
        depart = code.indexOf("</p>", 0);
        int fin = code.size();
        resultat = resultat.remove((depart+4), fin);
    }
    else{
        ErrorManager instance_erreur;
        instance_erreur.Erreur_msg(tr("ODT : erreur de troncation de code dans les listes à puces : élément non déterminé"), QMessageBox::Ignore);
        return "N/A";
    }

    return resultat;
}
