/*  This file is part of OloNO
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
 * main.c
 * Contient la boucle principale du programme
 */

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <string.h>

// On inclu nos librairies personnelles :
#include "../lib/libDefine.h"
#include "../lib/libDisplay.h"
#include "../lib/libPlay.h"
#include "../lib/libCases.h"
#include "../lib/libIA.h"

int boucleDeJeu(CASE *plateau[]);

// Certains compilos définissent _WIN32, d'autres __WIN32
#ifdef  _WIN32
#ifndef __WIN32
#define __WIN32
#endif
#endif

// Variables globales
arguments args;
int    taillePlateau;       // Taille du plateau par défaut;

//! Lance le programme
/**
 * Cette fonction est la boucle principale du programme.
 * Elle permet de rassembler toutes les autres fonctions dans un contenu reflétant
 * le fonctionnement du programme. Pour comprendre le fonctionnement d'un programme,
 * lire le main() devrait suffire.
*/
int main(int argc, char ** argv)
{

    if (argc < 5)
        return 1;

    /* On initialise les arguments à leur valeur par défaut */
    args.taillePlateau = atoi(argv[1]);
    args.verbose = (strcmp("grilleOn", argv[2]) == 0) ? 1 : 0;
    strcpy(args.type, argv[3]);
    args.couleur = (*argv[4] == 'B' ? 0 : 1);
    args.Tore        = 0;

    taillePlateau = args.taillePlateau;

//    erreur("valeur d'un coin:%d\n", valeurLigne(0)+valeurLigne(0));
//    erreur("valeur en diagonale d'un coin:%d\n", valeurLigne(1)+valeurLigne(1));
//    erreur("idem, mais a la fin:%d\n", valeurLigne(taillePlateau-2)+valeurLigne(taillePlateau-2));
//    erreur("valeur d'un pion a coté du joker:%d\n", valeurLigne(taillePlateau/2 - 1)*valeurLigne(taillePlateau/2 + 1));

// ############################################################### //
// ### On commence les choses sérieuses ici (cad le main) ######## //
// ############################################################### //

    int  i=0;             // Variables de boucle.
    int  points[]={0, 0}; // Points obtenus pour les blancs et les noirs
    char retour;

// ######################################################################################### //
// # ETAPE 2 : Initialiser le plateau ###################################################### //
// ######################################################################################### //

    // On créé un tableau à la taille demandée par le joueur
    CASE ** plateau = createPlateau();
    // On initialise ce tableau avec la consigne
    initPlateau(plateau);

// ######################################################################################### //
// # ETAPE 3 : Boucle de jeux principale ################################################### //
// ######################################################################################### //

    retour = boucleDeJeu(plateau);

    if (retour)
        erreur("La boucle de jeu s'est arrêtée à cause d'une erreur\n");

// ######################################################################################### //
// # ETAPE 4 : Score ####################################################################### //
// ######################################################################################### //

    affichePlateau(plateau);
    score(plateau, points);
    afficherScore(points);

    // On oublie pas de libérer l'espace mallocé
    for (i=0; i<taillePlateau*taillePlateau; i++)
        free( plateau[i] );
    free(plateau);
    return 0;
}

//! Boucle de jeu, pour jouer quoi ...
/**
 * Chaque IA joue son coup puis attend le coup de l'adversaire (ou l'inverse).
 * Si c'est demandé, elle affiche aussi le platau entre les deux.
 * En cas d'erreur, quelle qu'elle soit (interne(peu probable)) ou de la part
 * de l'adversaire (peu probable sauf si l'adversaire est un humain), alors on
 * s'arrête (boucleDeJeu interprêtera un retour de 1 comme une erreur.)
 */
int boucleDeJeu(CASE *plateau[])
{
    // On commence par les blancs (dans la règle du jeu).
    char couleur = args.couleur;
    char aPuJouer[] = {1, 1}; // aPuJouer[0]: blanc; aPuJouer[1]: noir


    if (couleur == 0)
    {
        while (!testFinPartie(plateau) && !(aPuJouer[BLANC] == 0 && aPuJouer[NOIR] == 0) )
        {
            int retour;
            aPuJouer[BLANC] = 1;
            // On a les blancs, on commence à jouer
            retour = jouerSonTour(couleur, plateau);
            if (retour == -1)
                return -1; // Une erreur s'est produite
            else if (retour == 1) // On ne pouvait rien jouer
                aPuJouer[BLANC] = 0;

            if (args.verbose)
                affichePlateau(plateau);

            // Puis on lit le coup de l'adversaire
            aPuJouer[NOIR] = 1;
            retour = jouerTourAdverse(couleur, plateau);
            if (retour == -1)
                return -1; // L'adversaire a commis une faute
            else if (retour == 1) // L'adversaire n'a rien joué
                aPuJouer[NOIR] = 0;

        }
    }

    else
    {
        while (!testFinPartie(plateau) && !(aPuJouer[BLANC] == 0 && aPuJouer[NOIR] == 0))
        {
            int retour;

            if (args.verbose)
                affichePlateau(plateau);

            // On a les noirs, on commence par écouter l'adversaire
            aPuJouer[BLANC] = 1;
            retour = jouerTourAdverse(couleur, plateau);
            if (retour == -1)
                return -1; // Une erreur s'est produite
            else if (retour == 1) // On ne pouvait rien jouer
                aPuJouer[BLANC] = 0;

            // Puis on joue
            aPuJouer[NOIR] = 1;
            retour = jouerSonTour(couleur, plateau);
            if (retour == -1)
                return -1; // Une erreur s'est produite
            else if (retour == 1) // On ne pouvait rien jouer
                aPuJouer[NOIR] = 0;

        }
            
    }
    return 0; // Pas d'erreur
}

