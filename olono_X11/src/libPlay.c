/*  This file is part of Olono
    Copyright (C) 2008 Martin Potier (<mpotier@isep.fr>) and
                       David Wagner  (<dwagner@isep.fr>)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; version 3
    of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

/** \file
 * libPlay.c
 * Librairie indépendante du mode d'affichage.
 * Elle gère toutes les règles du jeu et s'occupe d'effectuer les mouvements
 * comme les retournements de pions.
 */

#include <stdio.h>
#include <stdlib.h>
#include "libPlay.h"
#include "libDefine.h"
#include "libDisplay.h"
#include "libIA.h"

static int elaguer(NOEUD * noeud);

static NOEUD tronc; // Le plateau d'origine
static NOEUD * actuel; // Pointe en permanence sur le plateau en cours

extern int taillePlateau;

//! Renvoie 0 si le coup n'est pas valide, le nombre de pièces qu'il retourne sinon.
/**
 * Cette fonction renvoie le nombre de pièces à retourner et met les
 * coordonnées des pièces à retourner dans le pointeur listeCoups.
 *
 * La liste increments contient les vecteurs selon lesquels parcourir le
 * tableau: on suit toutes les droites et diagonales (première boucle for).
 * Au début de chaque vecteur, on vérifie qu'on reste dans la plateau, que
 * la case immédiatement n'est ni vide ni de la couleur du joueur. Dans le
 * cas contraire, on passe immédiatement au vecteur suivant.
 * Une fois ces prérequis vérifiés, on parcourt la suite du vecteur à la
 * recherche d'une pierre amie (le while). On met les coordonnées des pions
 * qui se trouvent sur le chemin. Si on trouve effectivement une pierre amie,
 * le compteur total de pion à retourner est incrémenté.
 * coupsTemp permet, si on ne rencontre pas d'amie, de ne pas fausser le compte
 * de coups
 *
 * A tout moment, si on rencontre un joker, on le saute, c'est à dire qu'on
 * fait un continue.
*/
char lister(int x,int y,char couleur,CASE *plateau[], int listeCoups[])
{
    CASE caseDepart;
    char increments[8][2] =
    {
        {-1, -1},
        {-1,  0},
        {-1,  1},
        { 0,  1},
        { 1,  1},
        { 1,  0},
        { 1, -1},
        { 0, -1}
    }; // Liste des vecteurs
    int inc=0; // Parcourt les vecteurs
    int coups=0, coupsTemp; // Compteur de nombre de pions retournés
    char rencontre = 0;

    caseDepart = *adresseParXY(x, y, plateau);

    if (caseDepart.couleur != VIDE)
        return 0;

    // On parcourt les vecteurs
    for (inc=0; inc<8; inc++)
    {
        char incX=increments[inc][0], incY=increments[inc][1];
        char couleurCase;
        CASE * caseTemp;

        caseTemp = caseSuivante(caseDepart, incX, incY, plateau);
        // Si on est au bord du plateau ou s'il y a eu une erreur
        if (caseTemp == NULL)
            continue;
        // Si on tombe ensuite sur une case de la meme couleur ou vide, on saute
        if ( (caseTemp->couleur == couleur) || (caseTemp->couleur == VIDE) )
            continue;

        // Dans les autres cas, on continue d'incrementer
        coupsTemp=1; // On retourne potentiellement 1 pion
        // On l'ajoute à la liste des coups
        listeCoups[coups] = (caseTemp->x)*taillePlateau + caseTemp->y;
        while (1) // Pas besoin de vérification puisqu'on sort avec un break
        {
            caseTemp = caseSuivante(*caseTemp, incX, incY, plateau);
            if (caseTemp == NULL)
                break; // Il y a pas de case suivante. On est arrivé au bord sans trouver d'ami
            couleurCase = caseTemp->couleur;

            // Si on tombe sur le joker, on passe cette case:
            if (couleurCase == JOKER)
            {
                continue;
            }
            // Case Vide, Stop:
            if (couleurCase == VIDE)
            {
                break;
            }
            // Si on tombe sur un ami:
            if (couleurCase == couleur)
            {
                rencontre = 1;
                break; // On a trouve qqn et on a rentre les coups. On passe au vecteur suivant
            }
            // On est tombé sur un ennemi, on l'ajoute
            listeCoups[coups + (coupsTemp++)] = (caseTemp->x)*taillePlateau + caseTemp->y;
        }
        if (rencontre)
            coups += coupsTemp;
        coupsTemp = 0;
        rencontre = 0;
    }

    return coups;
}

//! Renvoie 1 si le coup est valide, 0 pour invalide
char valide(int x, int y, char couleur, CASE *plateau[])
{
    CASE caseDepart;
    char increments[8][2] =
    {
        {-1, -1},
        {-1,  0},
        {-1,  1},
        { 0,  1},
        { 1,  1},
        { 1,  0},
        { 1, -1},
        { 0, -1}
    }; // Liste des vecteurs
    int inc=0; // Parcourt les vecteurs
    char rencontre = 0;

    caseDepart = *adresseParXY(x, y, plateau);

    if (caseDepart.couleur != VIDE)
        return 0;

    // On parcourt les vecteurs
    for (inc=0; inc<8; inc++)
    {
        char incX=increments[inc][0], incY=increments[inc][1];
        char couleurCase;
        CASE * caseTemp;

        caseTemp = caseSuivante(caseDepart, incX, incY, plateau);
        // Si on est au bord du plateau ou s'il y a eu une erreur
        if (caseTemp == NULL)
            continue;
        // Si on tombe ensuite sur une case de la meme couleur ou vide, on saute
        if ( (caseTemp->couleur == couleur) || (caseTemp->couleur == VIDE) )
            continue;

        // Dans les autres cas, on continue d'incrementer
        do
        {
            caseTemp = caseSuivante(*caseTemp, incX, incY, plateau);
            if (caseTemp == NULL)
                break; // Il y a pas de case suivante. On est arrivé au bord sans trouver d'ami
            couleurCase = caseTemp->couleur;

            // Si on tombe sur le joker, on passe cette case:
            if (couleurCase == JOKER)
                continue;
            // Case Vide, Stop:
            else if (couleurCase == VIDE)
                break;
            // On a trouvé un pote !
            else if (couleurCase == couleur)
                rencontre = 1; 
        } while(!rencontre);
        if (rencontre)
            return 1; // Au moins 1 vecteur
    }
    return 0; // Rien trouvé: pas valide
}

//! Renvoie le nombre de coups valides
char nbCoupsValides(char couleur, CASE *plateau[], int listeValides[])
{
    char i, j;
    int coups=0;

    for(i=0; i<taillePlateau; i++)
    {
        for(j=0; j<taillePlateau; j++)
        {
            if (valide(i, j, couleur, plateau))
            {
                listeValides[coups] = i*taillePlateau + j;
                coups++;
            }
        }
    }
    return coups; // Rien trouvé: pas valide
}

//! Retourne un Pion
/**
 * Échange, entre NOIR et BLANC, la couleur d'une case.
 * Si la case est vide ou un joker, ne fait rien.
*/
void retourne(int x, int y, CASE *plateau[])
{
    CASE * caseARetourner = adresseParXY(x, y, plateau);
    char couleur = caseARetourner->couleur;

    // Le reste est tout con
    if (couleur == BLANC)
        couleur = NOIR;
    // Important tout de même de else if, sinon ça ne marche pas.
    else if (couleur == NOIR)
        couleur = BLANC;
    // Si c JOKER VIDE ou quoi que ce soit d'autre on se casse, c'est pas notre boulot.
    else
        return;
    caseARetourner->couleur = couleur;
}

//! Joue le tour actuel si possible.
/**
 * Cette fonction, appelée par le main, joue le tour d'un joueur passé
 * en argument. S'il y a lieu, il retourne les pions et place le pion du
 * joueur.
 * Elle renvoie ensuite le nombre de pièces retournées.
 * Donc si le joueur ne peut pas jouer, la fonction ne fait rien et renvoie 0.
 */
int peutJouer(char couleur, CASE *plateau[])
{
    int i, j;

    // On cherche s'il existe au moins 1 coup possible
    for (i=0; i<taillePlateau; i++)
    {
        for (j=0; j<taillePlateau; j++)
        {
            if(valide(i, j, couleur, plateau))
                return 1;
        }
    }
    return 0;
}

//! Place un pion et retourne les pions adéquats
int jouerCoup(int x, int y, char couleur, CASE *plateau[])
{
    int nbCoups;
    int * listeCoups = malloc(taillePlateau * taillePlateau * sizeof(int));

    nbCoups = lister(x, y, couleur, plateau, listeCoups);
    if (!nbCoups)
    {
        free(listeCoups);
        return 0;
    }

    int i;
    for(i=0; i<nbCoups; i++) // On retourne les pions
        retourne(listeCoups[i]/taillePlateau, listeCoups[i]%taillePlateau, plateau);
    adresseParXY(x, y, plateau)->couleur = couleur; // On pose le pion

    free(listeCoups);
    return nbCoups;
}

//! Teste la fin de partie.
/**
 * La partie se finit si le plateau est rempli ou si un joueur n'as plus de
 * pièces sur le plateau (Là, il l'a mauvaise). Dans ce cas cette fonction renvoi 1.
 * Tant que la partie n'est pas finie, cette fonction renvoi 0
 */
int testFinPartie(CASE *plateau[])
{
    int i, j;
    int unicolore=1, plein=1;
    char couleur[2] = {0, 0};

    for (i=0; i<taillePlateau; i++)
    {
        for (j=0; j<taillePlateau; j++)
        {
            CASE pion = *adresseParXY(i, j, plateau);
            if (pion.couleur == JOKER)
                continue;
            if (plein && (pion.couleur == VIDE))
                plein =0;
            if (unicolore && (pion.couleur != VIDE))
            {
                couleur[0] |= (pion.couleur == BLANC);
                // couleur[0] passe ou reste à 1 si on est sur une case blanche
                couleur[1] |= (pion.couleur == NOIR);
                if (couleur[0] && couleur[1])
                    unicolore = 0;
            }

            // Si on n'est ni unicolore ni plein, la partie est pas finie
            if (!(unicolore || plein))
                return 0;
        }
    }

    return 1;
}

//! Renvoi le score de fin de partie.
/**
 * Cette fonction compte le nombre de pions blancs et noirs afin de
 * déterminer le score de fin de partie (donc pour que ce soit sensé :
 * l'utiliser en fin de partie ...)
 * Le score est rangé dans un tableau passé en argument,
 * de la forme : score[blanc,noir].
 */
void score(CASE *plateau[], int * points)
{
    // Déclaration des variables de boucle et de stockage :
    int i=0,j=0;

    // On lit
    for (i=0; i<taillePlateau; i++)
    {
        for (j=0; j<taillePlateau; j++)
        {
            if (adresseParXY(i, j, plateau)->couleur == BLANC)
            {
                points[0]++;
            }
            else if (adresseParXY(i, j, plateau)->couleur == NOIR)
            {
                points[1]++;
            }
        }
    }
}

//! Renvoit un simple pointeur comme tableau multidim
/**
 * Crée et envoie une liste de pointeurs qui représente le tableau.
 * Le pointeur renvoyé représente les colonne et chaque pointeur dans cette
 * liste est une ligne. Le tableau renvoyé est initialisé à VIDE.
*/
CASE ** createPlateau()
{
    // On crée une liste de pointeurs sur les cases
    CASE ** plateau = malloc(taillePlateau * taillePlateau * sizeof(CASE *));
    //erreur("Création d'un plateau à l'adresse %p\n", plateau);
    // On crée maintenant chaque case, et on met son pointeur dans la liste
    int i=0;
    for (i=0;i<(taillePlateau*taillePlateau);i++)
    {
        plateau[i] = malloc(sizeof(CASE));
        plateau[i]->x = i / taillePlateau;
        plateau[i]->y = i % taillePlateau;
        plateau[i]->couleur = VIDE;
    }

    // Cette fonction renvoit un pointeur sur le plateau
    // pour modif et affichage ultérieur.
    return plateau;
}

//! Copie un plateau passé en argument. utilise taillePlateau
CASE ** copiePlateau(CASE *plateau[])
{
    CASE ** copie = malloc(taillePlateau * taillePlateau * sizeof(CASE *));
    //erreur("Copie du plateau %p en %p\n", plateau, copie);
    int i;
    for(i=0; i<(taillePlateau*taillePlateau); i++)
    {
        copie[i] = malloc(sizeof(CASE));
        copie[i]->x = i / taillePlateau;
        copie[i]->y = i % taillePlateau;
        copie[i]->couleur = adresseParXY(copie[i]->x, copie[i]->y, plateau)->couleur;
    }
    return copie;
}

//! Rempli le centre d'un tableau passé en argument
/**
 * Initialise la partie en ajoutant au tableau vide le carré central donné
 * en consigne. Il contient un Joker en son centre.
*/
void initPlateau(CASE *plateau[])
{
    // Le motif donné par la consigne :
    char motif[9];
    if (taillePlateau % 2 == 1) // Plateau Impair
    {
        // La position de début du motif est à :
        int debut = (taillePlateau/2)-1;

        motif[0] = BLANC;  motif[1] = NOIR;  motif[2] = BLANC;
        motif[3] = BLANC;  motif[4] = JOKER; motif[5] = NOIR;
        motif[6] = NOIR;   motif[7] = BLANC; motif[8] = NOIR;

        int i=0,j=0;
        for (i=0;i<3;i++)
        {
            for (j=0;j<3;j++)
            {
                adresseParXY(debut+i, debut+j, plateau)->couleur = motif[j*3+i];
            }
        }
    }
    else
    {
        // La position de début du motif est à :
        int debut = (taillePlateau/2)-1;

        motif[0] = BLANC; motif[1] = NOIR; 
        motif[2] = NOIR;  motif[3] = BLANC;

        int i=0,j=0;
        for (i=0;i<2;i++)
        {
            for (j=0;j<2;j++)
            {
                adresseParXY(debut+i, debut+j, plateau)->couleur = motif[j*2+i];
            }
        }
    }

}

//! Trouve un coup et l'envoit
/**
 * Fait appel à la libIA pour trouver le meilleur coup dans la position donnée
 * Envoit ensuite le coup via la libDisplay (ecrire())
 */
int jouerSonTour(char couleur, CASE *plateau[])
{
    int x, y;
    int retour;

    //erreur("On afiche le plateau que reçoit l'IA:\n");
    affichePlateau(plateau);

    if (!peutJouer(couleur, plateau))
    {
        retour = meilleurXY(plateau, couleur, &x, &y);
        pasDeCoup();
        //erreur("L'IA ne peut pas jouer\n");
        return 1;
    }
    else
    {
        retour = meilleurXY(plateau, couleur, &x, &y); // Trouve le meilleur coup
        if (retour)
            return -1; // Erreur !

        envoyerXY(x, y);
        retour = jouerCoup(x, y, couleur, plateau);
        if (retour == 0)
            return -1; // Erreur comise
    }

    return 0; // Tout s'est bien déroulé
}

//! Lit le coup adverse et le joue
/**
 * Via la libDisplay, on lit le coup adverse (lireCoup())
 * Puis on le joue. S'il est illégal, on retourne un erreur.
 */
int jouerTourAdverse(char couleur, CASE *plateau[])
{
    int retour;
    char x;
    int y;

    retour = lireCoup(&x, &y);
    if (retour == 0) // L'adversaire a joué
    {
        retour = jouerCoup(x, y, !couleur, plateau);
        if (retour == 0) // Aucun pion retourné
            return -1; // L'adversaire a joué un coup illégal
    }
    else if (retour == 1)
        return 1; // L'adversaire n'avait pas de coup}

    return 0; // L'adversaire a joué correctement
}

//! Joue le tour actuel si possible.
/**
 * Cette fonction, appelée par le main, joue le tour d'un joueur passé
 * en argument. S'il y a lieu, il retourne les pions et place le pion du
 * joueur.
 * Elle renvoie ensuite le nombre de pièces retournées.
 * Donc si le joueur ne peut pas jouer, la fonction ne fait rien et renvoie 0.
 */
int jouerTourSimple(char couleur, CASE *plateau[])
{
    int nbCoups=0;
    int coords[2] = {0, 0};

    // On cherche s'il existe au moins 1 coup possible
    if(!peutJouer(couleur, plateau))
        return 0; // Le joueur ne peut pas jouer

    // On demande le coup du joueur
    do
    {
        demanderXY(coords);
        nbCoups = jouerCoup(coords[0], coords[1], couleur, plateau);
    }
    while (nbCoups == 0);

    return nbCoups;
}

//! Initialise l'historique avec le plateau/tronc
/**
 * Après cette fonction, le tronc contient le plateau de départ
 * (On peut partir avec un plateau de partie déjà en cours)
 * Et actuel pointe sur le tronc
 */
NOEUD * initHistorique(CASE *plateau[], char couleur)
{
    tronc.nbDeFils = 0;
    tronc.listeFils = NULL;
    tronc.plateau = copiePlateau(plateau);
    tronc.couleur = !couleur;
    tronc.pere = NULL;

    actuel = &tronc;

    return &tronc;
}

//! ajoute un fils au noeud actuel
/**
 * L'historique supporte les variantes: un noeud peut avoir plusieurs fils
 * Cette fonction retourne un pointeur vers le noeud crée
 */
NOEUD * majHistorique(CASE *plateau[], char couleur)
{
    NOEUD * fils = malloc(sizeof(NOEUD));
    fils->nbDeFils = 0;
    fils->listeFils = NULL;
    fils->plateau = copiePlateau(plateau);
    fils->couleur = couleur;
    fils->pere = actuel;

    // TODO : Si le fils existe déjà, il faut pas le recrée

    ajouteFils(actuel, fils);

    actuel = fils;
    return actuel; // On informe le programme de l'emplacement du nouveau noeud
}

//! Libère les noeuds de l'historique
void endHistorique()
{
    int j;
    elaguer(&tronc); // On passe du désherbant hyperpuissant

    for (j=0; j<(taillePlateau*taillePlateau); j++)
        free(tronc.plateau[j]);
    free(tronc.plateau);
}

//! renvoie la couleur du joueur dont c'est le tour
/**
 * Fais pointer actuel sur son propre père.
 * Recopie ensuite le plateau du noeud actuel dans le plateau passé en
 * argument.
 * Renvoie BLANC si on est au tronc; l'inverse de la couleur du plateau sinon.
 */
char reculerHistorique(CASE * plateau[])
{
    if (actuel->pere != NULL)
    {
        actuel = actuel->pere;

        int j;
        for (j=0; j<taillePlateau*taillePlateau; j++)
            *(plateau[j]) = *(actuel->plateau[j]);
    }

    return !(actuel->couleur);
}

//! renvoie la couleur du joueur dont c'est le tour
/**
 * Fais pointer actuel sur le noeud passé en argument
 * Recopie ensuite le plateau du noeud actuel dans le plateau passé en
 * argument.
 * Renvoie BLANC si on est au tronc; l'inverse de la couleur du plateau sinon.
 */
char sauterHistorique(CASE * plateau[], NOEUD * destination)
{

    if (destination == NULL)
        return -1;

    int j;
    actuel = destination;
    for (j=0; j<taillePlateau*taillePlateau; j++)
        *(plateau[j]) = *(actuel->plateau[j]);

    return !(actuel->couleur);
}

//! renvoie la couleur du joueur dont c'est le tour
/**
 * Fais pointer actuel sur son premier fils
 * Recopie ensuite le plateau du noeud actuel dans le plateau passé en
 * argument.
 * Renvoie BLANC si on est au tronc; l'inverse de la couleur du plateau sinon.
 */
char avancerHistorique(CASE * plateau[])
{
    if (actuel->nbDeFils <= 0 || actuel->listeFils == NULL)
        return (actuel->couleur == VIDE) ? BLANC : !(actuel->couleur);

    actuel = actuel->listeFils[actuel->nbDeFils - 1];
    
    int j;
    for (j=0; j<taillePlateau*taillePlateau; j++)
        *(plateau[j]) = *(actuel->plateau[j]);

    return !(actuel->couleur);
}

//! Ajoute un fils à la liste d'un noeud
/**
 * Utilise realloc pour agrandir la liste du pere(premier argument) et
 * ajoute un pointeur vers le fils(2ème argument) à la liste.
 */
void ajouteFils(NOEUD * pere, NOEUD * fils)
{
    (pere->nbDeFils)++;
    // On étend la liste
    pere->listeFils = realloc(pere->listeFils, pere->nbDeFils * sizeof(NOEUD *));

    pere->listeFils[pere->nbDeFils - 1] = fils;
}

//! libère tout les fils d'un noeud
static int elaguer(NOEUD * noeud)
{
    int i, j;
    for (i=0; i<noeud->nbDeFils; i++)
    {
        NOEUD * fils = noeud->listeFils[i];
        if (fils->listeFils != NULL) // Ce fils a des fils, donc appel récursif de cette fonction
            elaguer(fils);

        // On libère le plateau:
        for(j=0; j<(taillePlateau*taillePlateau); j++)
            free((fils->plateau)[j]);
        free(fils->plateau);

        // On peut maintenant libérer ce fils
        free(noeud->listeFils[i]);
    }
    // On libère la liste des fils:
    free(noeud->listeFils);

    return 0;
}

