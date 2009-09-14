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
 * recherche d'une pierre amie (le while(!rencontre)).
 * Si on tombe sur une pierre amie, alors on parcourt le vecteur en sans
 * inverse (la boucle sur j. Ici gît). Dans cette boucle, la liste des
 * coups est mise à jour et le compteur incrémenté (celui qu'on renvoie)
 *
 * A tout moment, si on rencontre un joker, on le saute, c'est à dire qu'on fait un
 * continue.
*/
char valide(int x,int y,char couleur,CASE *plateau[],int taillePlateau, int listeCoups[])
{
    CASE caseDepart, caseTemp;
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
    char inc=0; // Parcourt les vecteurs
    char i=0, j=0;
    char coups=0; // Compteur de nombre de coups

    caseDepart = *adresseParXY(x, y, plateau, taillePlateau);

    if (caseDepart.couleur != VIDE)
        return 0;

    // On parcourt les vecteurs
    for (inc=0; inc<8; inc++)
    {
        char incX=increments[inc][0], incY=increments[inc][1];
        char rencontre = 0; // determine si on a rencontre un ami
        char couleurCase;

        caseTemp = *caseSuivante(caseDepart, incX, incY, plateau, taillePlateau);
        // Si on tombe ensuite sur une case de la meme couleur ou vide, on saute
        if ( (caseTemp.couleur == couleur) || (caseTemp.couleur == VIDE) )
            continue;
        // Dans les autres cas, on continue d'incrementer
        i=1; // i est le compteur d'increments
        while (!rencontre)
        {
            i++; // On passe a la case suivante le long du vecteur
            //char X = x + (i*incX);
            //char Y = y + (i*incY);

            caseTemp = *caseSuivante(caseTemp, incX, incY, plateau, taillePlateau);
            couleurCase = caseTemp.couleur;

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
                // On met les cases dans la liste et on agmente le compteur
                for (j=i-1; j>0; j--)
                {
                    caseTemp = *caseSuivante(caseTemp, -incX, -incY, plateau, taillePlateau);
                    if (caseTemp.couleur == JOKER) // Le joker, on saute
                        continue;
                    listeCoups[coups++] = (caseTemp.x)*taillePlateau + caseTemp.y;
                    // Pour retrouver la case: x = coup/taille et y = coup%taille
                }
                break; // On a trouve qqn et on a rentre les coups. On passe au vecteur suivant
            }
        }
    }

    return coups;
}

//! Retourne un Pion
/**
 * Échange, entre NOIR et BLANC, la couleur d'une case.
 * Si la case est vide ou un joker, ne fait rien.
*/
void retourne(int x, int y, CASE *plateau[], int taillePlateau)
{
    CASE * caseARetourner = adresseParXY(x, y, plateau, taillePlateau);
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

//! Détermine si une coordonnée se trouve dans le plateau ou non.
/**
 * Cette fonction détermine si une coordonnée se trouve dans le plateau ou non.
 * Si oui elle renvoit 1, sinon elle renvoit 0.
*/
int caseDansLeTableau(int x, int y, int taillePlateau)
{
    return !(x>(taillePlateau-1) || x<0 || y>(taillePlateau-1) || y<0);
}

//! Joue le tour actuel si possible.
/**
 * Cette fonction, appelée par le main, joue le tour d'un joueur passé
 * en argument. S'il y a lieu, il retourne les pions et place le pion du
 * joueur.
 * Elle renvoie ensuite le nombre de pièces retournées.
 * Donc si le joueur ne peut pas jouer, la fonction ne fait rien et renvoie 0.
 */
int jouerTour(char couleur, CASE *plateau[], int taillePlateau)
{
    int nbCoups=0;
    int coords[2] = {0, 0};
    int * listeCoups = malloc((taillePlateau - 3)*4 * sizeof(int));
    int i, j;
    CASE * caseJouee;

    // On cherche s'il existe au moins 1 coup possible
    for (i=0; i<taillePlateau; i++)
    {
        for (j=0; j<taillePlateau; j++)
        {
            nbCoups = valide(i, j, couleur, plateau, taillePlateau, listeCoups);
            if (nbCoups)
                break;
        }
        if (nbCoups)
            break;
    }

    // S'il n'y en a pas
    if (!nbCoups)
    {
        free(listeCoups);
        return 0; // Le joueur ne peut pas jouer
    }

    // On demande le coup du joueur
    do
    {
        demanderXY(coords, taillePlateau);
        caseJouee = adresseParXY(coords[0], coords[1], plateau, taillePlateau);
        nbCoups = valide(coords[0], coords[1], couleur, plateau, taillePlateau, listeCoups);
    }
    while (!nbCoups);

    // On retourne les pions adéquats
    for (i=0; i<nbCoups; i++)
    {
        // Les coordonnées sont codées, on les décode avec une division entière/reste
        retourne(listeCoups[i]/taillePlateau,listeCoups[i]%taillePlateau,plateau,taillePlateau);
    }
    // On place un pion où le joueur a joué
     caseJouee->couleur = couleur;

    free(listeCoups);
    return nbCoups;
}

//! Teste la fin de partie.
/**
 * La partie se finit si le plateau est rempli ou si un joueur n'as plus de
 * pièces sur le plateau (Là, il l'a mauvaise). Dans ce cas cette fonction renvoi 1.
 * Tant que la partie n'est pas finie, cette fonction renvoi 0
 */
int testFinPartie(CASE *plateau[], int taillePlateau)
{
    int i, j;
    int unicolore=1, plein=1;
    char couleur[2] = {0, 0};

    for (i=0; i<taillePlateau; i++)
    {
        for (j=0; j<taillePlateau; j++)
        {
            CASE pion = *adresseParXY(i, j, plateau, taillePlateau);
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
void score(CASE *plateau[],int taillePlateau, int * points)
{
    // Déclaration des variables de boucle et de stockage :
    int i=0,j=0;

    // On lit
    for (i=0; i<taillePlateau; i++)
    {
        for (j=0; j<taillePlateau; j++)
        {
            if (adresseParXY(i, j, plateau, taillePlateau)->couleur == BLANC)
            {
                points[0]++;
            }
            if (adresseParXY(i, j, plateau, taillePlateau)->couleur == NOIR)
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
CASE ** createPlateau(int taillePlateau)
{
    // On crée une liste de pointeurs sur les cases
    CASE ** plateau = malloc(taillePlateau * taillePlateau * sizeof(CASE *));
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

//! Rempli le centre d'un tableau passé en argument
/**
 * Initialise la partie en ajoutant au tableau vide le carré central donné
 * en consigne. Il contient un Joker en son centre.
*/
void initPlateau(CASE *plateau[],int taillePlateau)
{
    // Le motif donné par la consigne :
    char motif[9] = {BLANC,NOIR,BLANC,BLANC,JOKER,NOIR,NOIR,BLANC,NOIR};
    // La position de début du motif est à :
    int debut = (taillePlateau/2)-1;

    int i=0,j=0;
    for (i=0;i<3;i++)
    {
        for (j=0;j<3;j++)
        {
            adresseParXY(debut+i, debut+j, plateau, taillePlateau)->couleur = motif[j*3+i];
        }
    }
}

