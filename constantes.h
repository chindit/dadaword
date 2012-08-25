/*
  Développeur : David Lumaye (littletiger58@gmail.com)
  Date : 01/08/12
  Ce code est concédé sous licence GPL v3 (texte fourni avec le programme).
  Merci de ne pas supprimer cette notice.
  */

#ifndef CONSTANTES_H
#define CONSTANTES_H

#define VERSION "1.1.2"

//Nombre de lignes à laisser dans le fichier de debug lors du délestage
#define ITEMS_LOG 10

//Marge à afficher pour les document de type "word"
#define MARGIN_WORD 60
#define TAMPON_WORD 55

//Énumérations pour le changement de couleur
enum { TEXTE, SURLIGNE };

//Énumérations pour les alertes
enum { LOW, MEDIUM, HIGH };

//Énumérations pour les tableaux
enum { COLL, ROW };

//Énumérations pour les toolbars
enum { DEFAULT, PUCES, TABLEAUX, RECHERCHE, FORMAT };

//Énumérations pour la recherche
enum { FENETRE, QTOOLBAR };

//Énumérations our le sens de la recherche
enum { GAUCHE, DROITE };

#endif // CONSTANTES_H
