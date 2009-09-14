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
 * libDisplay.c (console)
 * Contient les fonctions d'affichage en mode console:
 * concerne aussi bien l'affichage de la grille que les entrées utilisateur.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include "libDefine.h"
#include "libPlay.h"
#include "libDisplay.h"

//! Demande les coordonnées au joueur
/**
 * Cette fonction demande à l'utilisateur les coordonnées du point à jouer
 * et les converti pour qu'elles correspondent aux indices en mémoire.
*/
void demanderXY(int coords[], int taillePlateau)
{
    char x=0;
    int  y=0;
    int retourScanf;

    do
    {
        printf("Entrez les coordonnées où jouer: ");
        scanf(" %c%d%*[^\n]", &x, &y);

        int c;
        // ... On vide le flux
        while ( ((c = getchar()) != '\n') && c != EOF);
    }
    while (!( x>=65 && (x-65)<taillePlateau && y>=0 && y<taillePlateau ));
    // tant que les coordonnees ne sont pas dans les limites

    // on passe les coordonnees normalises (partant de 0) ET transtypage.
    coords[0] = (int) x-65;
    coords[1] = taillePlateau-y-1;
}

//! Affiche le plateau et un cadrillage
/**
 * Cette fonction renvoie et affiche de manière "jolie" le tableau passé en paramètre
*/
void affichePlateau(CASE *plateau[],int taillePlateau)
{
    // Déclaration des variables de boucle
    int  i=0,j=0;
    char y=taillePlateau-1;
    char x=65; // char(65) correspond à A
    char pions[4];

    pions[BLANC] = 'O';
    pions[NOIR]  = 'X';
    pions[JOKER] = '#';
    pions[VIDE]  = '.';

    printf("\n");

    // Petite décoration au dessus du plateau :
    for (i=0;i<taillePlateau+1;i++)
        printf("--");
    printf("+\n");

    // Boucle simple pour afficher les lignes et les colonnes
    // du tableau
    for (i=0;i<taillePlateau;i++)
    {
        printf("%.2d ",y--);
        for (j=0;j<taillePlateau;j++)
        {
            printf("%c ",pions[adresseParXY(j, i, plateau, taillePlateau)->couleur]);
        }
        printf("\n"); // après chaque ligne, retour à la ligne
    }
    printf("   ");
    for (i=0;i<taillePlateau;i++)
        printf("%c ",x++);
    printf("\n");

    //Petite décoration en dessous du plateau :
    for (i=0;i<taillePlateau+1;i++)
        printf("--");
    printf("+\n");
}

//! Écrit sur la sortie standard
/**
 * Cette fonction utilise write pour écrire sur la sortie standard.
 * Elle s'assure de n'écrire que l'essentiel en enlevant le \\0 final et tout
 * ce qui se trouve après.
 */
void ecrire(char *chaine, ...)
{

    // On déclare la liste d'arguments
    va_list arg;

    // On les assigne
    va_start(arg, chaine);
    // vfprintf fonctionne comme fprintf (qui, au lieu d'imprimer sur stdout, imprime sur la sortie qu'on lui donne.)
    // Ici, il se trouve qu'on imprime sur stdout... mais la fonction ecrire() permettra d'assurer la compatibilité avec la version graphique.
    // le v devant fprintf signifie qu'au lieu de prendre plusieurs arguments en paramètre, elle prend une variable de type va_list en paramètre. C'est donc tout à fait adapté au but recherché.
    vfprintf(stdout, chaine, arg);
    // On termine... je sais pas vraiment pourquoi, mais c'est comme ça. (à approfondir)
    va_end(arg);

}

//! Demande la taille du plateau
int demanderTaillePlateau()
{
    int taillePlateau=0;
    int retourScanf;

    while ((taillePlateau%2 == 0) || (taillePlateau <= 3) || (taillePlateau > 25))
    {
        printf("Taille impaire (5-25) : ");
        retourScanf = scanf("%d%*[^\n]",&taillePlateau);
        // Si le scanf a retourné une erreur ...
        if (!retourScanf)
        {
            // ... On vide le flux
            int c;
            while ( ((c = getchar()) != '\n') && c != EOF);
        }
        else
            // Si tout s'est bien passé, on enlève le \n final
            getchar();
    }

    return taillePlateau;
}

