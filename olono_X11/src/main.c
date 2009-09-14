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

/// Hey, passage en graphique
#include <gtk/gtk.h>
#include <glade/glade.h>


/// On inclu nos librairies personnelles (attention le chemin est par rapport à .
/// pas par rapport à BASEDIR, et ce, malgrès le make général ! ):
#include "libDefine.h"
#include "libGUI.h"
#include "libIA.h"
#include "libPlay.h"
#include "libCases.h"
#include "libDisplay.h"

/// Certains compilos définissent _WIN32, d'autres __WIN32
#ifdef  _WIN32
#ifndef __WIN32
#define __WIN32
#endif
#endif

/// INIT 1 : Préférences globales (built-in default)
int    couleurDepart = BLANC;   /// Couleur de départ par défaut;
int    allowIA       = 0;       /// Faire jouer l'IA;
int    couleurIA     = NOIR;    /// Couleur de l'IA
int    affJouable    = 0;       /// Afficher les positions jouables;
int    taillePlateau = 8;       /// Taille du plateau par défaut;

/// Variables globales :
int    jeton         = BLANC;   /// Comme la couleur de départ;
int    aPuJouer[]    = {1, 1};  /// Indique que les blancs et les noirs peuvent jouer
int    points[]      = {0, 0};  /// Points obtenus pour les blancs et les noirs

window      *wOlono;            /// Les widgets sont accessibles de n'importe où.
struct      _CASE **plateau;
arguments   args;
NOEUD       *tronc;
preferences prefs;

FILE * fichierPreferences = NULL;


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
    {"taille-plateau", 't', "taillePlateau", 0, "Changer la taille de plateau par défaut (9)"},
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
        args->taillePlateau = (int) arg;
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

const char *argp_program_version =
    "OloNO " VERSION " - édition 42";

const char *argp_program_bug_address =
    "<dwagner@isep.fr> &| <mpotier@isep.fr>";

//}
// ############################################################### //




///! Lance le programme
/**
 * Cette fonction est la boucle principale du programme.
 * Elle permet de rassembler toutes les autres fonctions dans un contenu reflétant
 * le fonctionnement du programme. Pour comprendre le fonctionnement d'un programme,
 * lire le main() devrait suffire.
 * Dans notre cas en revanche ce n'est pas tout à fait exact, car gtk implique
 * une execution évènementielle, c'est à dire une éxécution basée sur les
 * évènements. Le programme ne se lit donc plus du haut vers le bas, mais en
 * fonction des évènements liés à des fonctions dites de callback, dans le fichier
 * libGUI.c.
 */
int main(int argc, char ** argv)
{

    // Commençons par charger les préférences, si elles existent. On ouvre le fichier
    // en lecture pour charger la structure préférences :
    fichierPreferences = fopen(".olonoPrefs", "rb");
    if (fichierPreferences == NULL)
        erreur("Pas de fichier de préférences trouvé\n");
    else
    {
        // On lit 1 élément de donnée de longueur preferences depuis le fichier
        // fPreferences pour le stocker dans la variable prefs (de type preferences)
        fread(&prefs,sizeof(preferences),1,fichierPreferences);

        /// INIT 2 : Préférences globales (from file)
        couleurDepart = prefs.couleurDepart;
        allowIA       = prefs.allowIA;
        couleurIA     = prefs.couleurIA;
        affJouable    = prefs.affJouable;
        taillePlateau = prefs.taillePlateau;

        if (args.verbose)
            ecrire("Chargement de :\n\t couleurDepart: %d, allowIA      : %d\n\t couleurIA    : %d, taillePlateau: %d",couleurDepart,allowIA,couleurIA,taillePlateau);
    }



    /* On initialise les arguments à leur valeur par défaut */
    args.taillePlateau = 0;
    args.verbose       = 0;
    args.Tore          = 0;

    /* This is where the magic should happen | Attention, instant magique ... */
    argp_parse (&argp, argc, argv, 0, 0, &args);

    /// INIT 3 : Préférences globales (from)
    if (args.taillePlateau != 0)
        if (atoi((char*) args.taillePlateau) <= 25 && atoi((char*) args.taillePlateau) >= 4)
            taillePlateau = atoi((char*) args.taillePlateau);


    /// Les 3 priorités d'initialisation étant terminées, on peut désormais
    //  assigner la couleur de départ du jeton :
    jeton = couleurDepart;


/// ######################################################################## ///
/// ### On commence les choses sérieuses ici (cad le main) ################# ///
/// ######################################################################## ///

    gint    i=0,j=0,n=0;
    CASE    caseActuelle[625];

    wOlono = g_malloc(sizeof(window));

    plateau = createPlateau();
    initPlateau(plateau);
    tronc = initHistorique(plateau,couleurDepart);


/// ######################################################################## ///
/// ### On fait chauffer l'Interface Graphique ############################# ///
/// ######################################################################## ///
    GladeXML    *xml;

    /* Initialisation de la lib GTK+ */
    gtk_init(&argc, &argv);

    /* load the interface and complete it with the variable pieces */
    xml = glade_xml_new(DATADIR "/" GLADEDIR "/" GLADEFILE, NULL, NULL);

    /* connect the signals in the interface */
    glade_xml_signal_autoconnect(xml);

    /*    // timer up*/
    /*    wOlono->pIAClock = g_timer_new();*/
    /*    g_timer_start(wOlono->pIAClock);*/

    // get the main Window
    wOlono->pWindow     = glade_xml_get_widget(xml, "olonoWindow");
    gtk_window_set_title(GTK_WINDOW (wOlono->pWindow), "OloNO v" VERSION);

    // get checkBoxes, and set their values
    wOlono->pCheckAllowIA = glade_xml_get_widget(xml, "allowIA");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (wOlono->pCheckAllowIA),allowIA);
    wOlono->pCheckiaIsBlack = glade_xml_get_widget(xml, "iaIsBlack");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (wOlono->pCheckiaIsBlack),couleurIA);
    wOlono->pCheckAffJouable = glade_xml_get_widget(xml, "showPossibleMoves");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (wOlono->pCheckAffJouable),affJouable);
    wOlono->pCheckCouleurDeb = glade_xml_get_widget(xml, "whiteStart");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (wOlono->pCheckCouleurDeb),!couleurDepart);

    // also get the spin button and set the default value :
    wOlono->pSpinTT = glade_xml_get_widget(xml, "taillePlateau");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON (wOlono->pSpinTT),taillePlateau);

    // why not speak to the prefs window too ?
    wOlono->pPrefWindow = glade_xml_get_widget(xml, "olonoPreferences");

    // get the status bar :
    wOlono->pStatusBar  = glade_xml_get_widget(xml, "barreDesTaches");

    // creating context ids :
    wOlono->contextId01 = gtk_statusbar_get_context_id(GTK_STATUSBAR(wOlono->pStatusBar),
                          "MsgTour");
    wOlono->contextId02 = gtk_statusbar_get_context_id(GTK_STATUSBAR(wOlono->pStatusBar),
                          "MsgScore");

    // creating first blank message for contextId01 :
    gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                        GPOINTER_TO_INT(wOlono->contextId01),"Bienvenue - Youkoso");

    // On s'occupe de charger les images en mémoire :
    wOlono->imgVide     = gdk_pixbuf_new_from_file(DATADIR "/" IMGDIR "/" IMGVIDE,&wOlono->gerror);
    wOlono->imgBlanche  = gdk_pixbuf_new_from_file(DATADIR "/" IMGDIR "/" IMGBLANCHE,&wOlono->gerror);
    wOlono->imgNoire    = gdk_pixbuf_new_from_file(DATADIR "/" IMGDIR "/" IMGNOIRE,&wOlono->gerror);
    wOlono->imgJoker    = gdk_pixbuf_new_from_file(DATADIR "/" IMGDIR "/" IMGJOKER,&wOlono->gerror);

    // Puis on créé les dérivées transparentes :
    wOlono->imgTransBlanche = gdk_pixbuf_new_from_file(DATADIR "/" IMGDIR "/" IMGTBLANCHE,&wOlono->gerror);
    wOlono->imgTransNoire   = gdk_pixbuf_new_from_file(DATADIR "/" IMGDIR "/" IMGTNOIRE,&wOlono->gerror);


    // set the plateau.
    wOlono->pPlateau    = gtk_table_new(taillePlateau,taillePlateau,TRUE);

    for (i=0;i<taillePlateau;i++)
    {
        for (j=0;j<taillePlateau;j++)
        {
            n=i*taillePlateau+j;
            wOlono->pImage[n] = gtk_image_new();
            wOlono->pEvent[n] = gtk_event_box_new ();
            gtk_container_add (GTK_CONTAINER (wOlono->pEvent[n]), wOlono->pImage[n]);

            // Important : On place le gtk_event_box au dessus sinon on peut pas
            // recevoir des évènements, puisqu'il est caché par l'image.
            gtk_event_box_set_above_child (GTK_EVENT_BOX (wOlono->pEvent[n]),1);

            // On rempli caseActuelle :
            caseActuelle[n].x = i;
            caseActuelle[n].y = j;

            // On connecte chaque signal à la fonction callback :
            g_signal_connect (G_OBJECT (wOlono->pEvent[n]),
                              "button_press_event",
                              G_CALLBACK (on_case_clic),
                              &caseActuelle[n]);

            gtk_table_attach_defaults (GTK_TABLE (wOlono->pPlateau),
                                       wOlono->pEvent[n], i, i+1, j, j+1);
        }
    }

    /* Rafraichit le plateau (fait concorder celui en mémoire et celui affiché) */
    refreshBoard(plateau);

    wOlono->pContainer = glade_xml_get_widget(xml, "cadreAspect");
    gtk_container_add(GTK_CONTAINER(wOlono->pContainer), wOlono->pPlateau);

    /* For all the abandonned widgets ie plateau */
    gtk_widget_show_all (wOlono->pPlateau);

    /* start the event loop */
    gtk_main();

    return 0;
}

