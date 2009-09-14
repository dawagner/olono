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

// On inclu nos librairies personnelles :
#include "../lib/libDefine.h"
#include "../lib/libDisplay.h"
#include "../lib/libPlay.h"
#include "../lib/libCases.h"

void boucleDeJeu(struct _CASE *plateau[]);

// Certains compilos définissent _WIN32, d'autres __WIN32
#ifdef  _WIN32
#ifndef __WIN32
#define __WIN32
#endif
#endif

// On utilise agrp pour traiter les options, c'est GNU et c'est joli
// Par contre, il faut un peu de définition, d'où le bloc suivant :

// ############################################################### //
// ### Ici la magnifique structure bordélique de argp : ########## //
// ############################################################### //
//{


/*
   OPTIONS.  Field 1 in ARGP.
   Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
*/
static struct argp_option options[] =
{
    {"taille-plateau", 't', "TAILLE", 0, "Changer la taille de plateau par défaut (9)"
    },
    {"verbose",        'v', 0, 0, "Activer les commentaires étendus"},
    {"Tore",           'T', 0, 0, "Avoir un plateau Toroïdal (les bords sont connectés)"},
    {0}
};

/*
   PARSER. Field 2 in ARGP.
   Order of parameters: KEY, ARG, STATE.
*/
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    arguments *args = state->input;

    switch (key)
    {
    case 't':
        args->taillePlateau = (int)arg;
        break;
    case 'v':
        args->verbose = 1;
        break;
    case 'T':
        args->Tore = 1;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/*
   ARGS_DOC. Field 3 in ARGP.
   A description of the non-option command-line arguments
     that we accept.
*/
static char args_doc[] = "";

/*
  DOC.  Field 4 in ARGP.
  Program documentation.
*/
static char doc[] =
    "OloNO -- A program to play Othello, simply way better \vOloNO is Not an Othello - C project 2008-2009";

/*
   The ARGP structure itself.
*/
static struct argp argp =
{
    options, parse_opt, args_doc, doc
};
//}
// ############################################################### //

const char *argp_program_version =
    "OloNO 0.4 - édition 42";

const char *argp_program_bug_address =
    "<dwagner@isep.fr> &| <mpotier@isep.fr>";

// Variables globales
arguments args;
int    taillePlateau = 9;       // Taille du plateau par défaut;

//! Lance le programme
/**
 * Cette fonction est la boucle principale du programme.
 * Elle permet de rassembler toutes les autres fonctions dans un contenu reflétant
 * le fonctionnement du programme. Pour comprendre le fonctionnement d'un programme,
 * lire le main() devrait suffire.
*/
int main(int argc, char ** argv)
{

    /* On initialise les arguments à leur valeur par défaut */
    args.taillePlateau = 0;
    args.verbose       = 0;
    args.Tore          = 0;

    /* This is where the magic should happen | Attention, instant magique ... */
    argp_parse (&argp, argc, argv, 0, 0, &args);

    // Si l'argument --taille-plateau ou -t est placé alors il override la taille par défaut.
    if (args.taillePlateau != 0)
        taillePlateau = atoi((char *)args.taillePlateau);

    if (taillePlateau < 4 || taillePlateau > 25)
    {
        printf("La taile doit être comprise entre 4 et 25");
        return -1;
    }


// ############################################################### //
// ### On commence les choses sérieuses ici (cad le main) ######## //
// ############################################################### //

    int  i=0;                    // Variables de boucle.
    int  points[]={0, 0};             // Points obtenus pour les blancs et les noirs


// ######################################################################################### //
// # ETAPE 1 : Dire Bonjour (c'est important de saluer son utilisateur, qu'on se le dise). # //
// ######################################################################################### //

    direBonjour();
    
    // si l'argument -T ou --Tore est placé alors on le rappelle au joueur
    if (args.Tore)
        ecrire ("Vous avez choisi le mode Toroïdal :p \n");

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

    initHistorique(plateau, BLANC);
    boucleDeJeu(plateau);
    endHistorique();


// ######################################################################################### //
// # ETAPE 4 : Score ####################################################################### //
// ######################################################################################### //

    score(plateau, points);
    affichePlateau(plateau);
    afficherScore(points);


    // On oublie pas de libérer l'espace mallocé
    for (i=0; i<taillePlateau*taillePlateau; i++)
        free( plateau[i] );
    free(plateau);
    return 0;
}

//! Boucle de jeu, pour jouer quoi ...
void boucleDeJeu(struct _CASE *plateau[])
{
    // On commence par les blancs (dans la rêgle du jeu).
    char couleur = BLANC;
    int  aPuJouer[]={1, 1};

    do
    {
        affichePlateau(plateau);
        if (peutJouer(couleur, plateau))
        {
            char action = demanderAction(couleur);

            switch (action)
            {
                case 'J':
                    break;
                case 'R':
                    couleur = reculerHistorique(plateau);
                    continue;
                default:
                    return;
            }
            ecrire("A %s de jouer\n", couleur ? "Noir" : "Blanc");
        }
        aPuJouer[(int) couleur] = jouerTourSimple(couleur, plateau);

        // On ajoute ce plateau à l'historique
        majHistorique(plateau, couleur);

        if (!aPuJouer[(int) couleur])
        {
            ecrire("Vous ne pouvez pas jouer, désolé.\n");

            if (!aPuJouer[!couleur]) // Si en plus l'autre joueur ne pouvait pas jouer
                break; // On sort de la boucle de jeu

            couleur = !couleur;
            continue; // On continue un nouveau tour
        }

        // On nettoye l'écran
        // (À changer, pour la future interface graphique !)
        /*
        #ifdef __WIN32
        sytem("cls");
        #else
        system("clear");
        #endif
        */

        ecrire("Bravo, vous avez pû jouer! Voilà la grille \n");
        couleur = !couleur;
    }
    while (!testFinPartie(plateau));
}
