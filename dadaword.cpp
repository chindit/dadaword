/*
  Développeur : David Lumaye (littletiger58@gmail.com)
  Date : 16/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#include "dadaword.h"

//Constructeur
DadaWord::DadaWord(QWidget *parent)
    : QMainWindow(parent)
{
    //Constructeur vide pour permettre la création d'instances DadaWord
    //On initialise tout de même type_liste par sécurité
    type_liste = "";

    //Initialisation des dicos
    Outils instance;
    dictPath = "/usr/share/hunspell/"+instance.lire_config("dico").toString();
    if(dictPath == "/usr/share/hunspell/"){//Si la config n'existe pas (jamais visité la config), on la met par défaut
        dictPath = "/usr/share/hunspell/fr_FR";
    }

    //On regarde si le dossier de config existe
    QString dossier = QDir::homePath()+QDir::separator()+".dadaword";
    QDir dir_dossier(dossier);
    if(!dir_dossier.exists()){
        if(!dir_dossier.mkdir(dossier)){
            Erreur instance;
            instance.Erreur_msg(tr("Impossible de créer le dossier de configuration"), QMessageBox::Information);
        }
        else{
            //On crée un fichier vide
            QFile dico_perso(dossier+"/perso.dic");
            if(dico_perso.open(QFile::WriteOnly)) {
                dico_perso.close();
            }
            else{
                Erreur instance;
                instance.Erreur_msg(tr("Impossible de créer le dictionnaire personnel"), QMessageBox::Information);
            }
        }
    }
}

//Destructeur
DadaWord::~DadaWord()
{
    //On vérifie si le fichier n'est pas déjà ouvert
    QList<QMdiSubWindow *> liste_fichiers = zone_centrale->findChildren<QMdiSubWindow *>();
    QTextDocument *temp_document = new QTextDocument;
    for(int i=0; i<liste_fichiers.size(); i++){
        QMdiSubWindow *temp_boucle = liste_fichiers.at(i);
        QTextEdit *edit_temp = temp_boucle->findChild<QTextEdit *>();
        temp_document = edit_temp->document();
        if(temp_document->isModified()){
            alerte_enregistrement(temp_boucle);
        }
    }

    //On fait les delete
    delete zone_centrale;
    delete enregistrer;
    delete puces;
    delete barre_tableau;
    delete titre_doc;
    delete choix_police;
    delete taille_police;
    delete gras;
    delete italique;
    delete souligne;
    delete couleur_actuelle;
    delete edition_redo;
    delete edition_undo;
    delete incremente_puce_bouton;
    delete desincremente_puce_bouton;
    delete lecture_seule;
    delete to_text;
    delete barre_etat;
    delete affichage_default;
    delete affichage_puces;
    delete barre_standard;
    delete affichage_tableau;
    delete full_screen;
    delete barre_recherche;
    delete affichage_recherche;
    delete barre_orthographe;

}

//Ancien constucteur : génère l'interface utilisateur : À n'appeller que dans le main();
void DadaWord::cree_iu(){
    //UTF-8
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    Outils instance_outils;
    //Par précaution, on remplit à "HIGH" la config si ce n'est pas encore fait
    if(instance_outils.lire_config("alertes").toInt() != LOW && instance_outils.lire_config("alertes").toInt() != MEDIUM && instance_outils.lire_config("alertes").toInt() != HIGH){
        instance_outils.enregistre_config("alertes", HIGH);
    }

    //On regarde s'il y a une taille par défaut définie sinon on la définit (pour éviter un avertissement lors de l'exécution
    if(instance_outils.lire_config("taille").toInt() < 1){
        instance_outils.enregistre_config("taille", 12);
    }


    //Préparation des variables globales
    //modification = false;
    titre_doc = new QString;
    couleur_actuelle = new QColor(Qt::transparent);

    //Création de la barre d'état
    barre_etat = statusBar();
    barre_etat->showMessage(tr("Prêt"), 2500);
    //Initialisation des boutons
    //Langue du document en cours
    status_langue = new QPushButton(statusBar());
    QString nom_dico = dictPath.split("/").last();
    status_langue->setText(nom_dico);
    status_langue->setToolTip(tr("Langue pour le document actuel"));
    status_langue->setFlat(true);
    connect(status_langue, SIGNAL(clicked()), this, SLOT(orth_langue()));
    statusBar()->addPermanentWidget(status_langue);

    //Surécriture
    status_surecriture = new QPushButton(tr("INS"), statusBar());
    //Bouton aplati
    status_surecriture->setFlat(true);
    //Ajout à la barre des tâches
    statusBar()->addPermanentWidget(status_surecriture);
    //Connexion
    connect(status_surecriture, SIGNAL(clicked()), this, SLOT(mode_surecriture()));
    //Enregistrement
    status_is_modified = new QPushButton(statusBar());
    status_is_modified->setIcon(QIcon(":/menus/images/filesave.png"));
    status_is_modified->setEnabled(false);
    //Style du bouton pour qu'on ne le remarque pas
    status_is_modified->setFlat(true);
    statusBar()->addPermanentWidget(status_is_modified);
    //On connecte le QLabel avec le slot d'enregistrement
    connect(status_is_modified, SIGNAL(clicked()), this, SLOT(enregistrement()));

    //Création des menus
    create_menus();

    this->setWindowIcon(QIcon(":/programme/images/dadaword.gif"));

    //Création du MDI
    zone_centrale = new QMdiArea;
    QTextEdit *document = new QTextEdit;
    document->installEventFilter(this);
    QTextDocument *doc_principal = new QTextDocument;
    document->setDocument(doc_principal);
    doc_principal->setModified(false);
    QMdiSubWindow *zone_document;
    //Connection du QTextEdit à la correction
    document->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(document, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(affiche_menu_perso(const QPoint &)));
    connect(document, SIGNAL(cursorPositionChanged()), this, SLOT(curseur_change()));

    //Gestion des onglets Word ou non
    if(instance_outils.lire_config("word").toBool()){
        zone_document = new QMdiSubWindow;
        //On delete en plus de fermer
        zone_document->setAttribute(Qt::WA_DeleteOnClose);
        zone_centrale->addSubWindow(zone_document);

        QPrinter printer(QPrinter::HighResolution);
        printer.setPaperSize(QPrinter::A4);
        document->setMaximumHeight((printer.paperSize(QPrinter::Point)).toSize().rheight()+MARGIN_WORD);
        document->setMaximumWidth((printer.paperSize(QPrinter::Point)).toSize().rwidth()+MARGIN_WORD+TAMPON_WORD);
        //document_onglet->setMaximumSize((printer.paperSize(QPrinter::Point)).toSize());
        document->setMinimumWidth((printer.paperSize(QPrinter::Point)).toSize().rwidth()+MARGIN_WORD+TAMPON_WORD);//Addition d'un tampon parce qu'on est pas tout à fait juste
        QHBoxLayout *layout_horizontal = new QHBoxLayout;
        QWidget *widget = new QWidget; //Sert juste pour le layout
        layout_horizontal->addWidget(document, 0, Qt::AlignHCenter);
        document->setFrameShape(QFrame::NoFrame);
        widget->setLayout(layout_horizontal);
        document->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        zone_document->setWidget(widget);
        QTextFrame *tf = doc_principal->rootFrame();
        QTextFrameFormat tff = tf->frameFormat();
        tff.setMargin(MARGIN_WORD);
        tf->setFrameFormat(tff);
        doc_principal->setModified(false);

    }
    else{
        zone_document = zone_centrale->addSubWindow(document);
    }

    //Quelle que soit la config, on affiche ça :
    zone_document->setWindowTitle(tr("Nouveau document"));
    zone_document->setAccessibleName(tr("Nouveau document"));

    //Vue en onglets
    if(!instance_outils.lire_config("onglets").toBool()){
        zone_centrale->setViewMode(QMdiArea::TabbedView);
        //Autorisation de déplacer les onglets
        QTabBar *tab_bar_area = zone_centrale->findChild<QTabBar*>();
        tab_bar_area->setMovable(true);
        tab_bar_area->setTabsClosable(true);
        connect(tab_bar_area, SIGNAL(tabCloseRequested(int)), this, SLOT(close_tab_button(int)));
    }

    //Déclaration de la police par défaut :
    QFont police("ubuntu");
    document->setFont(police);

    //Placement du widget comme zone centrale
    this->setCentralWidget(zone_centrale);

    //connections globales
    connect(doc_principal, SIGNAL(contentsChanged()), this, SLOT(indicateur_modifications()));
    connect(zone_centrale, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(changement_focus(QMdiSubWindow*)));

    //Maintenant, on récupère les arguments (il faut que l'interface soit créée avant de pouvoir les ouvrir
    //Récupération des arguments au lancement
#if defined(Q_WS_MAC)
    //On récupère les arguments selon MAC
#else //Arguments Linux & Windows
    QStringList liste = QCoreApplication::arguments();
    if(liste.count() > 1){
        QFile fichier(liste.at(1));
        if(fichier.exists()){
            ouvrir_fichier(liste.at(1));
        }
    }
#endif
}

//A propos du programme
void DadaWord::affiche_about(){

    QString string_about = ("<h2>A propos de DadaWord</h2><br><b>Dévoloppé par</b> : David Lumaye<br><b>Version</b> : ")+QString::fromAscii(VERSION)+tr("<br><b>Courriel</b>:<a href='mailto:littletiger58@gmail.com'>littletiger58@gmail.com</a><br><b>Distribué sous license</b> : <a href='http://www.gnu.org/licenses/lgpl-3.0.fr.html'>LGPL 3</a>");
    QMessageBox::about(this, tr("À propos de DadaWord"), string_about);
    return;
}

//Changement de police
void DadaWord::change_police(QFont nouvelle_police){
    int police = find_edit()->fontWeight();
    bool police_italique = find_edit()->fontItalic();
    int taille_police = find_edit()->fontPointSize();
    bool police_souligne = find_edit()->fontUnderline();
    find_edit()->setCurrentFont(nouvelle_police);
    find_edit()->setFontPointSize(taille_police);
    if(police == QFont::Bold){
        graisse_police(true);
    }
    find_edit()->setFontItalic(police_italique);
    find_edit()->setFontUnderline(police_souligne);
    find_edit()->setFocus();
    return;
}

//Mise en gras
void DadaWord::graisse_police(bool etat){
    if(etat){
        QTextCursor curseur = find_edit()->textCursor();
        if(curseur.hasSelection()){
            gras->setChecked(false);
        }
        find_edit()->setFontWeight(QFont::Bold);
    }
    else{
        find_edit()->setFontWeight(QFont::Normal);
    }
    return;
}

//Mise en italique
void DadaWord::italique_police(bool etat){
    QTextCursor curseur = find_edit()->textCursor();
    if(curseur.hasSelection()){
        italique->setChecked(false);
    }
    find_edit()->setFontItalic(etat);
    return;
}

//Soulignement
void DadaWord::souligne_police(bool etat){
    QTextCursor curseur = find_edit()->textCursor();
    if(curseur.hasSelection()){
        souligne->setChecked(false);
    }
    find_edit()->setFontUnderline(etat);
    return;
}

//Modification de la taille
void DadaWord::change_taille(int taille){
    find_edit()->setFontPointSize(taille);
    find_edit()->setFocus();
    return;
}

//Aperçu avant impression
void DadaWord::apercu_avant_impression(){
    QPrinter printer;
    printer.setPaperSize(QPrinter::A4);
    Outils instance;
    if(instance.lire_config("word").toBool()){
        printer.setPageMargins(0, 0, 0, 0, QPrinter::Pica);
    }
    printer.setOrientation(QPrinter::Portrait);
    QPrintPreviewDialog *pd = new QPrintPreviewDialog(&printer);
    connect(pd,SIGNAL(paintRequested(QPrinter*)),this,SLOT(print(QPrinter*)));
    pd->exec();
    return;
}

//Impression (sert juste pour l'aperçu)
void DadaWord::print(QPrinter *p)
{
    find_edit()->print(p);
    return;
}

//Impression (réelle)
void DadaWord::imprimer(){
    QPrinter printer(QPrinter::HighResolution);
    Outils instance;
    if(instance.lire_config("word").toBool()){
        printer.setPageMargins(0, 0, 0, 0, QPrinter::Pica);
    }
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    //On imprime que la sélection
    if(find_edit()->textCursor().hasSelection()){
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    }
    dlg->setWindowTitle(tr("Imprimer le document"));
    if(dlg->exec() == QDialog::Accepted){
        find_edit()->print(&printer);
    }
    delete dlg;
    return;
}

//Enregistrement
void DadaWord::enregistrement(QMdiSubWindow* fenetre_active, bool saveas){

    //---------------------------------------------------
    //Variables globales pour la fonction
    //---------------------------------------------------
    QString nom_fichier = "";
    QString contenu_fichier;
    QTextEdit *edit_temp;
    QMdiSubWindow *fenetre_temp;
    QTextDocument *temp_document;
    QStringList extensions_style;
    extensions_style << "ddw" << "htm" << "html" << "odt" << "ddz";
    Outils instance_outils;
    Erreur instance_erreur;

    //----------------------------------------------
    //Récupération du QTextEdit à enregistrer
    //----------------------------------------------
    if(fenetre_active == 0){ //Pas de QMdiSubWindow -> fenêtre active!
        edit_temp = find_edit();
        fenetre_temp = find_onglet();
        if(edit_temp == 0){
            //Si on enregistre et qu'il n'y a pas de fenêtre (déclenchement du QTimer), on quitte
            return;
        }
    }
    else{//On n'est pas dans la fenêtre active
        edit_temp = fenetre_active->findChild<QTextEdit *>();
        fenetre_temp = fenetre_active;
    }

    //-------------------------------------
    //Récupération du nom du fichier
    //-------------------------------------
    QString nom_fenetre = fenetre_temp->accessibleDescription();
    if(nom_fenetre.isEmpty() || nom_fenetre.isNull() || nom_fenetre.contains(tr("Nouveau document")) || saveas){
        //POUR AUTORISER L'ODT, SUFFIT DE RAJOUTER CECI : ;;Documents ODT (*.odt)
        //MALHEUREUSEMENT, ÇA MARCHE PAS (SINON JE L'AURAIS DÉJÀ FAIT ;-) )
        nom_fichier = QFileDialog::getSaveFileName(this, "Enregistrer un fichier", QDir::homePath(), "Documents DadaWord (*.ddz *.ddw);;Documents texte (*.txt);;Documents HTML (*.html, *.htm);;Documents divers (*.*)");
        if(nom_fichier.isNull() || nom_fichier.isEmpty()){
            //On enregistre pas, on fait comme si de rien n'était
            return;
        }

        //------------------------------------------------
        //Traitement du fichier selon les extentions
        //------------------------------------------------
        QFileInfo nom_info(nom_fichier);
        if(nom_info.completeSuffix().isEmpty() || nom_info.completeSuffix().isNull()){
            nom_fichier.append(".ddz"); //Si pas extention -> DDZ!
            nom_info.setFile(nom_fichier);
        }

        //---------------------
        //Extentions de texte
        //---------------------
        if(!extensions_style.contains(nom_info.completeSuffix()) && edit_temp->acceptRichText()){
                //-------------------------
                //Avertissement de format
                //-------------------------
            if(instance_outils.lire_config("alertes").toInt() != LOW){
                int reponse = QMessageBox::question(this, tr("Format texte"), tr("Attention: le format que vous avez choisi ne peut sauvegarder les informations de style présentes dans ce document.\n Voulez-vous continuer?"), QMessageBox::Yes | QMessageBox::No);
                if(reponse == QMessageBox::Yes){
                    //Si l'utilisateur est OK, on remplace le contenu de "contenu_fichier" par du plain text
                    contenu_fichier = edit_temp->toPlainText();
                }
                else if(reponse == QMessageBox::No){
                    //Si l'utilisateur ne veut pas de TXT, on annule tout
                    return;
                }
                else{
                    instance_erreur.Erreur_msg(tr("Enregistrement : retour de QMessageBox inopiné -> exit"), QMessageBox::Critical);
                    return;
                }
            }//IF : gestion des alertes
            else{
                contenu_fichier = edit_temp->toPlainText();
            }
            //On désactive le richeText (pour ne pas qu'il y aie de bug si quelqu'un clique sur "Texte seul"
            edit_temp->setAcceptRichText(false);
        }//IF : mode Texte

        //--------------------------------------
        //Extentions de style (export en HTML)
        //--------------------------------------
        else if(nom_info.completeSuffix() == "odt"){
            //ODT, et que ça saute!!!
            //Mais avant, faut indiquer le nom du fichier sinon ça bugue
            fenetre_temp->setAccessibleDescription(nom_fichier);
            export_odt();
        }
        else{
            contenu_fichier = edit_temp->toHtml();
        }

        //------------------------------------------------
        //Opérations générales appliquées à la fenêtre
        //------------------------------------------------
        fenetre_temp->setAccessibleDescription(nom_fichier);
        fenetre_temp->setWindowTitle(nom_fichier.split("/").last());

        //Activation de la coloration syntaxique
        if(fenetre_temp->windowTitle().contains(".htm") || fenetre_temp->windowTitle().contains(".xml")){
            colore_html->setEnabled(true);
        }
        else{
            colore_html->setEnabled(false);
        }
    }//IF : nom de fenêtre inexistant

    //-------------------------------------------------
    //Opérations si le nom de fichier existait déjà
    //-------------------------------------------------
    else{
        nom_fichier = fenetre_temp->accessibleDescription();
        if(extensions_style.contains(QFileInfo(nom_fichier).completeSuffix()) && !nom_fichier.endsWith(".odt", Qt::CaseInsensitive)){ //Tout le style sauf l'ODT
            contenu_fichier = edit_temp->toHtml();
        }
        else if(nom_fichier.endsWith(".odt", Qt::CaseInsensitive)){
            //On exporte en ODT
            export_odt();
            //On se casse parce qu'on a pas besoin d'écrire
            return;
        }
        else{ //Fichiers textes
            contenu_fichier = edit_temp->toPlainText();
        }
    }

    //-----------------------------------------------------
    //Écriture du contenu selon les différentes extentions
    //-----------------------------------------------------
    if(nom_fichier.contains(".ddz")){
        DDZ instance_ddz;
        if(!instance_ddz.enregistre(nom_fichier, contenu_fichier)){
            instance_erreur.Erreur_msg(tr("Impossible d'enregistrer au format DDZ"), QMessageBox::Warning);
        }
    }
    else{ //Pas de DDZ, fichier normal
        QFile file(nom_fichier);
        if (file.open(QFile::WriteOnly)) {
            QTextStream out(&file);
            out << contenu_fichier;
            file.close();
        }
    }

    //----------------------------------------------
    //Opérations de post-enregistrement
    //----------------------------------------------
    temp_document = edit_temp->document();
    temp_document->setModified(false);
    enregistrer->setEnabled(false);
    status_is_modified->setEnabled(false);

    return;
}

//Ouverture d'un fichier
void DadaWord::ouvrir_fichier(const QString &fichier){
    //Variables globales
    QString nom_fichier;
    QString contenu;
    bool style = false;
    bool texte = false;


    //----------------------------------------------------------------
    //Récupération du nom du fichier et actions de pré-ouverture
    //----------------------------------------------------------------
    //Récupération du nom de fichier
    QFile file(fichier);
    if(!file.exists()){
        nom_fichier = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QDir::homePath(), "Tous documents (*.*);;Documents DadaWord (*.ddz);;Documents HTML (*.html *.htm);;Documents texte (*.txt);;Documents ODT (*.odt)");
        //On vérifie que l'utilisateur n'a pas fait "Cancel"
        if(nom_fichier.isNull() || nom_fichier.isEmpty()){
            Erreur instance_erreur;
            instance_erreur.Erreur_msg(tr("Impossible d'ouvrir le fichier, aucun nom n'a été donné"), QMessageBox::Ignore);
            return;
        }
        //S'il n'a pas d'extention, on la rajoute
        QString extention = nom_fichier.section('.', -1);
        if(extention.isNull() || extention.isEmpty()){
            nom_fichier.append(".ddz");
        }
        //On remplace le QFile
        file.setFileName(nom_fichier);
    }
    else{
        //Au cas où le fichier est bon, il faut copier le titre dans "nom_fichier" pour qu'il soit utilisable
        nom_fichier = fichier;
    }

    QString titre = nom_fichier.split("/").last();
    Outils instance_outils;

    //On vérifie si le fichier n'est pas déjà ouvert
    QList<QMdiSubWindow *> liste_fichiers = zone_centrale->findChildren<QMdiSubWindow *>();
    for(int i=0; i<liste_fichiers.size(); i++){
        QMdiSubWindow *temp_boucle = liste_fichiers.at(i);
        if(temp_boucle->accessibleName() == titre){
            break; //On sort de la boucle
            if(instance_outils.lire_config("alertes").toInt() != LOW){
                QMessageBox::information(this, tr("Fichier déjà ouvert"), tr("Le fichier que vous tentez d'ouvrir est déjà ouvert.\n Si ce n'est pas le cas, deux fichiers portent un nom identique. Veuillez les renommer."));
            }
            return;
        }
    }

    //----------------------------------------------------
    // Récupération du contenu du fichier à ouvrir
    // ---------------------------------------------------

    //Gestion de l'ODT
    if(nom_fichier.endsWith(".odt", Qt::CaseInsensitive)){
        //C'est un ODT, on le balance à OpenDocument
        OpenDocument instance_doc;
        contenu = instance_doc.ouvre_odt(nom_fichier);
        style = true; //Fichier contenant du style
        //Erreur de lecture
        if(contenu.isEmpty() || contenu.isNull() || contenu == "NULL"){
            Erreur instance_erreur;
            instance_erreur.Erreur_msg(tr("Une erreur est survenue lors de l'ouverture du fichier ODT. \n Consultez le fichier de LOG pour plus d'informations"), QMessageBox::Critical);
            return;
        }
    }
    //Gestion du DDZ
    else if(nom_fichier.endsWith(".ddz", Qt::CaseInsensitive)){
        DDZ instance_ddz;
        contenu = instance_ddz.ouvre(nom_fichier);
        //Erreur de lecture
        if(contenu.isEmpty() || contenu.isNull() || contenu == "NULL"){
            Erreur instance_erreur;
            instance_erreur.Erreur_msg(tr("Une erreur est survenue lors de l'ouverture du fichier DDZ. \n Consultez le fichier de LOG pour plus d'informations"), QMessageBox::Critical);
            return;
        }
        style = true; //Fichier de style
    }
    //Gestions des fichiers en mode texte simple
    else if(file.open(QFile::ReadOnly)){
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            contenu = contenu + line + "\n";
        }
        file.close();

        //BUG : Fichier vide
        if((contenu.isEmpty() || contenu.isNull()) && !instance_outils.lire_config("fichiers_vides").toBool()){
            Erreur instance_erreur;
            instance_erreur.Erreur_msg(tr("Une erreur s'est produite lors de l'ouverture du fichier : aucun contenu n'a été détecté. \n Êtes-vous sûr que le fichier n'est pas corrompu?"), QMessageBox::Critical);
            return;
        }

        //Fichier avec style (HTML)
        if(nom_fichier.endsWith(".ddw", Qt::CaseInsensitive) || nom_fichier.endsWith(".htm", Qt::CaseInsensitive) || nom_fichier.endsWith(".html", Qt::CaseInsensitive)){
            if(!nom_fichier.endsWith(".ddw", Qt::CaseInsensitive)){
                colore_html->setEnabled(true);
            }
            else{
                colore_html->setEnabled(false);
            }
            style = true;
        }

        //Fichier texte
        else{
            texte = true;
        }
    }//Fin FILE.OPEN
    //Une erreur s'est produite lors de l'ouverture du fichier
    else{
        //Échec de l'ouverture du fichier
        Erreur instance_erreur;
        instance_erreur.Erreur_msg(tr("Le fichier demandé n'a pu être ouvert. \n Veuillez réessayer."), QMessageBox::Critical);
        return;
    }

    //------------------------------------------------------------
    //Actions de post-ouverture
    //Mise à jour des paramètres système
    //------------------------------------------------------------
    QTextDocument *document_actuel = new QTextDocument;

    if(liste_fichiers.size() == 0){ //Pas d'onglets ouverts -> on en ouvre un
        ouvre_onglet(true, titre);
    }
    else{
        document_actuel = find_edit()->document();
        if(document_actuel->isModified() || !document_actuel->isEmpty()){
            ouvre_onglet(true, titre);
        }
        else{
            //Si on ouvre pas un nouvel onglet, on envoie tout de même le signal de changement pour mettre à jour les menus
            changement_focus(find_onglet());
        }
    }
    //On refait un "find_edit()" parce qu'il se peut que celui-ci ait changé vu qu'on a (peut-être) ouvert un nouvel onglet
    document_actuel = find_edit()->document();
    document_actuel->setModified(false);
    find_onglet()->setWindowTitle(titre);
    find_onglet()->setAccessibleName(titre);
    find_onglet()->setAccessibleDescription(nom_fichier);
    //Connexion au slot des récemment ouverts
    instance_outils.enregistre_fichiers(nom_fichier);
    //Configurations WORD
    Outils instance;
    if(instance.lire_config("word").toBool()){
        QTextFrame *tf = find_edit()->document()->rootFrame();
        QTextFrameFormat tff = tf->frameFormat();
        tff.setMargin(MARGIN_WORD);
        tf->setFrameFormat(tff);
        find_edit()->document()->setModified(false);
    }

    //----------------------------------------------
    //Insertion du contenu dans le QTextEdit
    //----------------------------------------------
    if(style){
        //Gestion des fichiers de style
        find_edit()->setAcceptRichText(true);
        find_edit()->insertHtml(contenu);
    }
    else if(texte){
        //Gestion des fichiers de texte
        find_edit()->setText(contenu);
        find_edit()->toPlainText();
        find_edit()->setAcceptRichText(false);
        //On coche le "Mode texte" parce que ce ne peut pas être autre chose pour du TXT
        to_text->setChecked(true);
        to_text->setEnabled(false); //On le désactive, c'est inutile pour du TXT
        //On active la coloration aussi pour le XML
        if(!nom_fichier.endsWith(".xml", Qt::CaseInsensitive)){
            colore_html->setEnabled(true);
        }
    }
    else{
        Erreur instance_erreur;
        instance_erreur.Erreur_msg(tr("Erreur lors de l'ouverture de fichier : il n'a pu être déterminé si le fichier s'ouvrait ou non en mode texte"), QMessageBox::Warning);
        return;
    }

    return;
}

//Export en ODT
void DadaWord::export_odt(){
    //Enregistrement ODT
        QTextDocumentWriter *enregistrement_fichier;
        enregistrement_fichier = new QTextDocumentWriter;
        enregistrement_fichier->setFormat("odf");
        QFile *test_nom = new QFile(find_onglet()->accessibleDescription());
        QString fichier = find_onglet()->accessibleDescription();
        if(!test_nom->isWritable()){
            Erreur instance;
            instance.Erreur_msg(tr("ODT : Emplacement non inscriptible"), QMessageBox::Ignore);
            fichier = QFileDialog::getSaveFileName(this, tr("Enregistrer un fichier"), QDir::homePath(), tr("Documents textes (*.odt)"));
        }
        else{
            fichier = find_onglet()->accessibleDescription();
        }
        enregistrement_fichier->setFileName(fichier);
        //On exporte l'onglet actuel :
        QTextEdit *current_text_edit = find_edit();
        QTextDocument *current_text_document = current_text_edit->document();
        enregistrement_fichier->write(current_text_document);
        return;
    /*if(nom_fichier == "null" && find_edit()->accessibleDescription().isEmpty()){
        Erreur instance;
        instance.Erreur_msg(tr("Nom de fichier erroné -> annulation de l'enregistrement"), QMessageBox::Information);
        return;
    }
    else if(nom_fichier == "null"){
        nom_fichier = find_edit()->accessibleDescription();
    }
    else{
        //On ne fait rien.
    }
    QTextDocumentWriter enregistrement_fichier;
    enregistrement_fichier.setFormat("odf");
    enregistrement_fichier.setFileName(nom_fichier.toAscii());
    enregistrement_fichier.write(find_edit()->document());
*/
    //QMessageBox::information(this, "o", nom_fichier);
   // return;
}

//Export en PDF
void DadaWord::export_pdf(){
    QString nom_fichier = QFileDialog::getSaveFileName(this, tr("Exporter en PDF"), QDir::homePath(), tr("Documents PDF (*.pdf)"));
    if(!nom_fichier.isEmpty()){
        if(QFileInfo(nom_fichier).suffix().isEmpty()){
            nom_fichier.append(".pdf");
        }
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        if(!printer.isValid()){
            Erreur instance_erreur;
            instance_erreur.Erreur_msg(tr("Aucune imprimante PDF n'a été trouvée sur votre système.\n Annulation de l'impression"), QMessageBox::Warning);
            return;
        }
        printer.setOutputFileName(nom_fichier);
        find_edit()->document()->print(&printer);
    }
    return;
}

//Création des listes à puces
void DadaWord::create_liste_puce(const int ordonne){
    QTextCursor curseur = find_edit()->textCursor();
    QTextListFormat liste_puce;
    QTextList *liste_actuelle = curseur.currentList();
    if(liste_actuelle == 0){
        if(ordonne == 1){
            fen_puces instance_puces;
            //On récupère le choix dans un QSTring
            QString choix = instance_puces.get_radio();
            if(choix == "QTextListFormat::ListDecimal"){
                liste_puce.setStyle(QTextListFormat::ListDecimal);
            }
            else if(choix == "QTextListFormat::ListLowerAlpha"){
                liste_puce.setStyle(QTextListFormat::ListLowerAlpha);
            }
            else if(choix == "QTextListFormat::ListUpperAlpha"){
                liste_puce.setStyle(QTextListFormat::ListUpperAlpha);
            }
            else if(choix == "QTextListFormat::ListLowerRoman"){
                liste_puce.setStyle(QTextListFormat::ListLowerRoman);
            }
            else if(choix == "QTextListFormat::ListUpperRoman"){
                liste_puce.setStyle(QTextListFormat::ListUpperRoman);
            }
            else{
                Erreur instance;
                instance.Erreur_msg(tr("Erreur lors de la transmission du type de liste à DadaWord::create_liste_puce"), QMessageBox::Information);
                return;
            }
            //Si on est ici, c'est qu'il n'y a pas eu de bugs.  Donc, on enregistre le type de liste dans "type_liste"
            type_liste = choix;
        }
        else{
            liste_puce.setStyle(QTextListFormat::ListDisc);
            //On oublie pas de mettre à jour type_liste pour ne pas avoir de problèmes
            type_liste = "";
        }
        curseur.beginEditBlock();
        curseur.createList(liste_puce);
        curseur.endEditBlock();
        //On affiche la ToolBar des listes à puces
        puces->show();
        affichage_puces->setChecked(true);
        incremente_puce_bouton->setEnabled(true);
        desincremente_puce_bouton->setEnabled(true);
    }
    else{
        Outils instance_outils;
        if(instance_outils.lire_config("alertes").toInt() == HIGH){
            QMessageBox::information(this, tr("Erreur de liste"), QString::fromUtf8("Vous êtes déjà dans une liste à puces; vous ne pouvez donc en insérer une autre."));
        }
    }
    return;
}

//Filtre d'événements clavier
bool DadaWord::eventFilter(QObject *obj, QEvent *event){

    if(event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return){
            QTextCursor curseur = find_edit()->textCursor();
            curseur.movePosition(QTextCursor::Left);
            curseur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            QString contenu_selection = curseur.selectedText();
            if(curseur.currentList() != 0){
                if(!contenu_selection.isEmpty()){
                    QChar char_test = contenu_selection.at(0);
                    if(char_test == QChar(8233)){
                        desincremente_puce();
                    }
                }
            }//Fin CurrentList
        }//Fin du "if" de la QKey
        if(keyEvent->key() == Qt::Key_Tab){
            QTextCursor curseur = find_edit()->textCursor();
            curseur.movePosition(QTextCursor::Left);
            curseur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            QString contenu_selection = curseur.selectedText();
            if(curseur.currentList() != 0){
                if(!contenu_selection.isEmpty()){
                    QChar char_test = contenu_selection.at(0);
                    if(char_test == QChar(8233)){
                        incremente_puce();
                    }
                }
            }//Fin CurrentList
            else{
                puces->hide();
                affichage_puces->setChecked(false);
                incremente_puce_bouton->setEnabled(false);
                desincremente_puce_bouton->setEnabled(false);
            }
        }//Fin du "if"
        if(keyEvent->key() == Qt::Key_Escape){
            if(this->isFullScreen()){
                //Si c'est du plein écran et que l'utilisateur appuye sur "Esc", on se casse
                full_screen->setChecked(false);
                //Et on appelle la fonction
                make_full_screen();
            }
        }//Fin du "if" de "Escape"
        if(keyEvent->key() == Qt::Key_Space){
            Outils instance_outils;
            if(instance_outils.lire_config("orthographe").toBool()){
                QTextCursor temp = find_edit()->textCursor();
                temp.movePosition(QTextCursor::PreviousWord);
                if(temp.movePosition(QTextCursor::PreviousWord)){
                    temp.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
                    if(temp.hasSelection() && temp.selectedText().at(temp.selectedText().size()-1).isLetter()){
                        QString userDict= QDir::homePath() + "/.config/libreoffice/3/user/wordbook/standard.dic";
                        if(!QFile::exists(userDict)){
                            userDict = QDir::homePath() + ".dadaword/perso.dic";
                        }
                        SpellChecker instance_orthographe(dictPath, userDict);
                        if(!temp.selectedText().isEmpty() && !instance_orthographe.spell(temp.selectedText()) && !list_skip.contains(temp.selectedText())){
                            // highlight the unknown word
                            QTextCharFormat marquage_erreurs;
                            QColor couleur(Qt::red);
                            marquage_erreurs.setUnderlineColor(couleur);
                            marquage_erreurs.setUnderlineStyle(QTextCharFormat::WaveUnderline);

                            //Sélection de texte (pour le surlignage)
                            QTextEdit::ExtraSelection es;
                            es.cursor = temp;
                            es.format = marquage_erreurs;

                            liste_erreurs << es;
                            find_edit()->setExtraSelections(liste_erreurs);
                        } //IF : s'il y a une faute
                    }//IF : s'il y a sélection et qu'elle est valide
                }//IF : s'il y a un mot précédent
            }//IF : si la correction est activée
        }//IF : activation de la touche "Espace"
    }
    return QWidget::eventFilter(obj, event);
}

//Désincrémentation dans les listes à puces
void DadaWord::desincremente_puce(){
    QTextCursor curseur = find_edit()->textCursor();
    QTextListFormat liste_puce;
    QTextList *liste_actuelle = curseur.currentList();
    liste_puce = liste_actuelle->format();
    curseur.beginEditBlock();
    curseur.movePosition(QTextCursor::NextBlock);

    //Type de liste
    if(type_liste == "QTextListFormat::ListDecimal"){
        liste_puce.setStyle(QTextListFormat::ListDecimal);
    }
    else if(type_liste == "QTextListFormat::ListLowerAlpha"){
        liste_puce.setStyle(QTextListFormat::ListLowerAlpha);
    }
    else if(type_liste == "QTextListFormat::ListUpperAlpha"){
        liste_puce.setStyle(QTextListFormat::ListUpperAlpha);
    }
    else if(type_liste == "QTextListFormat::ListLowerRoman"){
        liste_puce.setStyle(QTextListFormat::ListLowerRoman);
    }
    else if(type_liste == "QTextListFormat::ListUpperRoman"){
        liste_puce.setStyle(QTextListFormat::ListUpperRoman);
    }
    else{
        liste_puce.setStyle(QTextListFormat::ListDisc);
    }

    if(liste_puce.indent() > 1){
        liste_puce.setIndent(liste_puce.indent() -1);
        curseur.createList(liste_puce);
        curseur.endEditBlock();
    }
    else{
        curseur.endEditBlock();
    }
    return;
}

//Incrémentation dans les listes à puces
void DadaWord::incremente_puce(){
    QTextCursor curseur = find_edit()->textCursor();
    QTextListFormat liste_puce;
    QTextList *liste_actuelle = curseur.currentList();
    liste_puce = liste_actuelle->format();
    curseur.beginEditBlock();
    curseur.movePosition(QTextCursor::NextBlock);

    if(type_liste == "QTextListFormat::ListDecimal"){
        liste_puce.setStyle(QTextListFormat::ListDecimal);
    }
    else if(type_liste == "QTextListFormat::ListLowerAlpha"){
        liste_puce.setStyle(QTextListFormat::ListLowerAlpha);
    }
    else if(type_liste == "QTextListFormat::ListUpperAlpha"){
        liste_puce.setStyle(QTextListFormat::ListUpperAlpha);
    }
    else if(type_liste == "QTextListFormat::ListLowerRoman"){
        liste_puce.setStyle(QTextListFormat::ListLowerRoman);
    }
    else if(type_liste == "QTextListFormat::ListUpperRoman"){
        liste_puce.setStyle(QTextListFormat::ListUpperRoman);
    }
    else{
        liste_puce.setStyle(QTextListFormat::ListDisc);
    }

    liste_puce.setIndent(liste_puce.indent() +1);
    curseur.createList(liste_puce);
    curseur.endEditBlock();
    return;
}

//Création des menus pour l'UI
void DadaWord::create_menus(){
    //Instance outils
    Outils instance_outils;
    //Création de la barre de menu "Fichier"
    QMenu *menu_fichier = menuBar()->addMenu(tr("Fichier"));

    QAction *nouveau_document = menu_fichier->addAction(tr("Nouveau document"));
    nouveau_document->setIcon(QIcon(":/menus/images/nouveau.png"));
    nouveau_document->setShortcut(QKeySequence("Ctrl+N"));
    nouveau_document->setStatusTip(tr("Ouvrir un nouveau document"));
    connect(nouveau_document, SIGNAL(triggered()), this, SLOT(ouvre_onglet()));

    QAction *menu_ouvrir_fichier = menu_fichier->addAction(tr("Ouvrir un fichier"));
    menu_ouvrir_fichier->setIcon(QIcon(":/menus/images/fileopen.png"));
    menu_ouvrir_fichier->setShortcut(QKeySequence("Ctrl+O"));
    menu_ouvrir_fichier->setStatusTip(tr("Ouvrir un nouveau fichier"));
    connect(menu_ouvrir_fichier, SIGNAL(triggered()), this, SLOT(ouvrir_fichier()));

    QMenu *menu_recents = menu_fichier->addMenu(tr("Récemments ouverts"));
    QStringList recemment_ouverts = instance_outils.fichiers_recents();
    if(recemment_ouverts.size() == 0){
        menu_recents->setEnabled(false);
    }
    else{
        QAction *action_ouverts[recemment_ouverts.size()];
        for(int i=(recemment_ouverts.size()-1); i>0; i--){
            QString temp = recemment_ouverts.at(i);
            if(!temp.isEmpty() && !temp.isNull()){
                action_ouverts[i] = menu_recents->addAction(temp.split("/").last());
                QSignalMapper *mappeur_string = new QSignalMapper;
                connect(action_ouverts[i], SIGNAL(triggered()), mappeur_string, SLOT(map()));
                mappeur_string->setMapping(action_ouverts[i], recemment_ouverts.at(i));
                connect(mappeur_string, SIGNAL(mapped(const QString &)), this, SLOT(ouvrir_fichier(const QString &)));
            }
            else{
                Erreur instance_erreur;
                instance_erreur.Erreur_msg(tr("Erreur lors de la génération des fichiers récents; des items vides ont été trouvés"), QMessageBox::Information);
            }
        }
    }

    enregistrer = menu_fichier->addAction(QIcon(":/menus/images/filesave.png"), tr("Enregistrer le fichier"));
    enregistrer->setIcon(QIcon(":/menus/images/filesave.png"));
    enregistrer->setShortcut(QKeySequence("Ctrl+S"));
    enregistrer->setStatusTip(tr("Enregistrer le fichier courant"));
    connect(enregistrer, SIGNAL(triggered()), this, SLOT(enregistrement()));
    //Désactivation par défaut, on a rien modifié
    enregistrer->setEnabled(false);
    //status_is_modified->setText(tr("Pas de modifications"));
    status_is_modified->setEnabled(false);

    QAction *enregistrer_sous = menu_fichier->addAction(QIcon(":/menus/images/enregistrer_sous.png"), tr("Enregistrer le fichier sous"));
    enregistrer_sous->setShortcut(QKeySequence("Maj+Ctrl+S"));
    enregistrer_sous->setStatusTip(tr("Enregistrer le fichier courant sous…"));
    connect(enregistrer_sous, SIGNAL(triggered()), this, SLOT(enregistrer_sous()));

    //Sous-menu d'export
    QMenu *exporter = menu_fichier->addMenu(tr("Exporter"));

    QAction *exporter_odt = exporter->addAction(tr("ODT"));
    exporter_odt->setIcon(QIcon(":/menus/images/odt.png"));
    exporter_odt->setShortcut(QKeySequence("Ctrl+E"));
    exporter_odt->setStatusTip(tr("Exporter le fichier courant en ODT"));
    connect(exporter_odt, SIGNAL(triggered()), this, SLOT(export_odt()));

    QAction *exporter_pdf = exporter->addAction(tr("PDF"));
    exporter_pdf->setIcon(QIcon(":/menus/images/pdf.png"));
    exporter_pdf->setStatusTip(tr("Exporter le fichier courant en PDF"));
    connect(exporter_pdf, SIGNAL(triggered()), this, SLOT(export_pdf()));


    QAction *apercu_impression = menu_fichier->addAction(tr("Aperçu avant impression"));
    connect(apercu_impression, SIGNAL(triggered()), this, SLOT(apercu_avant_impression()));
    apercu_impression->setStatusTip(tr("Afficher un aperçu avant impression"));
    apercu_impression->setIcon(QIcon(":/menus/images/document_preview.png"));

    QAction *impression = menu_fichier->addAction(tr("Imprimer"));
    impression->setIcon(QIcon(":/menus/images/imprimer.png"));
    impression->setStatusTip(tr("Imprimer le fichier courant"));
    impression->setShortcut(QKeySequence("Ctrl+P"));
    connect(impression, SIGNAL(triggered()), this, SLOT(imprimer()));

    QAction *fichier_fermer = menu_fichier->addAction(tr("Fermer"));
    fichier_fermer->setToolTip(tr("Fermer le document actif"));
    fichier_fermer->setShortcut(QKeySequence("Ctrl+W"));
    fichier_fermer->setIcon(QIcon(":/menus/images/close.png"));
    fichier_fermer->setStatusTip(tr("Fermer l'onglet courant"));
    connect(fichier_fermer, SIGNAL(triggered()), this, SLOT(fermer_fichier()));

    QAction *fichier_quitter = menu_fichier->addAction(tr("Quitter"));
    //Connexion slot
    connect(fichier_quitter, SIGNAL(triggered()), qApp, SLOT(quit()));
    //Image
    fichier_quitter->setIcon(QIcon(":/menus/images/exit.png"));
    //Shortcut
    fichier_quitter->setShortcut(QKeySequence(QKeySequence::Quit));
    //Tooltip
    fichier_quitter->setStatusTip(tr("Fermer le programme"));


    //Création du menu "Édition"
    QMenu *menu_edition = menuBar()->addMenu(tr("Édition"));
    edition_undo = menu_edition->addAction(tr("Annuler"));
    edition_undo->setIcon(QIcon(":/menus/images/undo.png"));
    edition_undo->setShortcut(QKeySequence("Ctrl+Z"));
    edition_undo->setStatusTip(tr("Annuler l'opération précédente"));
    connect(edition_undo, SIGNAL(triggered()), this, SLOT(make_undo()));

    edition_redo = menu_edition->addAction(tr("Refaire"));
    edition_redo->setIcon(QIcon(":/menus/images/redo.png"));
    edition_redo->setStatusTip(tr("Refaire l'action précédemment annulée"));
    edition_redo->setShortcut(QKeySequence("Ctrl+Maj+Z"));
    connect(edition_redo, SIGNAL(triggered()), this, SLOT(make_redo()));

    QAction *edition_couper = menu_edition->addAction(tr("Couper"));
    edition_couper->setStatusTip(tr("Couper la sélection"));
    edition_couper->setShortcut(QKeySequence("Ctrl+X"));
    edition_couper->setIcon(QIcon(":/menus/images/edit-cut.png"));
    connect(edition_couper, SIGNAL(triggered()), this, SLOT(couper()));

    QAction *edition_copier = menu_edition->addAction(tr("Copier"));
    edition_copier->setStatusTip(tr("Copier la sélection"));
    edition_copier->setShortcut(QKeySequence("Ctrl+C"));
    edition_copier->setIcon(QIcon(":/menus/images/edit-copy.png"));
    connect(edition_copier, SIGNAL(triggered()), this, SLOT(copier()));

    QAction *edition_coller = menu_edition->addAction(tr("Coller"));
    edition_coller->setStatusTip(tr("Coller la sélection"));
    edition_coller->setShortcut(QKeySequence("Ctrl+V"));
    edition_coller->setIcon(QIcon(":/menus/images/edit-paste.png"));
    connect(edition_coller, SIGNAL(triggered()), this, SLOT(coller()));

    //Recherche
    QAction *rechercher = menu_edition->addAction(tr("Rechercher"));
    rechercher->setStatusTip(tr("Recherche dans le document courant"));
    rechercher->setShortcut(QKeySequence("Ctrl+F"));
    rechercher->setIcon(QIcon(":/menus/images/search.png"));
    connect(rechercher, SIGNAL(triggered()), this, SLOT(recherche()));

    //Remplacer
    QAction *remplacer = menu_edition->addAction(tr("Remplacer"));
    remplacer->setStatusTip(tr("Remplacer du texte"));
    remplacer->setShortcut(QKeySequence("Ctrl+Maj+F"));
    remplacer->setIcon(QIcon(":/programme/images/remplace.png"));
    connect(remplacer, SIGNAL(triggered()), this, SLOT(call_remplace()));

    //Gestion des style
    QAction *gere_styles = menu_edition->addAction(tr("Gérer les styles"));
    gere_styles->setStatusTip(tr("Gére les styles par défauts et vous permet de créer vos propres styles"));
    Style *instance_style = new Style;
    connect(gere_styles, SIGNAL(triggered()), instance_style, SLOT(affiche_fen()));
    connect(instance_style, SIGNAL(styles_changed()), this, SLOT(recharge_styles()));

    //Alignement
    QMenu *alignement = menu_edition->addMenu(tr("Alignement"));
    QAction *alignement_gauche = alignement->addAction(tr("Gauche"));
    alignement_gauche->setToolTip(tr("Aligner le texte à gauche (écritures latines)"));
    alignement_gauche->setIcon(QIcon(":/menus/images/format_gauche.png"));
    alignement_gauche->setShortcut(QKeySequence("Ctrl+Maj+G"));
    QSignalMapper *mappeur_alignement = new QSignalMapper;
    connect(alignement_gauche, SIGNAL(triggered()), mappeur_alignement, SLOT(map()));
    mappeur_alignement->setMapping(alignement_gauche, Qt::AlignLeft);
    connect(mappeur_alignement, SIGNAL(mapped(const int &)), this, SLOT(change_align(const int &)));

    QAction *alignement_centre = alignement->addAction(tr("Centré"));
    alignement_centre->setToolTip(tr("Texte centré"));
    alignement_centre->setIcon(QIcon(":/menus/images/format_centre.png"));
    alignement_centre->setShortcut(QKeySequence("Ctrl+Maj+C"));
    connect(alignement_centre, SIGNAL(triggered()), mappeur_alignement, SLOT(map()));
    mappeur_alignement->setMapping(alignement_centre, Qt::AlignCenter);
    connect(mappeur_alignement, SIGNAL(mapped(const int &)), this, SLOT(change_align(const int &)));

    QAction *alignement_droit = alignement->addAction(tr("Droite"));
    alignement_droit->setToolTip(tr("Alignement à droite (écritures orientales)"));
    alignement_droit->setIcon(QIcon(":/menus/images/format_droite.png"));
    alignement_droit->setShortcut(QKeySequence("Ctrl+Maj+D"));
    connect(alignement_droit, SIGNAL(triggered()), mappeur_alignement, SLOT(map()));
    mappeur_alignement->setMapping(alignement_droit, Qt::AlignRight);
    connect(mappeur_alignement, SIGNAL(mapped(const int &)), this, SLOT(change_align(const int &)));

    QAction *alignement_justifie = alignement->addAction(tr("Justifié"));
    alignement_justifie->setToolTip(tr("Justifié (sans marges)"));
    alignement_justifie->setIcon(QIcon(":/menus/images/format_justifie.png"));
    alignement_justifie->setShortcut(QKeySequence("Ctrl+Maj+J"));
    connect(alignement_justifie, SIGNAL(triggered()), mappeur_alignement, SLOT(map()));
    mappeur_alignement->setMapping(alignement_justifie, Qt::AlignJustify);
    connect(mappeur_alignement, SIGNAL(mapped(const int &)), this, SLOT(change_align(const int &)));

    //Création de la barre de menu "Insertion"
    QMenu *menu_insertion = menuBar()->addMenu(tr("Insérer"));

    QAction *insere_puce = menu_insertion->addAction(tr("Puces"));
    insere_puce->setStatusTip(tr("Insérer une liste à puces"));
    insere_puce->setIcon(QIcon(":/menus/images/puces.png"));
    connect(insere_puce, SIGNAL(triggered()), this, SLOT(create_liste_puce()));

    QAction *puce_speciale = menu_insertion->addAction(tr("Liste ordonnée"));
    puce_speciale->setStatusTip(tr("Insérer une liste ordonnée"));
    puce_speciale->setIcon(QIcon(":/menus/images/liste_ordonnee.png"));
    QSignalMapper *mappeur_puce = new QSignalMapper;
    connect(puce_speciale, SIGNAL(triggered()), mappeur_puce, SLOT(map()));
    mappeur_puce->setMapping(puce_speciale, 1);
    connect(mappeur_puce, SIGNAL(mapped(const int)), this, SLOT(create_liste_puce(const int)));

    incremente_puce_bouton = menu_insertion->addAction(tr("Incrémenter la puce"));
    incremente_puce_bouton->setEnabled(false);
    connect(incremente_puce_bouton, SIGNAL(triggered()), this, SLOT(incremente_puce()));

    desincremente_puce_bouton = menu_insertion->addAction(tr("Désincrémente la puce"));
    desincremente_puce_bouton->setEnabled(false);
    connect(desincremente_puce_bouton, SIGNAL(triggered()), this, SLOT(desincremente_puce()));

    QAction *insere_image = menu_insertion->addAction(tr("Insérer une image"));
    insere_image->setIcon(QIcon(":/menus/images/image.png"));
    insere_image->setStatusTip(tr("Insérer une image"));
    connect(insere_image, SIGNAL(triggered()), this, SLOT(add_image()));

    QMenu *menu_tableau = menu_insertion->addMenu(tr("Tableau"));
    QAction *insere_tableau = menu_tableau->addAction(tr("Insérer un tableau"));
    insere_tableau->setIcon(QIcon(":/menus/images/tableau.png"));
    insere_tableau->setStatusTip(tr("Insère un tableau"));
    connect(insere_tableau, SIGNAL(triggered()), this, SLOT(creer_tableau()));

    QAction *ajoute_ligne = menu_tableau->addAction(tr("Ajouter une ligne"));
    ajoute_ligne->setIcon(QIcon(":/menus/images/add_row.png"));
    ajoute_ligne->setStatusTip(tr("Ajouter une ligne au tableau"));
    QSignalMapper *tableau = new QSignalMapper;
    connect(ajoute_ligne, SIGNAL(triggered()), tableau, SLOT(map()));
    tableau->setMapping(ajoute_ligne, ROW);
    connect(tableau, SIGNAL(mapped(const int &)), this, SLOT(tableau_add(const int &)));

    QAction *ajoute_colonne = menu_tableau->addAction(tr("Ajouter une colonne"));
    ajoute_colonne->setIcon(QIcon(":/menus/images/add_column.jpeg"));
    ajoute_colonne->setStatusTip(tr("Ajoute une colonne au tableau"));
    QSignalMapper *tableau2 = new QSignalMapper;
    connect(ajoute_colonne, SIGNAL(triggered()), tableau2, SLOT(map()));
    tableau2->setMapping(ajoute_colonne, COLL);
    connect(tableau2, SIGNAL(mapped(const int &)), this, SLOT(tableau_add(const int &)));

    QAction *delete_ligne = menu_tableau->addAction(tr("Supprimer une ligne"));
    delete_ligne->setIcon(QIcon(":/menus/images/delete_row.jpeg"));
    delete_ligne->setStatusTip(tr("Supprime une ligne précise du tableau"));
    QSignalMapper *tableau3 = new QSignalMapper;
    connect(delete_ligne, SIGNAL(triggered()), tableau3, SLOT(map()));
    tableau3->setMapping(delete_ligne, ROW);
    connect(tableau3, SIGNAL(mapped(const int &)), this, SLOT(tableau_remove(const int &)));

    QAction *delete_colonne = menu_tableau->addAction(tr("Supprimer une colonne"));
    delete_colonne->setIcon(QIcon(":/menus/images/delete_column.gif"));
    delete_colonne->setStatusTip(tr("Supprime une colonne précise du tableau"));
    QSignalMapper *tableau4 = new QSignalMapper;
    connect(delete_colonne, SIGNAL(triggered()), tableau4, SLOT(map()));
    tableau4->setMapping(delete_colonne, COLL);
    connect(tableau4, SIGNAL(mapped(const int &)), this, SLOT(tableau_remove(const int &)));


    //Création du menu "Outils"
    QMenu *menu_outils = menuBar()->addMenu(tr("Outils"));

    //Sous-menu : "Affichage"
    QMenu *affichage = menu_outils->addMenu(tr("Barres d'outils"));
    affichage->setStatusTip(tr("Gère l'affichage des barres d'outils"));
    affichage->setIcon(QIcon(":/menus/images/visible.png"));

    //Contenu du sous-menu
    affichage_default = affichage->addAction(tr("Défaut"));
    affichage_default->setCheckable(true);
    affichage_default->setChecked(true);
    affichage_puces = affichage->addAction(tr("Puces"));
    affichage_puces->setCheckable(true);
    affichage_puces->setChecked(false);
    affichage_tableau = affichage->addAction(tr("Tableaux"));
    affichage_tableau->setCheckable(true);
    affichage_tableau->setChecked(false);
    affichage_recherche = affichage->addAction(tr("Recherche"));
    affichage_recherche->setCheckable(true);
    affichage_recherche->setChecked(false);
    affichage_format = affichage->addAction(tr("Format"));
    affichage_format->setCheckable(true);
    affichage_format->setChecked(true);

    //On connecte les affichages au slot
    QSignalMapper *mappeur_toolbar = new QSignalMapper;
    connect(affichage_default, SIGNAL(triggered()), mappeur_toolbar, SLOT(map()));
    mappeur_toolbar->setMapping(affichage_default, DEFAULT);
    connect(mappeur_toolbar, SIGNAL(mapped(const int)), this, SLOT(hide_toolbar(const int)));
    QSignalMapper *mappeur_toolbar2 = new QSignalMapper;
    connect(affichage_puces, SIGNAL(triggered()), mappeur_toolbar2, SLOT(map()));
    mappeur_toolbar2->setMapping(affichage_puces, PUCES);
    connect(mappeur_toolbar2, SIGNAL(mapped(const int)), this, SLOT(hide_toolbar(const int)));
    QSignalMapper *mappeur_toolbar3 = new QSignalMapper;
    connect(affichage_tableau, SIGNAL(triggered()), mappeur_toolbar3, SLOT(map()));
    mappeur_toolbar3->setMapping(affichage_tableau, TABLEAUX);
    connect(mappeur_toolbar3, SIGNAL(mapped(const int)), this, SLOT(hide_toolbar(const int)));
    QSignalMapper *mappeur_toolbar4 = new QSignalMapper;
    connect(affichage_recherche, SIGNAL(triggered()), mappeur_toolbar4, SLOT(map()));
    mappeur_toolbar4->setMapping(affichage_recherche, RECHERCHE);
    connect(mappeur_toolbar4, SIGNAL(mapped(const int)), this, SLOT(hide_toolbar(const int)));
    QSignalMapper *mappeur_toolbar8 = new QSignalMapper;
    connect(affichage_format, SIGNAL(triggered()), mappeur_toolbar8, SLOT(map()));
    mappeur_toolbar8->setMapping(affichage_format, FORMAT);
    connect(mappeur_toolbar8, SIGNAL(mapped(const int)), this, SLOT(hide_toolbar(const int)));

    //Lecture seule
    lecture_seule = menu_outils->addAction(tr("Lecture seule"));
    lecture_seule->setStatusTip(tr("Bloque l'accès en écriture au document courant"));
    lecture_seule->setCheckable(true);
    lecture_seule->setChecked(false);
    connect(lecture_seule, SIGNAL(triggered()), this, SLOT(slot_lecture_seule()));

    //Orthographe
    QAction *verif_langue = menu_outils->addAction(tr("Vérification orthographique"));
    verif_langue->setIcon(QIcon(":/menus/images/orthographe.png"));
    verif_langue->setStatusTip(tr("Vérifie l'orthographe du document courant"));
    verif_langue->setShortcut(QKeySequence("F7"));
    connect(verif_langue, SIGNAL(triggered()), this, SLOT(verif_orthographe()));

    //Langue
    QAction *change_langue = menu_outils->addAction(tr("Langue du correcteur"));
    change_langue->setStatusTip(tr("Change la langue du correcteur orthographique"));
    change_langue->setIcon(QIcon(":/menus/images/langue.png"));
    connect(change_langue, SIGNAL(triggered()), this, SLOT(orth_langue()));

    //Mode texte
    to_text = menu_outils->addAction(tr("Mode texte seul"));
    to_text->setIcon(QIcon(":/menus/images/text.png"));
    to_text->setStatusTip(tr("Afficher le fichier en mode texte"));
    to_text->setCheckable(true);
    to_text->setChecked(false);
    connect(to_text, SIGNAL(triggered()), this, SLOT(to_plain_text()));

    //Gestion du plein écran
    full_screen = menu_outils->addAction(tr("Mode plein écran"));
    full_screen->setIcon(QIcon(":/menus/images/full_screen.png"));
    full_screen->setStatusTip(tr("Affiche DadaWord en mode plein écran"));
    full_screen->setCheckable(true);
    full_screen->setChecked(false);
    full_screen->setShortcut(QKeySequence(Qt::Key_F10));
    connect(full_screen, SIGNAL(triggered()), this, SLOT(make_full_screen()));

    //Coloration du HTML
    colore_html = menu_outils->addAction(tr("Colorer la syntaxe"));
    colore_html->setIcon(QIcon(":/menus/images/coloration_syntaxique.png"));
    colore_html->setStatusTip(tr("Colore le code HTML"));
    colore_html->setCheckable(true);
    colore_html->setChecked(false);
    colore_html->setEnabled(false);
    connect(colore_html, SIGNAL(triggered()), this, SLOT(html_highlight()));

    //Statistiques
    QAction *statistiques_doc = menu_outils->addAction(tr("Statistiques"));
    statistiques_doc->setIcon(QIcon(":/menus/images/statistiques.png"));
    statistiques_doc->setStatusTip(tr("Affiche les statistiques pour le document en cours"));
    connect(statistiques_doc, SIGNAL(triggered()), this, SLOT(statistiques()));

    //Gestion du log
    QAction *gestion_log = menu_outils->addAction(tr("Gestion du log"));
    gestion_log->setIcon(QIcon(":/menus/images/log.png"));
    gestion_log->setStatusTip("Gérer le fichier de log");
    Outils *instance_connect = new Outils;
    connect(gestion_log, SIGNAL(triggered()), instance_connect, SLOT(affiche_log()));

    //Preferences
    QAction *preferences = menu_outils->addAction(tr("Préférences"));
    preferences->setIcon(QIcon(":/menus/images/outils.png"));
    preferences->setToolTip(tr("Préférences de DadaWord"));
    preferences->setStatusTip(tr("Configuration de DadaWord"));
    connect(preferences, SIGNAL(triggered()), instance_connect, SLOT(fenetre_config()));


    //Création de la barre de menu "Aide"
    QMenu *menu_aide = menuBar()->addMenu(tr("Aide"));
    //Menu "Aide"
    QAction *aide_a_propos = menu_aide->addAction(QIcon(":/menus/images/about.gif"), tr("A propos de Dadaword"));
    aide_a_propos->setToolTip(tr("A propos de Dadaword"));
    //Image
    aide_a_propos->setIcon(QIcon(":/menus/images/about.gif"));
    aide_a_propos->setStatusTip(tr("À propos du créateur de ce génial programme"));
    //Connexion slot
    connect(aide_a_propos, SIGNAL(triggered()), this, SLOT(affiche_about()));

    //AboutQt
    QAction *about_qt = menu_aide->addAction(tr("À propos de Qt"));
    about_qt->setIcon(QIcon(":/menus/images/logo_qt.jpg"));
    about_qt->setToolTip(tr("Qt est la librairie utilisée par DadaWord"));
    about_qt->setStatusTip(tr("À propos de Qt"));
    connect(about_qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


    //Ajout de la barre d'outils
    barre_standard = addToolBar(tr("Défaut"));
    barre_standard->addAction(nouveau_document);
    barre_standard->addAction(menu_ouvrir_fichier);
    barre_standard->addAction(enregistrer);
    choix_police = new QFontComboBox;
    choix_police->setCurrentFont(instance_outils.lire_config("police").value<QFont>());
    barre_standard->addWidget(choix_police);
    //Connection du slot
    connect(choix_police, SIGNAL(currentFontChanged(QFont)), this, SLOT(change_police(QFont)));

    taille_police = new QSpinBox;
    taille_police->setValue(instance_outils.lire_config("taille").toInt());
    barre_standard->addWidget(taille_police);
    //Connexion au slot
    connect(taille_police, SIGNAL(valueChanged(int)), this, SLOT(change_taille(int)));

    gras = barre_standard->addAction(QIcon(":/menus/images/text_bold.png"), tr("Gras"));
    gras->setShortcut(QKeySequence("Ctrl+B"));
    connect(gras, SIGNAL(triggered(bool)), this, SLOT(graisse_police(bool)));
    gras->setCheckable(true);

    italique = barre_standard->addAction(QIcon(":/menus/images/text_italic.png"), tr("Italique"));
    italique->setShortcut(QKeySequence("Ctrl+I"));
    connect(italique, SIGNAL(triggered(bool)), this, SLOT(italique_police(bool)));
    italique->setCheckable(true);

    souligne = barre_standard->addAction(QIcon(":/menus/images/text_under.png"), tr("Souligné"));
    souligne->setShortcut(QKeySequence("Ctrl+U"));
    connect(souligne, SIGNAL(triggered(bool)), this, SLOT(souligne_police(bool)));
    souligne->setCheckable(true);

    QAction *couleur_texte = barre_standard->addAction(QIcon(":/menus/images/couleur_texte.png"), tr("Couleur du texte"));
    //Mappeur pour passer la value à la fonction
    QSignalMapper *mappeur_couleur = new QSignalMapper;
    connect(couleur_texte, SIGNAL(triggered()), mappeur_couleur, SLOT(map()));
    mappeur_couleur->setMapping(couleur_texte, TEXTE);
    connect(mappeur_couleur, SIGNAL(mapped(const int &)), this, SLOT(change_couleur(const int &)));

    QAction *couleur_highlight = barre_standard->addAction(QIcon(":/menus/images/couleur_highlight.png"), tr("Couleur de surlignage"));
    //Mappeur 2 pour passer une valeur à la fonction
    QSignalMapper *mappeur_couleur2 = new QSignalMapper;
    connect(couleur_highlight, SIGNAL(triggered()), mappeur_couleur2, SLOT(map()));
    mappeur_couleur2->setMapping(couleur_highlight, SURLIGNE);
    connect(mappeur_couleur2, SIGNAL(mapped(const int &)), this, SLOT(change_couleur(const int &)));

    barre_standard->addAction(verif_langue);

    //Création de la toolbar de format
    bar_format = new QToolBar;
    addToolBar(Qt::TopToolBarArea, bar_format);
    nom_format = new QComboBox;
    QSettings settings("Dadaword", "dadaword");
    QStringList noms_styles = settings.value("noms_styles").toStringList();
    for(int i=0; i<noms_styles.count(); i++){
        nom_format->addItem(noms_styles.at(i));
    }
    bar_format->addWidget(nom_format);
    //Connexion
    connect(nom_format, SIGNAL(activated(int)), this, SLOT(change_style(int)));

    //Création de la toolbar des puces
    puces = new QToolBar;
    addToolBar(Qt::BottomToolBarArea, puces);
    incremente_puce_bouton->setIcon(QIcon(":/menus/images/suivant.png"));
    desincremente_puce_bouton->setIcon(QIcon(":/menus/images/precedent.png"));
    puces->addAction(incremente_puce_bouton);
    puces->addAction(desincremente_puce_bouton);
    puces->hide();

    //Création de la toolbar des tableaux
    barre_tableau = new QToolBar;
    addToolBar(Qt::BottomToolBarArea, barre_tableau);
    barre_tableau->addAction(insere_tableau);
    barre_tableau->addAction(ajoute_ligne);
    barre_tableau->addAction(ajoute_colonne);
    barre_tableau->addSeparator();
    barre_tableau->addAction(delete_ligne);
    barre_tableau->addAction(delete_colonne);
    barre_tableau->hide();
    barre_tableau->setMovable(true);

    //Création de la toolbar de recherche
    barre_recherche = new QToolBar;
    addToolBar(Qt::BottomToolBarArea, barre_recherche);
    champ_recherche = new QLineEdit();
    barre_recherche->addAction(rechercher);
    barre_recherche->addWidget(champ_recherche);
    barre_recherche->hide();
    barre_recherche->setMovable(true);
    QAction *make_recherche = barre_recherche->addAction(tr("Rechercher"));
    make_recherche->setIcon(QIcon(":/menus/images/ok.png"));
    make_recherche->setStatusTip(tr("Rechercher dans le document"));
    QSignalMapper *mappeur_toolbar5 = new QSignalMapper;
    connect(make_recherche, SIGNAL(triggered()), mappeur_toolbar5, SLOT(map()));
    mappeur_toolbar5->setMapping(make_recherche, QTOOLBAR);
    connect(mappeur_toolbar5, SIGNAL(mapped(const int)), this, SLOT(make_search(const int)));
    //On connect aussi la QLineEdit
    QSignalMapper *mappeur_toolbar9 = new QSignalMapper;
    connect(champ_recherche, SIGNAL(returnPressed()), mappeur_toolbar9, SLOT(map()));
    mappeur_toolbar9->setMapping(champ_recherche, QTOOLBAR);
    connect(mappeur_toolbar9, SIGNAL(mapped(const int)), this, SLOT(make_search(const int)));
    QAction *recherche_avant = barre_recherche->addAction(tr("Occurence précédente"));
    recherche_avant->setIcon(QIcon(":/menus/images/avant.png"));
    recherche_avant->setStatusTip(tr("Recherche l'occurence précédente"));
    QAction *recherche_apres = barre_recherche->addAction(tr("Occurence suivante"));
    recherche_apres->setIcon(QIcon(":/menus/images/apres.png"));
    recherche_apres->setStatusTip(tr("Recherche l'occurence suivante"));
    QAction *ferme_recherche = barre_recherche->addAction(tr("Ferme la barre d'outils"));
    ferme_recherche->setIcon(QIcon(":/menus/images/exit.png"));
    ferme_recherche->setStatusTip(tr("Ferme la barre d'outils de recherche"));
    connect(ferme_recherche, SIGNAL(triggered()), this, SLOT(hide_searchbar()));
    QSignalMapper *mappeur_toolbar6 = new QSignalMapper;
    connect(recherche_avant, SIGNAL(triggered()), mappeur_toolbar6, SLOT(map()));
    mappeur_toolbar6->setMapping(recherche_avant, GAUCHE);
    connect(mappeur_toolbar6, SIGNAL(mapped(const int)), this, SLOT(make_search(const int)));
    QSignalMapper *mappeur_toolbar7 = new QSignalMapper;
    connect(recherche_apres, SIGNAL(triggered()), mappeur_toolbar7, SLOT(map()));
    mappeur_toolbar7->setMapping(recherche_apres, DROITE);
    connect(mappeur_toolbar7, SIGNAL(mapped(const int)), this, SLOT(make_search(const int)));

    //Création de la barre d'orthographe
    barre_orthographe = new QToolBar;
    addToolBar(Qt::LeftToolBarArea, barre_orthographe);
    orth_suggest = new QComboBox;
    orth_mot = new QLabel("N/A");
    QPushButton *add_dico, *remplace_tout, *ignore, *ignore_tout, *suivant, *fin_orth;
    add_dico = new QPushButton(tr("Ajouter au dictionnaire"));
    add_dico->setIcon(QIcon(":/menus/images/add_dico.png"));
    remplace_tout = new QPushButton(tr("Remplacer tout"));
    ignore = new QPushButton(tr("Ignorer"));
    ignore->setIcon(QIcon(":/menus/images/apres.png"));
    ignore_tout = new QPushButton(tr("Ignorer tout"));
    ignore_tout->setIcon(QIcon(":/menus/images/ignorer.png"));
    suivant = new QPushButton(tr("Correction suivante"));
    suivant->setIcon(QIcon(":/menus/images/suivant2.png"));
    fin_orth = new QPushButton(tr("Annuler"));
    fin_orth->setIcon(QIcon(":/menus/images/exit.png"));
    //Ajout à la barre
    barre_orthographe->addWidget(orth_mot);
    barre_orthographe->addWidget(orth_suggest);
    barre_orthographe->addWidget(add_dico);
    barre_orthographe->addWidget(remplace_tout);
    barre_orthographe->addWidget(ignore);
    barre_orthographe->addWidget(ignore_tout);
    barre_orthographe->addWidget(suivant);
    barre_orthographe->addWidget(fin_orth);
    //Connexions
    connect(orth_suggest, SIGNAL(activated(QString)), this, SLOT(orth_remplace(QString)));
    connect(add_dico, SIGNAL(clicked()), this, SLOT(orth_dico()));
    connect(remplace_tout, SIGNAL(clicked()), this, SLOT(orth_remplace_all()));
    connect(ignore, SIGNAL(clicked()), this, SLOT(verif_orthographe())); //On ne fait que passer au mot suivant…
    connect(ignore_tout, SIGNAL(clicked()), this, SLOT(orth_ignore()));
    connect(suivant, SIGNAL(clicked()), this, SLOT(verif_orthographe()));
    connect(fin_orth, SIGNAL(clicked()), this, SLOT(orth_stop()));
    //On cache la barre par défaut
    barre_orthographe->hide();

    return;
}

//Ouverture d'un nouvel onglet
void DadaWord::ouvre_onglet(bool fichier, QString titre){
    //Titre du document
    bool titre_ok = false;
    QString reponse;
    if(!fichier){
        reponse = QInputDialog::getText(this, tr("Titre du document"), tr("Quel est le titre du document?"), QLineEdit::Normal, QString(), &titre_ok);
    }
    else{
        titre_ok = true;
        reponse = titre;
    }
    if(titre_ok && !reponse.isEmpty()){
        QTextEdit *document_onglet = new QTextEdit;
        document_onglet->installEventFilter(this);
        document_onglet->setContextMenuPolicy(Qt::CustomContextMenu); //Activation du menu personnalisé
        connect(document_onglet, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(affiche_menu_perso(const QPoint &)));//Connection au slot d'affichage du menu
        connect(document_onglet, SIGNAL(cursorPositionChanged()), this, SLOT(curseur_change()));
        QTextDocument *doc_principal_onglet = new QTextDocument;
        document_onglet->setDocument(doc_principal_onglet);
        Outils instance_outils;
        QMdiSubWindow *zone_document_onglet;
        if(instance_outils.lire_config("word").toBool()){
            zone_document_onglet = new QMdiSubWindow;
            zone_centrale->addSubWindow(zone_document_onglet);
            QPrinter printer(QPrinter::HighResolution);
            printer.setPaperSize(QPrinter::A4);
            document_onglet->setMaximumHeight((printer.paperSize(QPrinter::Point)).toSize().rheight()+MARGIN_WORD);
            document_onglet->setMaximumWidth((printer.paperSize(QPrinter::Point)).toSize().rwidth()+MARGIN_WORD+TAMPON_WORD);
            //document_onglet->setMaximumSize((printer.paperSize(QPrinter::Point)).toSize());
            document_onglet->setMinimumWidth((printer.paperSize(QPrinter::Point)).toSize().rwidth()+MARGIN_WORD+TAMPON_WORD);//Addition d'un tampon parce qu'on est pas tout à fait juste
            QHBoxLayout *layout_horizontal = new QHBoxLayout;
            QWidget *widget = new QWidget; //Sert juste pour le layout
            layout_horizontal->addWidget(document_onglet, 0, Qt::AlignHCenter);
            document_onglet->setFrameShape(QFrame::NoFrame);
            widget->setLayout(layout_horizontal);
            zone_document_onglet->setWidget(widget);
            QTextFrame *tf = doc_principal_onglet->rootFrame();
            QTextFrameFormat tff = tf->frameFormat();
            tff.setMargin(MARGIN_WORD);
            tf->setFrameFormat(tff);

            doc_principal_onglet->setModified(false);
        }
        else{
            zone_document_onglet = zone_centrale->addSubWindow(document_onglet);
        }
        zone_document_onglet->setWindowTitle(reponse);
        zone_document_onglet->setAccessibleName(reponse);
        //On delete en plus de fermer
        zone_document_onglet->setAttribute(Qt::WA_DeleteOnClose);
        connect(doc_principal_onglet, SIGNAL(contentsChanged()), this, SLOT(indicateur_modifications()));
        connect(document_onglet, SIGNAL(undoAvailable(bool)), this, SLOT(slot_undo(bool)));
        connect(document_onglet, SIGNAL(redoAvailable(bool)), this, SLOT(slot_redo(bool)));
        zone_document_onglet->setFocus();
        document_onglet->setFocus();
        zone_document_onglet->show();
    }
    else{
        return;
    }
    return;
}

//Active/désactive le bouton "enregistrer"
void DadaWord::indicateur_modifications(){
    //Gestion de l'icône d'enregistrement
    if(!enregistrer->isEnabled()){
        enregistrer->setEnabled(true);
        //status_is_modified->setText(tr("Document modifié"));
        status_is_modified->setEnabled(true);
    }
    return;
}

//Renvoie le QMdiSubWindow de l'onglet actif
QMdiSubWindow* DadaWord::find_onglet(){
    return zone_centrale->currentSubWindow();
}

//Trouve le QTextEdit de l'onglet actif
QTextEdit* DadaWord::find_edit(){
    QMdiSubWindow *temp = find_onglet();
    if(temp == 0)
        return 0;
    return temp->findChild<QTextEdit *>();
}

//Si on change d'onglet actif, il faut mettre à jour les menus
void DadaWord::changement_focus(QMdiSubWindow *fenetre_activee){


    QTextEdit *text_edit_temp = fenetre_activee->findChild<QTextEdit *>();
    if(text_edit_temp != 0){
        //Mise à jour des paramètres dans le menu
        //Police
        QFont police_onglet = text_edit_temp->currentFont();
        QTextDocument *document_actuel = text_edit_temp->document();
        if(document_actuel->isEmpty()){
            Outils instance_outils;
            choix_police->setCurrentFont(instance_outils.lire_config("police").value<QFont>());
            text_edit_temp->setFont(instance_outils.lire_config("police").value<QFont>());
        }
        else{
            choix_police->setCurrentFont(police_onglet);
        }
        //Taille
        qreal taille_onglet = text_edit_temp->fontPointSize();
        if(taille_onglet <= 0){
            Outils instance_outils;
            taille_police->setValue(instance_outils.lire_config("taille").toInt());
            text_edit_temp->setFontPointSize(instance_outils.lire_config("taille").toInt());
        }
        else{
            taille_police->setValue(taille_onglet);
        }
        //Gras
        int gras_onglet = text_edit_temp->fontWeight();
        if(gras_onglet == QFont::Bold){
            gras->setChecked(true);
        }
        else if(gras_onglet == QFont::Normal){
            gras->setChecked(false);
        }
        else{
            //BUG!!!
        }
        //Italique
        bool italique_onglet = text_edit_temp->fontItalic();
        if(italique_onglet){
            italique->setChecked(true);
        }
        else{
            italique->setChecked(false);
        }
        //Souligne
        bool souligne_onglet = text_edit_temp->fontUnderline();
        if(souligne_onglet){
            souligne->setChecked(true);
        }
        else{
            souligne->setChecked(false);
        }
        //Lecture seule
        lecture_seule->setChecked(text_edit_temp->isReadOnly());
        if(find_edit()->acceptRichText() && find_onglet()->accessibleName().contains(".txt")){
            find_edit()->setAcceptRichText(false);
            to_text->setChecked(true);
        }
        //Mode texte
        to_text->setChecked(!find_edit()->acceptRichText());
        //Reset du curseur pour la correction orthographique
        pos_orth.movePosition(QTextCursor::Start);

        //Coloration syntaxique
        //On ne l'affiche que si l'extension le permet
        if(find_onglet()->windowTitle().contains(".htm") || find_onglet()->windowTitle().contains(".xml")){
            colore_html->setEnabled(true);
        }
        else{
            colore_html->setEnabled(false);
        }
    }//Fin du "if" fenêtre valide
    return;
}

//Fermeture de l'onglet actif
void DadaWord::fermer_fichier(){
    QMdiSubWindow *onglet_actif = find_onglet();
    QTextEdit *text_edit_actif = find_edit();
    QTextDocument *document_actif = text_edit_actif->document();
    if(document_actif->isModified()){
        alerte_enregistrement(onglet_actif);
    }
    onglet_actif->close();
    return;
}

//Alerte d'enregistrement de fichier
void DadaWord::alerte_enregistrement(QMdiSubWindow *fenetre){
    Outils instance_outils;
    if(instance_outils.lire_config("alertes").toInt() == LOW){
        return;
    }
    int reponse = QMessageBox::question(this, tr("Enregistrer le fichier?"), "Des modifications ont été apportées au document \""+fenetre->accessibleName()+"\" depuis sa dernière modification.\n Voulez-vous les sauvegarder?", QMessageBox::Yes | QMessageBox::No);
    if(reponse == QMessageBox::Yes){
        enregistrement(fenetre);
    }
    else if(reponse == QMessageBox::No){
        //On ne fait rien, le programme est déjà en train de se fermer
    }
    else{
        //FATAL ERROR!!!!! Mais pour l'instant on ne fait rien
        Erreur instance_erreur;
        instance_erreur.Erreur_msg("Félicitations! Vous avez réussi à faire planter le programme! Un point pour vous.", QMessageBox::Critical);
    }
    return;
}

//Fonction qui gére le changement de couleur. Le paramètre indique si la couleur s'applique à l'arrière-plan ou au texte.
void DadaWord::change_couleur(const int &value){
    //On récupère la couleur souhaitée :
    QColor couleur = QColorDialog::getColor(Qt::yellow, this);

    //On applique la couleur au texte :
    if(value == TEXTE){
        find_edit()->setTextColor(couleur);
    }
    else if(value == SURLIGNE){
        QTextCharFormat format_couleur;
        format_couleur.setBackground(QBrush(couleur));
        find_edit()->setCurrentCharFormat(format_couleur);
    }
    else{
        //Erreur, on ne peut jamais arriver ici
        Erreur instance_erreur;
        instance_erreur.Erreur_msg(tr("Exception inconnue lors du choix de couleur.\n L'option choisie n'est pas valide"), QMessageBox::Warning);
        return;
    }
    return;
}

//Slot du Redo
void DadaWord::make_redo(){
    find_edit()->redo();
    return;
}

//Slot du Undo
void DadaWord::make_undo(){
    find_edit()->undo();
    return;
}

//Slot pour l'activation/désactivation du Redo
void DadaWord::slot_redo(bool etat){
    if(etat){
        edition_redo->setEnabled(true);
    }
    else{
        edition_redo->setEnabled(false);
    }
    return;
}

//Slot pour l'activation/désactivation du Undo
void DadaWord::slot_undo(bool etat){
    if(etat){
        edition_undo->setEnabled(true);
    }
    else{
        edition_undo->setEnabled(false);
    }
    return;
}

//Enregistrer Sous
void DadaWord::enregistrer_sous(){
    //On balance juste à "enregistrer" avec "saveas" en "true"
    enregistrement(find_onglet(), true);
    return;
}

//Changement de l'alignement du texte
void DadaWord::change_align(const int &value){
    if(value == Qt::AlignCenter){
        find_edit()->setAlignment(Qt::AlignCenter);
    }
    else if(value == Qt::AlignLeft){
        find_edit()->setAlignment(Qt::AlignLeft);
    }
    else if(value == Qt::AlignRight){
        find_edit()->setAlignment(Qt::AlignRight);
    }
    else if(value == Qt::AlignJustify){
        find_edit()->setAlignment(Qt::AlignJustify);
    }
    else{
        Erreur instance_erreur;
        instance_erreur.Erreur_msg(tr("Un alignement non présent a été demandé"), QMessageBox::Information);
        find_edit()->setAlignment(Qt::AlignLeft);
    }
    return;
}

//Insertion d'image (dans les fichiers DDW)
void DadaWord::add_image(){
    Outils instance_outils;
    if(!find_edit()->acceptRichText()){
        if(instance_outils.lire_config("alertes").toInt() != LOW){
            Erreur instance_erreur;
            instance_erreur.Erreur_msg(tr("Malheureusement, votre document n'accepte pas l'insertion d'image.\n Veuillez réessayer avec un format compatible comme \".ddw\""), QMessageBox::Information);
        }
        //Évidamment, on quitte
        return;
    }
    //1)Sélectionne l'image
    QString chemin_image = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QDir::homePath(), "Images (*.png *.gif *.jpg *.jpeg *bmp)");

    //S'il n'y a pas d'image, on se casse
    if(chemin_image.isEmpty() || chemin_image.isNull()){
        return;
    }

    //On enregistre le document par mesure de sécurité
    enregistrement();

    //On insère l'image
    find_edit()->insertHtml(("<img src=\""+chemin_image+"\">"));

    return;
}

//Slot de fermeture d'onglet
void DadaWord::close_tab_button(int index){
    QList<QMdiSubWindow *> liste = zone_centrale->findChildren<QMdiSubWindow *>();
    QTextEdit *text_edit_actif = liste.at(index)->findChild<QTextEdit *>();
    QTextDocument *document_actif = text_edit_actif->document();
    if(document_actif->isModified()){
        alerte_enregistrement(liste.at(index));
    }

    liste.at(index)->close();
    //}
    return;
}

//(Dés)Active la lecture seule
void DadaWord::slot_lecture_seule(){
    find_edit()->setReadOnly(lecture_seule->isChecked());
    return;
}

//Crée un tableau en demandant la taille à l'utilisateur
void DadaWord::creer_tableau(){
    bool ok = true;
    //On demande quelle taille de tableau à l'utilisateur
    int largeur = QInputDialog::getInt(this, tr("Largeur du tableau"), tr("Combien de colonnes?"), 4, 1, 10000, 1, &ok);
    if(!ok){
        //L'utilisateur a appuyé sur "Annuler" -> On se casse
        return;
    }
    int longueur = QInputDialog::getInt(this, tr("Longueur tableau"), tr("Combien de lignes?"), 5, 1, 10000, 1, &ok);
    if(!ok){
        //Idem
        return;
    }

    QTextCursor curseur = find_edit()->textCursor();
    curseur.insertTable(longueur, largeur);
    QTextTable *tableau = curseur.currentTable();
    QTextTableFormat objet;
    objet.setCellPadding(2);
    tableau->setFormat(objet);
    barre_tableau->show();
    //On change l'état du menu
    affichage_tableau->setChecked(true);

    return;
}

//Ajoute une ligne/colonne au tableau courant
void DadaWord::tableau_add(const int &pos){
    QTextCursor curseur = find_edit()->textCursor();
    if(curseur.currentTable() == 0){
        Erreur instance;
        instance.Erreur_msg(tr("Tentative d'insertion de ligne dans un tableau inexistant"), QMessageBox::Information);
        return;
    }
    QTextTable *tableau = curseur.currentTable();
    while(curseur.currentTable() == 0){
        curseur.movePosition(QTextCursor::PreviousBlock);
    }

    //Emplacement dans les cases
    //int emplacement_top = ((curseur.position() - curseur.currentTable()->firstPosition())+1)/tableau->columns();

    if(pos == ROW){
        //Insertion de la ligne
        tableau->appendRows(1);
    }
    else if(pos == COLL){
        //Insertion de la colonne
        tableau->appendColumns(1);
    }
    else{
        Erreur instance;
        instance.Erreur_msg(tr("Opération illégale dans la modification de tableau!"), QMessageBox::Warning);
        return;
    }

    return;
}

//Supprime une ligne/colonne au tableau courant
void DadaWord::tableau_remove(const int &pos){
    QTextCursor curseur = find_edit()->textCursor();
    if(curseur.currentTable() == 0){
        Erreur instance;
        instance.Erreur_msg(tr("Tentative d'insertion de ligne dans un tableau inexistant"), QMessageBox::Information);
        return;
    }
    QTextTable *tableau = curseur.currentTable();
    while(curseur.currentTable() == 0){
        curseur.movePosition(QTextCursor::PreviousBlock);
    }

    //Emplacement dans les cases
    //int emplacement_top = ((curseur.position() - curseur.currentTable()->firstPosition())+1)/tableau->columns();

    if(pos == ROW){
        //Suppression de la ligne
        int position = QInputDialog::getInt(this, tr("Supprimer une ligne"), tr("Quelle ligne voulez-vous supprimer?"), 1, 1, tableau->rows());
        if(position != 0){
            tableau->removeRows((position-1), 1);
        }
    }
    else if(pos == COLL){
        //Insertion de la colonne
        int position = QInputDialog::getInt(this, tr("Supprimer une colonne"), tr("Quelle colonne voulez-vous supprimer?"), 1, 1, tableau->columns());
        if(position != 0){
            tableau->removeColumns((position-1), 1);
        }
    }
    else{
        Erreur instance;
        instance.Erreur_msg(tr("Opération illégale dans la modification de tableau!"), QMessageBox::Warning);
        return;
    }

    return;
}

//Affiche le fichier courant au mode texte OU html
void DadaWord::to_plain_text(){

    if(to_text->isChecked() && find_edit()->acceptRichText()){
        QString contenu = find_edit()->toHtml();
        find_edit()->clear();
        find_edit()->setPlainText(contenu);
        instance = new HighlighterHtml(find_edit()->document());
        //On refuse le RichText sous peine de bug lors du changement de focus
        find_edit()->setAcceptRichText(false);
    }
    else if(to_text->isChecked() && !find_edit()->acceptRichText()){
        to_text->setChecked(false);
    }
    else if(find_onglet()->accessibleName().contains(".ddw") || find_onglet()->accessibleName().contains(".htm") || find_onglet()->accessibleName().contains(".html") || find_onglet()->accessibleName().contains(".odt")){
        QString contenu = find_edit()->toPlainText();
        find_edit()->clear();
        //On réactive le RichText
        find_edit()->setAcceptRichText(true);
        find_edit()->setHtml(contenu);
    }
    else{
        //Si on arrive ici, c'est qu'un boulet a essayé de passer un TXT en mode RichText
        //On recoche le bouton, par prudence
        to_text->setChecked(true);
    }
}

//Fonction qui gère l'affichage (ou non) des toolbars
void DadaWord::hide_toolbar(const int identifiant){
    switch(identifiant){
    case DEFAULT:
        //On récupère l'état du bouton
        if(affichage_default->isChecked()){
            barre_standard->show();
        }
        else{
            barre_standard->hide();
        }
        break;
    case PUCES:
        //On récupère l'état du bouton (encore)
        if(affichage_puces->isChecked()){
            puces->show();
        }
        else{
            puces->hide();
        }
        break;
    case TABLEAUX:
        //Et c'est parti pour les tableaux (je me demande pourquoi je commente ça, c'est évident!)
        if(affichage_tableau->isChecked()){
            barre_tableau->show();
        }
        else{
            barre_tableau->hide();
        }
        break;
    case RECHERCHE:
        if(affichage_recherche->isChecked()){
            barre_recherche->show();
        }
        else{
            barre_recherche->hide();
        }
        break;
    case FORMAT:
        if(affichage_format->isChecked()){
            bar_format->show();
        }
        else{
            bar_format->hide();
        }
        break;
    default:
        Erreur instance;
        instance.Erreur_msg(tr("Exception lors de la gestion de la visibilité des barres d'outils"), QMessageBox::Information);
        return;
        break;
    }

    return;
}

//Affiche (ou non) DadaWord en Full Screen
void DadaWord::make_full_screen(){
    if(!this->isFullScreen() && full_screen->isChecked()){
        this->setWindowState(Qt::WindowFullScreen);
    }
    else if(this->isFullScreen() && !full_screen->isChecked()){
        this->setWindowState(Qt::WindowActive);
    }
    else{
        //Bug, on ne peut pas être ici
        Erreur instance;
        instance.Erreur_msg(tr("Une erreur est survenue lors de la gestion du fullscreen. L'état de la fenêtre et du menu ne sont pas en adéquation -> skipping"), QMessageBox::Information);
    }
    return;
}

//Slot de recherche
void DadaWord::recherche(bool remplacer){
    dialog_recherche = new QDialog;
    dialog_recherche->setWindowModality(Qt::ApplicationModal);

    if(remplacer){
        dialog_recherche->setWindowTitle(tr("Remplacer dans …"));
    }
    else{
        dialog_recherche->setWindowTitle(tr("Rechercher dans …"));
    }
    dialog_recherche->setWindowIcon(QIcon(":/menus/images/search.png"));
    dialog_recherche->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout *layout = new QGridLayout;
    QLabel *titre;
    if(remplacer){
        titre = new QLabel(tr("<h2>Remplacer dans le document courant</h2>"));
    }
    else{
        titre = new QLabel(tr("<h2>Rechercher dans le document courant</h2>"));
    }
    QLabel *question = new QLabel(tr("Phrase à rechecher :"));
    QLabel *case_sensitive = new QLabel(tr("Sensible à la casse"));
    QLabel *mots_entiers = new QLabel(tr("Uniquement les mots entiers"));

    champ_recherche2 = new QLineEdit;
    checkbox_case = new QCheckBox;
    checkbox_mots = new QCheckBox;
    if(!champ_recherche->text().isNull() || !champ_recherche->text().isEmpty()){
        champ_recherche2->setText(champ_recherche->text());
    }

    QPushButton *bouton_recherche = new QPushButton;
    QPushButton *bouton_annuler = new QPushButton;
    bouton_recherche->setIcon(QIcon(":/menus/images/search.png"));
    bouton_recherche->setText(tr("Rechercher"));
    bouton_annuler->setIcon(QIcon(":/menus/images/sortir.png"));
    bouton_annuler->setText(tr("Annuler"));

    //On rentre tout dans le layout
    layout->addWidget(titre, 0, 0, 1, 4, Qt::AlignHCenter);
    layout->addWidget(question, 1, 0, 1, 1);
    layout->addWidget(champ_recherche2, 1, 1, 1, 4, Qt::AlignHCenter);
    layout->addWidget(case_sensitive, 2, 0);
    layout->addWidget(checkbox_case, 2, 1);
    layout->addWidget(mots_entiers, 2, 2);
    layout->addWidget(checkbox_mots, 2, 3);
    layout->addWidget(bouton_recherche, 4, 0, 1, 2);
    layout->addWidget(bouton_annuler, 4, 2, 1, 2);

    if(remplacer){
        QLabel *label_remplace = new QLabel(tr("Remplacer par"));
        le_remplace = new QLineEdit;
        layout->addWidget(label_remplace, 3, 0);
        layout->addWidget(le_remplace, 3, 1, 1, 4, Qt::AlignHCenter);
        bouton_recherche->setText(tr("Remplacer tout"));

        //On connecte le bouton
        connect(bouton_recherche, SIGNAL(clicked()), this, SLOT(remplace_all()));
    }

    //Connects
    connect(bouton_annuler, SIGNAL(clicked()), dialog_recherche, SLOT(close()));
    QSignalMapper *mappeur_x = new QSignalMapper;
    connect(bouton_recherche, SIGNAL(clicked()), mappeur_x, SLOT(map()));
    mappeur_x->setMapping(bouton_recherche, 196);//Bug si on passe "FENETRE" -> je passe un int absurde en attendant de trouver le bug
    connect(mappeur_x, SIGNAL(mapped(const int)), this, SLOT(make_search(const int)));

    dialog_recherche->setLayout(layout);
    dialog_recherche->show();

    return;
}

//Effectue la recherche
void DadaWord::make_search(const int from){
    //Initialisations
    bool rebours = false;
    bool resultat = false;
    bool fenetre = false;
    QTextEdit::ExtraSelection es;
    QTextCharFormat surligne;
    Outils instance_outils;
    surligne.setBackground(QBrush(QColor("#ff6060")));
    surligne.setForeground(QBrush(QColor("#000000")));

    //---------------------------------------------
    //Actions de la QToolBar
    //---------------------------------------------
    if(from == QTOOLBAR || from == GAUCHE || from == DROITE){//Pour les directionnelles, ça revient au même
        if(champ_recherche->text().isEmpty() || champ_recherche->text().isNull()){
            Erreur instance;
            instance.Erreur_msg(tr("Tentative de recheche à partir d'une chaine vide"), QMessageBox::Ignore);
            return;
        }
        if(from == GAUCHE){
            //Si on souhaite faire une recherche en remontant dans les occurences, faut le signaler par un flag
            rebours = true;
        }
    }
    else if(from == 196){
        //On vient de la fenêtre de recherche
        fenetre = true;
    }
    else{
        Erreur instance;
        instance.Erreur_msg(tr("Provenance de recherche non identifiée"), QMessageBox::Information);
    }

    //Préparation des curseurs
    QTextCursor cursor(find_edit()->textCursor());
    cursor.movePosition(QTextCursor::Start);
    QTextCursor newCursor(find_edit()->document());

    //On regarde si on est déjà en train de vérifier:
    if(!pos_recherche.isNull() && !pos_recherche.atStart()){
        pos_recherche.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
        newCursor = pos_recherche;
    }

    //-----------------------
    //Recherche
    //-----------------------
    while (!newCursor.isNull() && !newCursor.atEnd()){
        if(!fenetre){
            if(!rebours){
                newCursor = find_edit()->document()->find(champ_recherche->text(), newCursor);
            }
            else{
                newCursor = find_edit()->document()->find(champ_recherche->text(), newCursor, QTextDocument::FindBackward);
            }
        }
        else{//On vient de la fenêtre de recherche
            if(checkbox_case->isChecked() && checkbox_mots->isChecked()){
                newCursor = find_edit()->document()->find(champ_recherche2->text(), newCursor, QTextDocument::FindCaseSensitively | QTextDocument::FindWholeWords);
            }
            else if(checkbox_case->isChecked() && !checkbox_mots->isChecked()){
                newCursor = find_edit()->document()->find(champ_recherche2->text(), newCursor, QTextDocument::FindCaseSensitively);
            }
            else if(!checkbox_case->isChecked() && checkbox_mots->isChecked()){
                newCursor = find_edit()->document()->find(champ_recherche2->text(), newCursor, QTextDocument::FindWholeWords);
            }
            else{
                //Les flags sont OK. -> Recherche simple
                newCursor = find_edit()->document()->find(champ_recherche2->text(), newCursor);
            }
        }
        if (!newCursor.isNull()){
            newCursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
            QString word = newCursor.selectedText();

            //Nettoyage de la ponctuation APRES le mot
            while(!word.isEmpty() && !word.at(word.size()-1).isLetter()) {
                int cursorPos = newCursor.position();
                newCursor.setPosition(newCursor.position() - 1, QTextCursor::MoveAnchor);
                newCursor.setPosition((cursorPos-word.size()), QTextCursor::KeepAnchor);
                word = newCursor.selectedText();
                if(!word.at(word.size()-1).isLetter()){
                    //Si on doit refaire un tour de boucle, on rebouge le curseur
                    newCursor.setPosition(newCursor.anchor());
                }
            }

            //On surligne le mot
            es.cursor = newCursor;
            es.format = surligne;

            QList<QTextEdit::ExtraSelection> esList;
            esList << es;
            find_edit()->setExtraSelections(esList);

            resultat = true;
            break;
        }
    }

    //-------------------------------------------
    //Gestion du curseur post-recherche
    //-------------------------------------------
    if(newCursor.atEnd()){
        pos_recherche.movePosition(QTextCursor::Start);
        if(instance_outils.lire_config("alertes").toInt() == HIGH && !resultat){
            QMessageBox::information(this, tr("Fin du document"), tr("Le curseur a atteint la fin du document sans trouver de résultat"));
        }
    }
    else{
        pos_recherche = newCursor;
    }

    //-----------------------------------------------------
    //Action selon le résultat (ou l'absence de résultat
    //------------------------------------------------------
    if(!resultat){
        QPalette couleur = champ_recherche->palette();
        couleur.setColor(champ_recherche->backgroundRole(), Qt::red);
        champ_recherche->setPalette(couleur);

        //Avant de partir, on transfère les résultats si on vient de la fenêtre
        if(fenetre){
            if(instance_outils.lire_config("alertes").toInt() == HIGH || instance_outils.lire_config("alertes").toInt() == MEDIUM){
                QMessageBox::warning(this, tr("Recherche infructueuse"), tr("Malheureusement, votre recheche n'a donné aucun résultat.\nPeut-être devriez-vous essayer avec des critères moins stricts?"));
            }
            //On ferme tout
            dialog_recherche->close();
            delete dialog_recherche;
            fenetre = false;
        }
    }
    else{
        QPalette couleur = champ_recherche->palette();
        couleur.setColor(champ_recherche->backgroundRole(), Qt::green);
        champ_recherche->setPalette(couleur);

        //Avant de partir, on transfère les résultats si on vient de la fenêtre
        if(fenetre){
            champ_recherche->setText(champ_recherche2->text());
            //Et on affiche la barre de recherche
            hide_searchbar(true);
            //On ferme tout
            dialog_recherche->close();
            delete dialog_recherche;
            fenetre = false;
        }
    }

    return;
}

//On affiche ou cache la barre de recheche
void DadaWord::hide_searchbar(bool transfert){
    if(!transfert){
        //C'est normal, on ferme la barre des tâches. Pour ça, il suffit de décoche le menu et d'appelle la fonction qui gère l'affichage des barres de tâches
        affichage_recherche->setChecked(false);
        hide_toolbar(RECHERCHE);
    }
    else{
        //On vient du QDialog -> on affiche la barre de recherche
        affichage_recherche->setChecked(true);
        hide_toolbar(RECHERCHE);
    }
    return;
}

//Statistiques du document
void DadaWord::statistiques(){
    //Nouvelle fenêtre
    QDialog *fen_stats = new QDialog;

    //Initialisations
    QLabel *titre, *txt_all, *lignes_all, *mots_all, *carac_all, *carac_full_all;
    QTextCursor curseur = find_edit()->textCursor();
    QLabel *contenu_lignes_select, *contenu_mots_select, *contenu_carac_select, *contenu_full_select, *txt_select;
    QLabel *contenu_lignes_all, *contenu_mots_all, *contenu_carac_all, *contenu_full_all;
    QGridLayout *layout;
    QPushButton *quitter;

    //Instanciations
    titre = new QLabel(tr("<h1>Statistiques\n</h1>")); //Retour
    txt_all = new QLabel(tr("<b>Document actif:</b>"));
    lignes_all = new QLabel(tr("Lignes:"));
    mots_all = new QLabel(tr("Mots:"));
    carac_all = new QLabel(tr("Caractères\n(espaces exclus)"));
    carac_full_all = new QLabel(tr("Caractères totaux:"));
    quitter = new QPushButton(tr("Quitter"));
    layout = new QGridLayout;
    contenu_lignes_all = new QLabel(QString::number(find_edit()->document()->lineCount()));
    //Comptage de mots
    int taille;
    QString chaine = find_edit()->toPlainText();
    chaine = chaine.trimmed();
    do {
        chaine = chaine.replace("  ", " ");
    } while (chaine.contains("  "));
    taille = chaine.count(" ");
    taille += 1;
    contenu_mots_all = new QLabel(QString::number(taille));
    contenu_carac_all = new QLabel(QString::number(find_edit()->toPlainText().remove(" ").length()));
    contenu_full_all = new QLabel(QString::number(find_edit()->toPlainText().length()));

    //Caractéristiques supplémentaires
    fen_stats->setAttribute(Qt::WA_DeleteOnClose);
    fen_stats->setWindowModality(Qt::ApplicationModal);
    fen_stats->setWindowTitle(tr("Statistiques du document courant"));
    quitter->setIcon(QIcon(":/menus/images/exit.png"));
    connect(quitter, SIGNAL(clicked()), fen_stats, SLOT(close()));
    fen_stats->setLayout(layout);

    if(curseur.hasSelection()){
        txt_select = new QLabel(tr("<b>Sélection:</b>"));
        contenu_carac_select = new QLabel(QString::number(curseur.selectedText().remove(" ").length()));
        contenu_full_select = new QLabel(QString::number(curseur.selectedText().length()));
        taille = 0;
        chaine = curseur.selectedText();
        chaine = chaine.trimmed();
        do {
            chaine = chaine.replace("  ", " ");
        } while (chaine.contains("  "));
        taille = chaine.count(" ");
        taille += 1;
        contenu_mots_select = new QLabel(QString::number(taille));
        contenu_lignes_select = new QLabel(tr("N/A"));

        //On met dans le layout
        layout->addWidget(txt_select, 1, 2);
        layout->addWidget(contenu_lignes_select, 2, 2, 1, 1, Qt::AlignRight);
        layout->addWidget(contenu_mots_select, 3, 2, 1, 1, Qt::AlignRight);
        layout->addWidget(contenu_carac_select, 4, 2, 1, 1, Qt::AlignRight);
        layout->addWidget(contenu_full_select, 5, 2, 1, 1, Qt::AlignRight);
    }

    //Stockage dans le layout
    layout->addWidget(titre, 0, 0, 1, 3, Qt::AlignHCenter);
    layout->addWidget(txt_all, 1, 1);
    layout->addWidget(lignes_all, 2, 0);
    layout->addWidget(contenu_lignes_all, 2, 1, 1, 1, Qt::AlignHCenter);
    layout->addWidget(mots_all, 3, 0);
    layout->addWidget(contenu_mots_all, 3, 1, 1, 1, Qt::AlignHCenter);
    layout->addWidget(carac_all, 4, 0);
    layout->addWidget(contenu_carac_all, 4, 1, 1, 1, Qt::AlignHCenter);
    layout->addWidget(carac_full_all, 5, 0);
    layout->addWidget(contenu_full_all, 5, 1, 1, 1, Qt::AlignHCenter);
    layout->addWidget(quitter, 6, 0, 1, 3, Qt::AlignHCenter);

    //On affiche
    fen_stats->show();

    return;
}

//Change le format si sélection depuis la Toolbar
void DadaWord::change_style(int style){
    //Préparation des variables
    QSettings settings("Dadaword", "dadaword");
    QStringList nom_styles = settings.value("noms_styles").toStringList();
    QTextCursor curseur = find_edit()->textCursor();
    QTextCharFormat format;

    //--------------------------------
    //Application du style
    //--------------------------------

    //Pas de sélection
    if(!curseur.hasSelection()){
        settings.beginGroup(nom_styles.at(style));
        find_edit()->setCurrentFont(settings.value("police").value<QFont>());
        find_edit()->setFontPointSize(settings.value("taille").toInt());
        find_edit()->setFontUnderline(settings.value("souligne").toBool());
        find_edit()->setFontItalic(settings.value("italique").toBool());
        find_edit()->setFontWeight(settings.value("gras").toInt());
        find_edit()->setTextColor(settings.value("foreground").value<QColor>());
        if(settings.value("background").value<QColor>() != Qt::transparent){
            find_edit()->setTextBackgroundColor(settings.value("background").value<QColor>());
        }
        souligne->setChecked(settings.value("souligne").toBool());
        italique->setChecked(settings.value("italique").toBool());
        if(settings.value("gras").toInt() == QFont::Bold){
            gras->setChecked(true);
        }
        else{
            gras->setChecked(false);
        }
        settings.endGroup();
    }
    //Sélection
    else{
        settings.beginGroup(nom_styles.at(style));
        format.setFont(settings.value("police").value<QFont>());
        format.setFontPointSize(settings.value("taille").toInt());
        format.setFontUnderline(settings.value("souligne").toBool());
        format.setFontItalic(settings.value("italique").toBool());
        format.setFontWeight(settings.value("gras").toInt());
        if(settings.value("background").value<QColor>() != Qt::transparent){
            format.setBackground(QBrush(settings.value("background").value<QColor>()));
        }
        format.setForeground(QBrush(settings.value("foreground").value<QColor>()));
        curseur.setCharFormat(format);
    }

    //On rend le focus au QTextEdit
    find_edit()->setFocus();
    return;
}

//Orthographe : slot principal de vérification orthographique
void DadaWord::verif_orthographe(){
    QString userDict= QDir::homePath() + "/.config/libreoffice/3/user/wordbook/standard.dic";
    if(!QFile::exists(userDict)){
        userDict = QDir::homePath() + ".dadaword/perso.dic";
    }
    SpellChecker *spellChecker = new SpellChecker(dictPath, userDict);

    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QBrush(QColor("#ff6060")));
    highlightFormat.setForeground(QBrush(QColor("#000000")));

    // save the position of the current cursor
    QTextCursor oldCursor = find_edit()->textCursor();

    // create a new cursor to walk through the text
    QTextCursor cursor(find_edit()->document());

    //Sélection de texte (pour le surlignage)
    QTextEdit::ExtraSelection es;

    //On regarde si on est déjà en train de vérifier:
    if(!pos_orth.isNull() && !pos_orth.atStart()){
        pos_orth.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
        cursor = pos_orth;
    }


    //Booléen d'erreur qui stope la boucle
    bool erreur = false;

    while(!cursor.atEnd() && !erreur) {
        QCoreApplication::processEvents();
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
        QString word = cursor.selectedText();

        // Workaround for better recognition of words
        // punctuation etc. does not belong to words
        while(!word.isEmpty() && !word.at(0).isLetter() && cursor.anchor() < cursor.position()) {
            int cursorPos = cursor.position();
            cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
            cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
            word = cursor.selectedText();
        }

        //Nettoyage des éléments qui suivent
        while(!word.isEmpty() && !word.at(word.size()-1).isLetter()){
            word = word.remove(word.size()-1, word.size());
        }

        //Il n'y a erreur QUE si le mot n'est pas vide, n'est pas présent dans le dictionnaire ET n'est pas ignoré
        if(!word.isEmpty() && !spellChecker->spell(word) && !list_skip.contains(word)){
            //Affichage de la barre d'outils,
            barre_orthographe->show();

            QTextCursor tmpCursor(cursor);
            tmpCursor.setPosition(cursor.anchor());

            //On met à jour le mot dans la toolbar
            orth_mot->setText("<b>"+word+"</b>");
            orth_mot->setAlignment(Qt::AlignHCenter);
            //On nettoye la QComboBox
            orth_suggest->clear();

            // highlight the unknown word
            es.cursor = cursor;
            es.format = highlightFormat;

            QList<QTextEdit::ExtraSelection> esList;
            esList << es;
            find_edit()->setExtraSelections(esList);
            QCoreApplication::processEvents();

            //On met à jour la liste de suggestions
            QStringList suggestions = spellChecker->suggest(word);
            orth_suggest->setEnabled(true);
            for(int i=0; i<suggestions.size(); i++){
                orth_suggest->addItem(suggestions.at(i));
            }
            if(suggestions.size() == 0){
                //Vu qu'il n'y a rien, on l'indique
                orth_suggest->addItem(tr("Aucune suggestion"));
                orth_suggest->setEnabled(false);
            }

            //On coupe la boucle
            erreur = true;
            //Transfent de variable en cas de besoin pour le dico
            orth_erreur = word;
        }
        //On met à jour le curseur
        pos_orth = cursor;
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
    }

    find_edit()->setTextCursor(oldCursor);
    if(!erreur){//Si "erreur" est faux, c'est qu'on a atteint la fin du document
        Outils instance_outils;
        if(instance_outils.lire_config("alertes").toInt() == HIGH){
            QMessageBox::information(this, tr("Terminé"), tr("La vérification orthographique est terminée."));
        }

        //Masquage de la barre d'outils
        barre_orthographe->hide();

        //Et on efface le surlignage
        QTextCharFormat highlightFormat;
        highlightFormat.setBackground(QBrush(QColor("#ffffff")));
        QTextEdit::ExtraSelection ess;
        QList<QTextEdit::ExtraSelection> essList;
        essList << ess;
        find_edit()->setExtraSelections(essList);
        QCoreApplication::processEvents();
        ess.format = highlightFormat;

        //On efface pos_orth
        pos_orth.movePosition(QTextCursor::Start);
    }

    return;
}

//Orthographe : ignorer toutes les occurences du mot
void DadaWord::orth_ignore(){
    //On ajoute à la liste des mots à ignorer
    list_skip.append(orth_erreur);
    //Et on poursuit notre petit chemin
    if(!pos_orth.isNull() && !pos_orth.atStart()){ //Si on ne vient pas du menu contextuel, on continue
        verif_orthographe();
    }
    return;
}

//Orthographe : ajouter au dictionnaire
void DadaWord::orth_dico(){
    if(!orth_erreur.isEmpty() && !orth_erreur.isNull()){
        QString userDict= QDir::homePath() + "/.config/libreoffice/3/user/wordbook/standard.dic";
        if(!QFile::exists(userDict)){
            userDict = QDir::homePath() + ".dadaword/perso.dic";
        }
        SpellChecker instance_dico(dictPath, userDict);
        instance_dico.addToUserWordlist(orth_erreur);
        pos_orth.movePosition(QTextCursor::NextWord);
        verif_orthographe();

        return;
    }
}

//Orthographe : remplacer
void DadaWord::orth_remplace(QString mot){
    QTextCursor temp;
    bool clic = false;
    if(pos_orth_menu.isNull() || !pos_orth_menu.hasSelection()){
    temp = pos_orth;
    }
    else{
        temp = pos_orth_menu;
        pos_orth_menu.clearSelection();
        clic = true;
    }

    QString word = temp.selectedText();
    while(!word.isEmpty() && !word.at(0).isLetter() && temp.anchor() < temp.position()) {
        int cursorPos = temp.position();
        temp.setPosition(temp.anchor() + 1, QTextCursor::MoveAnchor);
        temp.setPosition(cursorPos, QTextCursor::KeepAnchor);
        word = temp.selectedText();
    }
    temp.removeSelectedText();
    temp.insertText(mot);
    temp.movePosition(QTextCursor::NextWord);
    if(!clic){
        verif_orthographe();
    }
    return;
}

//Orthographe : remplacer tout
void DadaWord::orth_remplace_all(QString remplace){
    int nb_remplacements = 0;
    QComboBox *select_words = new QComboBox;
    QDialog *mots = new QDialog;
    if(remplace.isEmpty()){
        QString userDict= QDir::homePath() + "/.config/libreoffice/3/user/wordbook/standard.dic";
        if(!QFile::exists(userDict)){
            userDict = QDir::homePath() + ".dadaword/perso.dic";
        }
        SpellChecker instance_dico(dictPath, userDict);
        QStringList suggestions = instance_dico.suggest(orth_erreur);
        for(int i=0; i<suggestions.size(); i++){
            select_words->addItem(suggestions.at(i));
        }
        if(suggestions.size() == 0){
            QMessageBox::information(this, tr("Impossible de remplacer"), tr("Aucun choix de remplacement n'a pu être trouvé"));
            return;
        }
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(select_words);
        QPushButton *ok = new QPushButton(tr("Valider"));
        ok->setIcon(QIcon(":/menus/images/ok.png"));
        connect(ok, SIGNAL(clicked()), mots, SLOT(close()));
        layout->addWidget(ok, 0, Qt::AlignHCenter);
        mots->setLayout(layout);
        mots->exec();
        QCoreApplication::processEvents();
    }
    // save the position of the current cursor
    QTextCursor oldCursor = find_edit()->textCursor();

    // create a new cursor to walk through the text
    QTextCursor cursor(find_edit()->document());

    //Parcours de tout le document
    while(!cursor.atEnd()) {
        QCoreApplication::processEvents();
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);
        QString word = cursor.selectedText();

        //Nettoyage des éléments qui précèdent
        while(!word.isEmpty() && !word.at(0).isLetter() && cursor.anchor() < cursor.position()) {
            int cursorPos = cursor.position();
            cursor.setPosition(cursor.anchor() + 1, QTextCursor::MoveAnchor);
            cursor.setPosition(cursorPos, QTextCursor::KeepAnchor);
            word = cursor.selectedText();
        }

        //Nettoyage des éléments qui suivent
        while(!word.isEmpty() && !word.at(word.size()-1).isLetter()){
            word = word.remove(word.size()-1, word.size());
        }

        //Remplacement du mot
        if(word == orth_erreur){
            cursor.removeSelectedText();
            if(remplace.isEmpty()){
                cursor.insertText(select_words->currentText());
            }
            else{
                cursor.insertText(remplace);
            }
            nb_remplacements++; //On incrémente le nombre de remplacements
        }

        //On met à jour le curseur
        cursor.movePosition(QTextCursor::NextWord, QTextCursor::MoveAnchor, 1);
    }

    //On remet en place le vieux curseur
    find_edit()->setTextCursor(oldCursor);

    delete mots;

    //On affiche le nombre de remplacements
    Outils instance_outils;
    if(instance_outils.lire_config("alertes").toInt() == HIGH){
        QMessageBox::information(this, "Remplacement terminé", QString("Le mot «%1» a été remplacé %2 fois").arg(orth_erreur).arg(nb_remplacements));
    }

    //Return
    return;
}

//Changement de la langue de vérification
void DadaWord::orth_langue(){
    QDialog *fen = new QDialog;
    fen->setWindowTitle(tr("Langue du correcteur"));
    fen->setWindowModality(Qt::ApplicationModal);
    QLabel *titre, *actuelle, *choix;
    QPushButton *valider = new QPushButton(tr("Valider"));
    QComboBox *liste = new QComboBox;
    titre = new QLabel("<h1>Langue du correcteur<h1>");
    QString nom_dico = dictPath.split("/").last();;
    actuelle = new QLabel(tr("Dictionnaire actuel : ")+nom_dico);
    choix = new QLabel(tr("Nouvelle langue"));
    QDir dossier;
    dossier.setPath("/usr/share/hunspell");
    QStringList extentions;
    extentions << "*.dic";
    QStringList liste_dicos = dossier.entryList(extentions);
    for(int i=0; i<liste_dicos.size(); i++){
        QString temp = liste_dicos.at(i);
        temp.resize((temp.size()-4));
        liste->addItem(temp);
        //On présélectionne la langue actuelle
        if(temp == nom_dico){
            liste->setCurrentIndex(i);
        }
    }
    valider->setIcon(QIcon(":/menus/images/ok.png"));
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(titre, 0, 0, 1, 2, Qt::AlignHCenter);
    layout->addWidget(actuelle, 1, 0);
    layout->addWidget(choix, 2, 0);
    layout->addWidget(liste, 2, 1);
    layout->addWidget(valider, 3, 0, 1, 2, Qt::AlignHCenter);
    connect(valider, SIGNAL(clicked()), fen, SLOT(close()));
    fen->setLayout(layout);
    fen->exec();
    QCoreApplication::processEvents();

    //On met à jour le dico
    dictPath = "/usr/share/hunspell/"+liste->currentText()+".dic";
    return;
}

//Arrêt de la vérification orthographique
void DadaWord::orth_stop(){
    //On remet le curseur au début pour ne pas avoir de bug
    pos_orth.movePosition(QTextCursor::Start);

    //On masque la couleur résiduelle
    //Et on efface le surlignage
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(QBrush(QColor("#ffffff")));
    QTextEdit::ExtraSelection ess;
    QList<QTextEdit::ExtraSelection> essList;
    essList << ess;
    find_edit()->setExtraSelections(essList);
    QCoreApplication::processEvents();
    ess.format = highlightFormat;

    //Et on ferme la barre des tâches
    barre_orthographe->hide();

    //Et c'est tout!
    return;
}

//Couper
void DadaWord::couper(){
    find_edit()->cut();
    return;
}

//Copier
void DadaWord::copier(){
    find_edit()->copy();
    return;
}

//Coller
void DadaWord::coller(){
    find_edit()->paste();
    return;
}

//Mode de sur-écriture
void DadaWord::mode_surecriture(){
    bool etat = find_edit()->overwriteMode();
    //Vu qu'il y a eu clic, on inverse le statut
    if(etat){
        find_edit()->setOverwriteMode(false);
        status_surecriture->setText("INS");
    }
    else{
        find_edit()->setOverwriteMode(true);
        status_surecriture->setText(tr("RFP")); //"RFP" pour "Refrappe"
    }
    return;
}

//Bouton de coloration syntaxique
void DadaWord::html_highlight(){
    //Si on est ici, c'est que l'utilisateur a cliqué sur le bouton, donc que c'est possible de colorer (sinon le bouton n'apparaîtrait pas)
    if(colore_html->isChecked()){
        instance = new HighlighterHtml(find_edit()->document());
    }
    else{
        find_edit()->setTextColor(Qt::black);
        delete instance;
    }
    return;
}

//Menu personnalisé
void DadaWord::affiche_menu_perso(const QPoint &position){
    //Vérification de l'éditeur
    QTextEdit *editor = qobject_cast<QTextEdit *>(sender());
    if(!editor){
        return;
    }

    //Création du menu
    QMenu *menu_contextuel = new QMenu;

    //On récupère le mot sous le curseur
    QTextCursor cursor = editor->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    QString mot  = cursor.selectedText();

    QString userDict= QDir::homePath() + "/.config/libreoffice/3/user/wordbook/standard.dic";
    if(!QFile::exists(userDict)){
        userDict = QDir::homePath() + ".dadaword/perso.dic";
    }
    SpellChecker instance_orthographe(dictPath, userDict);
    QStringList propositions;
    if(!mot.isEmpty() && !instance_orthographe.spell(mot) && !list_skip.contains(mot)){
        propositions = instance_orthographe.suggest(mot);
        orth_erreur = mot;
        //On parcourt la boucle
        for(int i=0; i<propositions.size(); i++){
            menu_contextuel->addAction(propositions.at(i));
        }
        //S'il y a des propositions, on ajoute un séparateur
        menu_contextuel->addSeparator();
        //On ajoute "Ignorer" et "Ajouter au dictionnaire" au menu
        menu_contextuel->addAction(tr("Ignorer tout"), this, SLOT(orth_ignore()));
        menu_contextuel->addAction(tr("Ajouter au dictionnaire"), this, SLOT(orth_dico()));
        //Et on ré-ajoute un séparateur
        menu_contextuel->addSeparator();
    }

    //Contenu du menu
    menu_contextuel->addAction(tr("Couper"), this, SLOT(couper()), QKeySequence("Ctrl+X"));
    menu_contextuel->addAction(tr("Copier"), this, SLOT(copier()), QKeySequence("Ctrl+C"));
    menu_contextuel->addAction(tr("Coller"), this, SLOT(coller()), QKeySequence("Ctrl+V"));
    QAction *choix = menu_contextuel->exec(this->cursor().pos());

    if(choix){
        if((propositions.contains(choix->text())) && !choix->isSeparator()){
            //On veut changer un mot
            //On transfère le QTextCursor
            pos_orth_menu = cursor;
            orth_remplace(choix->text());
        }
    }
    delete menu_contextuel;
    return;
}

//Recharge les styles si on en a rajouté un (ou supprimé un)
void DadaWord::recharge_styles(){
    nom_format->clear();
    QSettings settings("Dadaword", "dadaword");
    QStringList noms_styles = settings.value("noms_styles").toStringList();
    for(int i=0; i<noms_styles.count(); i++){
        nom_format->addItem(noms_styles.at(i));
    }
    return;
}

//Remplace toutes les occurences d'un mot
void DadaWord::remplace_all(){
    //On met à jour les paramètres
    orth_erreur = champ_recherche2->text();

    //On appelle la fonction
    orth_remplace_all(le_remplace->text());

    //On ferme la fenêtre
    dialog_recherche->close();
    delete dialog_recherche;
}

//Appelle la fonction de recherche
void DadaWord::call_remplace(){
    recherche(true);
    return;
}

//Gére le déplacement du curseur
void DadaWord::curseur_change(){
    //On récupère le curseur
    QTextCursor curseur = find_edit()->textCursor();

    //Si le curseur est à la fin, on ne fait rien
    if(curseur.atEnd()){
        return;
    }
    else{
        //On regrade les caractères
        curseur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        //On récupére le format
        QTextCharFormat format = curseur.charFormat();
        //On adapte les boutons selon le format
        if(format.fontWeight() == QFont::Bold && !gras->isChecked()){
            gras->setChecked(true);
        }
        if(format.fontWeight() == QFont::Normal && gras->isChecked()){
            gras->setChecked(false);
        }
        if(format.fontItalic() != italique->isChecked()){
            italique->setChecked(format.fontItalic());
        }
        if(format.fontUnderline() != souligne->isChecked()){
            souligne->setChecked(format.fontUnderline());
        }

    }
    return;
}
