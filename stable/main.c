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
#include "libDefine.h"
#include "libDisplay.h"
#include "libPlay.h"
#include "libCases.h"

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

    /* This structure is used by main to communicate with parse_opt. */
    struct arguments
    {
        int argTaillePlateau;   /* Arguments for -t */
        int verbose;            /* Arguments for -v */
    };

    /*
       OPTIONS.  Field 1 in ARGP.
       Order of fields: {NAME, KEY, ARG, FLAGS, DOC}.
    */
    static struct argp_option options[] =
    {
        {"taille-plateau", 't', 0, 0,
            "Changer la taille de plateau par défaut (9)"
        },
        {"verbose", 'v', 0, 0, "Activer les commentaires étendus"},
        {0}
    };

    /*
       PARSER. Field 2 in ARGP.
       Order of parameters: KEY, ARG, STATE.
    */
    static error_t
    parse_opt (int key, char *arg, struct argp_state *state)
    {
        struct arguments *arguments = state->input;

        switch (key)
        {
        case 't':
            arguments->argTaillePlateau = 1;
            break;
        case 'v':
            arguments->verbose = 1;
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
      DOC.  Fielhttp://www.gnu.org/software/libtool/manual/libc/Argp-Example-4.html#Argp-Example-4d 4 in ARGP.
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
"OloNO 0.3 - édition 42";

const char *argp_program_bug_address =
"<dwagner@isep.fr> &| <mpotier@isep.fr>";

//! Lance le programme
/**
 * Cette fonction est la boucle principale du programme.
 * Elle permet de rassembler toutes les autres fonctions dans un contenu reflétant
 * le fonctionnement du programme. Pour comprendre un programme, lire le main() devrait suffire.
*/
int main(int argc, char ** argv)
{
// ############################################################### //
// ### On traite les arguments : init puis affectation ########### //
// ############################################################### //

    struct arguments arguments;
    FILE *outstream;

    /* On initialise les arguments à leur valeur par défaut */
    arguments.argTaillePlateau = 0;
    arguments.verbose = 0;

    /* This is where the magic should happen | Attention, instant magique ... */
    argp_parse (&argp, argc, argv, 0, 0, &arguments);


// ############################################################### //
// ### On commence les choses sérieuses ici (cad le main) ######## //
// ############################################################### //

    int  taillePlateau=9; // Taille du plateau par défaut;
    int  i=0, j=0;                    // Variables de boucle.
    int  coords[]={0, 0};             // Coordonnees demandées a chaque tour
    char couleur;                     // Couleur d'une pièce
    int  points[]={0, 0};             // Points obtenus pour les blancs et les noirs
    int  peutJouer[]={1, 1};

    ecrire("\tOloNO  Copyright (C) 2008  David Wagner && Martin Potier\n\
\tThis program comes with ABSOLUTELY NO WARRANTY;\n\
\tThis is free software, and you are welcome to redistribute it under\n\
\tcertain conditions\n\n");

    ecrire("Bienvenue à OloNO\nOloNO is not an othello\n\n");

    // Si l'argument --taille-plateau ou -t est placé alors il override la taille par défaut.
    if (arguments.argTaillePlateau)
        taillePlateau = demanderTaillePlateau();

    // On créé un tableau à la taille demandée par le joueur
    CASE ** plateau = createPlateau(taillePlateau);
    // On initialise ce tableau avec la consigne
    initPlateau(plateau,taillePlateau);
    // Affichage, tableau initial :
#if 0
    int listeCoups[81];
    affichePlateau(plateau, taillePlateau);
    demanderXY(coords, taillePlateau);
    int nbCoups = valide(coords[0],coords[1],BLANC,plateau,taillePlateau,listeCoups);
    if (nbCoups)// Pour les tests: d'abord jouer une case valide
    {
        ecrire("Coup Valide\n");
        if (arguments.verbose == 1)
			ecrire("nombre de pièces à retourner: %d\n", nbCoups);
        for (i=0; i<nbCoups; i++)
        {
            retourne(listeCoups[i]/taillePlateau,listeCoups[i]%taillePlateau,plateau,taillePlateau);
            // Puis une case invalide pour observer l'inversion
        }
        //plateau[coords[1]][coords[0]] = BLANC;
    }
    ecrire("\n");
    affichePlateau(plateau, taillePlateau);

#else
    ///////////////////////////
    /*
     * Boucle Principale Du Jeu
     */
    ///////////////////////////
    couleur = BLANC;
    do
    {
        affichePlateau(plateau,taillePlateau);
        ecrire("A %s de jouer\n", couleur ? "Noir" : "Blanc");
        peutJouer[couleur] = jouerTour(couleur, plateau, taillePlateau);

        if (!peutJouer[couleur])
        {
            ecrire("Vous ne pouvez pas jouer, désolé.\n");

            if (!peutJouer[!couleur]) // Si en plus l'autre joueur ne pouvait pas jouer
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
    while (!testFinPartie(plateau, taillePlateau));

    score(plateau, taillePlateau, points);
    affichePlateau(plateau, taillePlateau);

    if (points[0] > points[1])
        ecrire("Les blancs gagnent par %d à %d\n", points[0], points[1]);
    else if (points[1] > points[0])
        ecrire("Les noirs gagnent par %d à %d\n", points[1], points[0]);
    else
        ecrire("Partie nulle: %d partout\n", points[0]);
#endif

    // On oublie pas de libérer l'espace mallocé
    for (i=0; i<taillePlateau*taillePlateau; i++)
        free( plateau[i] );
    free(plateau);
    return 0;
}

