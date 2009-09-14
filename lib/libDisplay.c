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
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "libDefine.h"
#include "libPlay.h"
#include "libDisplay.h"

int taillePlateau;

//! Demande les coordonnées au joueur
/**
 * Cette fonction demande à l'utilisateur les coordonnées du point à jouer
 * et les converti pour qu'elles correspondent aux indices en mémoire.
*/
char demanderAction(char couleur)
{
    char retourScanf = '\0';

    do
    {
        ecrire("%s, que voulez-vous faire: (J)ouer ou (R)eculer dans l'historique ? ",
            (couleur == BLANC) ? "Blanc" : "Noir");
        scanf(" %c%*[^\n]", &retourScanf); // On scan un char puis n'importe quoi sauf un retour à la ligne

        int c;
        // ... On vide le flux
        while ( ((c = getchar()) != '\n') && c != EOF);
    }
    while (retourScanf != 'J' && retourScanf != 'R');

    return retourScanf;
}

//! Demande les coordonnées au joueur
/**
 * Cette fonction demande à l'utilisateur les coordonnées du point à jouer
 * et les converti pour qu'elles correspondent aux indices en mémoire.
*/
void demanderXY(int coords[])
{
    char x=0;
    int  y=0;

    do
    {
        printf("Entrez les coordonnées où jouer: ");
        scanf(" %c%d%*[^\n]", &x, &y); // On scan un char puis un int puis n'importe quoi sauf un retour à la ligne
        x = toupper(x);

        int c;
        // ... On vide le flux
        while ( ((c = getchar()) != '\n') && c != EOF);
    }
    while (!( x>='A' && (x-'A')<taillePlateau && y>=0 && y<taillePlateau ));
    // tant que les coordonnees ne sont pas dans les limites

    // on passe les coordonnees normalises (partant de 0) ET transtypage.
    coords[0] = (int) x-'A';
    coords[1] = taillePlateau-y-1;
}

//! Affiche le plateau et un cadrillage
/**
 * Cette fonction renvoie et affiche de manière "jolie" le tableau passé en paramètre
*/
void affichePlateau(CASE *plateau[])
{
    // Déclaration des variables de boucle
    int  i=0,j=0;
    char y=taillePlateau-1;
    char x='A';
    char pions[4];

    pions[BLANC] = 'O';
    pions[NOIR]  = 'X';
    pions[JOKER] = '#';
    pions[VIDE]  = '.';

    erreur("\n");

    // Petite décoration au dessus du plateau :
    for (i=0;i<taillePlateau+1;i++)
        erreur("--");
    erreur("+\n");

    // Boucle simple pour afficher les lignes et les colonnes
    // du tableau
    for (i=0;i<taillePlateau;i++)
    {
        erreur("%.2d ",y--);
        for (j=0;j<taillePlateau;j++)
        {
            erreur("%c ",pions[(int) adresseParXY(j, i, plateau)->couleur]);
        }
        erreur("\n"); // après chaque ligne, retour à la ligne
    }
    erreur("   ");
    for (i=0;i<taillePlateau;i++)
        erreur("%c ",x++);
    erreur("\n");

    //Petite décoration en dessous du plateau :
    for (i=0;i<taillePlateau+1;i++)
        erreur("--");
    erreur("+\n");
}

//! Écrit sur la sortie standard
/**
 * Cette fonction utilise write pour écrire sur la sortie standard.
 * Elle est inspirée de la version de glib de la fonction printf
 */
void ecrire(char *chaine, ...)
{
    char * copie = malloc(255 * sizeof(char));
    // On déclare la liste d'arguments
    va_list arg;

    // On les assigne
    va_start(arg, chaine);
    // vsprintf fonctionne comme sprintf (qui, au lieu d'imprimer sur stdout, imprime sur la chaine qu'on lui donne.)
    // Ici, il se trouve qu'on imprime sur stdout... mais la fonction ecrire() permettra d'assurer la compatibilité avec la version graphique.
    // le v devant sprintf signifie qu'au lieu de prendre plusieurs arguments en paramètre, elle prend une variable de type va_list en paramètre. C'est donc tout à fait adapté au but recherché.
    vsprintf(copie, chaine, arg);
    write(STDOUT_FILENO, copie, strlen(copie));
    // On termine... je sais pas vraiment pourquoi, mais c'est comme ça. (à approfondir)
    va_end(arg);

    free(copie);

}

//! Écrit sur la sortie erreur
/**
 * Cette fonction utilise write pour écrire sur la sortie erreur.
 */
void erreur(char *chaine, ...)
{
    char * copie = malloc(255 * sizeof(char));
    // On déclare la liste d'arguments
    va_list arg;

    // On les assigne
    va_start(arg, chaine);
    // vfprintf fonctionne comme fprintf (qui, au lieu d'imprimer sur stdout, imprime sur la sortie qu'on lui donne.)
    // Ici, il se trouve qu'on imprime sur stdout... mais la fonction ecrire() permettra d'assurer la compatibilité avec la version graphique.
    // le v devant fprintf signifie qu'au lieu de prendre plusieurs arguments en paramètre, elle prend une variable de type va_list en paramètre. C'est donc tout à fait adapté au but recherché.
    vsprintf(copie, chaine, arg);
    write(STDERR_FILENO, copie, strlen(copie));
    // On termine... je sais pas vraiment pourquoi, mais c'est comme ça. (à approfondir)
    va_end(arg);

    free(copie);

}

//! Demande la taille du plateau
int demanderTaillePlateau()
{
    taillePlateau=0;
    int retourScanf;

    while ((taillePlateau < 4) || (taillePlateau > 25))
    {
        printf("Entrez une taille entre 4 et 25 : ");
        retourScanf = scanf("%d%*[^\n]",&taillePlateau);
        // Si le scanf a retourné une erreur ...
        if (!retourScanf)
        {
            // ... On vide le flux
            char c;
            while ( ((c = getchar()) != '\n') && c != EOF);
        }
        else
            // Si tout s'est bien passé, on enlève le \n final
            getchar();
    }

    return taillePlateau;
}

//! Dire bonjour :
void direBonjour()
{
ecrire("\tOloNO  Copyright (C) 2008  David Wagner && Martin Potier\n\
\tThis program comes with ABSOLUTELY NO WARRANTY;\n\
\tThis is free software, and you are welcome to redistribute it under\n\
\tcertain conditions\n\n");
ecrire("Bienvenue à OloNO\nOloNO is not an othello\n\n");
}

//! utilise ecrire() pour envoyer les coordonnées
void envoyerXY(int x, int y)
{
    ecrire("%c%d\n", x + 'A', taillePlateau - y - 1);
}

//! utilise ecrire() pour signifier qu'il n'y a pas de coup possible
void pasDeCoup()
{
    write(STDOUT_FILENO, "pas de coup\n", 12);
}

//! lit un coup sur stdin
/**
 * Si "pas de coup" est reçu, on renvoit 1.
 * Sinon, on renvoie 0 et on place les coordonnées dans x et y
 * Cette fonction ne se charge pas de vérifier que les coordonnées soient
 * bonnes: ele est utilisée par l'IA, qui doit s'arrêter en cas d'erreur de
 * l'adversaire
 */
int lireCoup(char * x, int * y)
{
    char * input = malloc(20 * sizeof(char));
    char * copie = malloc(20 * sizeof(char));

    read(STDIN_FILENO, input, 20);

    int i=0;
    while((copie[i] = input[i]) && copie[i++] != '\n');
    copie[i] = '\0';
    free(input);

    if (strcmp(copie, "pas de coup\n") == 0)
    {
        free(copie);
        return 1; // L'adversaire ne peut pas jouer
    }

    sscanf(copie, "%c%d", x, y);
    *x = (int) toupper(*x) - 'A';
    *y = taillePlateau - *y - 1;

    free(copie);
    return 0;
}

//! Affiche le score
/**
 * Utilise l'erreur standard pour ne pas encombrer la stdin qui ne doit
 * concerner que les coups joués
 */
void afficherScore(int points[])
{
    if (points[0] > points[1])
        erreur("Les blancs gagnent par %d à %d\n", points[0], points[1]);
    else if (points[1] > points[0])
        erreur("Les noirs gagnent par %d à %d\n", points[1], points[0]);
    else
        erreur("Partie nulle: %d partout\n", points[0]);
}

