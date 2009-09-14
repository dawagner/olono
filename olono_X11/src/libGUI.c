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
 * libGUI.c
 * Contient les fonctions de callback de l'interface graphique ainsi que des
 * fonctions annexes appelées par les fonctions de callback.
 */


#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "libDefine.h"
#include "libDisplay.h"
#include "libGUI.h"
#include "libCases.h"
#include "libPlay.h"
#include "libIA.h"


extern int          couleurDepart;
extern int          allowIA;
extern int          couleurIA;
extern int          affJouable;
extern int          taillePlateau;

extern int          jeton;
extern int          aPuJouer[];
extern window       * wOlono;
extern CASE         **plateau;
extern arguments    args;
extern preferences  prefs;
extern NOEUD        *tronc;
extern int          points [];
extern FILE         *fichierPreferences;

/// ICI Section fonction annexe

//! Cette fonction se charge de recopier le tableau en mémoire.
/**
 * Cette fonction rafraichit le tableau (GtkTable) par rapport à son
 * homologue en mémoire (structure CASE).
 *
 * Elle prend en compte deux arguments, le plateau en dur et la fenetre olono
 */
void refreshBoard (CASE ** plateau)
{
    // Les variables locales de boucle :
    gint n,i,j;

    for (i=0;i<taillePlateau;i++)
    {
        for (j=0;j<taillePlateau;j++)
        {
            n=i*taillePlateau+j;
            switch (adresseParXY(i,j,plateau)->couleur)
            {
            case JOKER:
                gtk_image_set_from_pixbuf (GTK_IMAGE(wOlono->pImage[n]), wOlono->imgJoker);
                break;

            case BLANC:
                gtk_image_set_from_pixbuf (GTK_IMAGE(wOlono->pImage[n]), wOlono->imgBlanche);
                break;

            case NOIR:
                gtk_image_set_from_pixbuf (GTK_IMAGE(wOlono->pImage[n]), wOlono->imgNoire);
                break;

            case VIDE:
                gtk_image_set_from_pixbuf (GTK_IMAGE(wOlono->pImage[n]), wOlono->imgVide);
                break;

            default:
                break;
            }
        }
    }
}

//! La fonction suivante sert à montrer les coups possibles sur le plateau actuel
/**
 * Cette fonction liste et charge les images pour montrer les coups possibles
 * pour la couleur donnée sur le plateau donné.
 * Ces coups seront directement affichés avec un mépris non dissimulé pour cette
 * piètre fonction de bête recopie qu'est refreshBoard(plateau).
 */
void montrerCoupsPossibles(char jeton, CASE ** plateau)
{
    int * listeValides;
    int nbCoups,i,n;

    listeValides = (int *) malloc(taillePlateau*taillePlateau*sizeof(int));
    nbCoups = nbCoupsValides(jeton,plateau,listeValides);

    for (i=0;i<nbCoups;i++)
    {
        n = listeValides[i];
        switch (jeton)
        {
        case BLANC:
            gtk_image_set_from_pixbuf (GTK_IMAGE(wOlono->pImage[n]), wOlono->imgTransBlanche);
            break;

        case NOIR:
            gtk_image_set_from_pixbuf (GTK_IMAGE(wOlono->pImage[n]), wOlono->imgTransNoire);
            break;
        }
    }
}


//! Cette fonction se charge de jouer le tour de l'IA :
/**
 *
 */
void joueTourIA (void)
{
    // On isole le plateau pendant que l'IA joue
    gtk_grab_add(wOlono->pPlateau);

    int     x=0,y=0;
    char    message[100];

    sprintf(message,"%s joue. Calcul du coup ...", couleurIA ? "Noir" : "Blanc");

    // On fait un pop/push pour éviter de remplir la pile. Et puis à qui
    // elle sert cette *** de pile ?
    gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                       GPOINTER_TO_INT(wOlono->contextId01));
    gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                        GPOINTER_TO_INT(wOlono->contextId01), message);

    // On force GTK a évaluer la situation :
    gtk_main_iteration ();

    do
    {
        meilleurXY(plateau,couleurIA,&x,&y);
        jouerCoup(x,y,couleurIA,plateau);
        majHistorique(plateau,couleurIA);

        refreshBoard(plateau);

        // On force GTK a évaluer la situation :
        gtk_main_iteration ();

        // Si le suivant a un coup, on le laisse jouer :
        if (peutJouer(!couleurIA,plateau)) // PAS DE ; !
            break;

        // Si jamais on est en fin de partie ie personne ne peut jouer :
        if (!peutJouer(jeton,plateau) && !peutJouer(!jeton,plateau))
        {
            if (args.verbose)
                ecrire("Fin de partie ...\n");
            score(plateau, points);

            if (points[BLANC] > points[NOIR])
                sprintf(message, "Fin de partie : les Blancs gagnent par %d contre %d.",
                        points[BLANC], points[NOIR]);
            else if (points[NOIR] > points[BLANC])
                sprintf(message, "Fin de partie : les Noirs gagnent par %d contre %d.",
                        points[NOIR], points[BLANC]);
            else
                sprintf(message, "Fin de partie : match nul, %d partout.", points[BLANC]);

            // On nettoie la variable des scores immédiatement après son affichage :
            // dans le cas d'un retour dans l'historique les points se trouvenet
            // curiosement aditionnés sinon :
            points[BLANC] = 0;
            points[NOIR] = 0;

            // On fait un pop/push pour éviter de remplir la pile. Et puis à qui
            // elle sert cette *** de pile ?
            // Pour le contexte 1, a qui le tour :
            gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                               GPOINTER_TO_INT(wOlono->contextId01));
            gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                                GPOINTER_TO_INT(wOlono->contextId01),"");

            // Pour le contexte 2, score :
            gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                               GPOINTER_TO_INT(wOlono->contextId02));
            gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                                GPOINTER_TO_INT(wOlono->contextId02),message);

            // Et on enlève le grab apres que l'IA ai fini de jouer
            gtk_grab_remove(wOlono->pPlateau);

            return;
        }
    }
    while (1);

    jeton = !jeton;

    // L'ia a fini de jouer, elle nous montre nos coups :
    (affJouable) ? montrerCoupsPossibles(jeton,plateau) : "";

    sprintf(message,"À %s de jouer", jeton ? "Noir" : "Blanc");

    // On fait un pop/push pour éviter de remplir la pile. Et puis à qui
    // elle sert cette *** de pile ?
    gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                       GPOINTER_TO_INT(wOlono->contextId01));
    gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                        GPOINTER_TO_INT(wOlono->contextId01), message);

    // Et on enlève le grab apres que l'IA ai fini de jouer
    gtk_grab_remove(wOlono->pPlateau);
}


//! Cette fonction se charge de vider tout ce qui a été mallocé :
/**
 * Cette fonction détruit toutes les références explicites en mémoire,
 * par exemple, les pixbuf, les tableau mallocé, etc.
 */
void freeEverything (void)
{
    int i;

    for (i=0; i<taillePlateau*taillePlateau; i++)
        free( plateau[i] );
    free(plateau);

    // On vire les pixbuf
    g_object_unref(wOlono->imgVide);
    g_object_unref(wOlono->imgNoire);
    g_object_unref(wOlono->imgBlanche);
    g_object_unref(wOlono->imgJoker);
    g_object_unref(wOlono->imgTransBlanche);
    g_object_unref(wOlono->imgTransNoire);

    // Et les autres choses qui trainent encore
    gtk_widget_destroy(wOlono->pPlateau);
    gtk_widget_destroy(wOlono->pWindow);

    // Avant de freeer la structure wOlono
    free(wOlono);
}











/*
 * #############################################################################
 * # PARTIE FONCTION DE CALLBACK : #############################################
 * #############################################################################
 */


//! Callback : Clic sur le bouton "Précédent"
void on_boutonPrecedent_clicked (GtkWidget * widget, gpointer data)
{
    // On prépare la variable du texte message :
    char message[100];

    // Tant qu'on ne peut pas jouer on recule dans l'historique :
    do
    {
        jeton = reculerHistorique(plateau);
    }
    while (!peutJouer(jeton,plateau));

    // Pas d'oubli du user :
    refreshBoard(plateau);
    (affJouable) ? montrerCoupsPossibles(jeton,plateau) : "";

    // On fait un pop/push pour éviter de remplir la pile. Et puis à quoi
    // elle sert cette *** de pile ?
    sprintf(message,"À %s de jouer", jeton ? "Noir" : "Blanc");
    gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                       GPOINTER_TO_INT(wOlono->contextId01));
    gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                        GPOINTER_TO_INT(wOlono->contextId01), message);
}

void on_boutonSuivant_clicked (GtkWidget * widget, gpointer data)
{
    // De même que pour boutonPrecedent :
    char message[100];

    // Tant qu'on ne peut pas jouer on avance dans l'historique :
    if (!peutJouer(jeton,plateau))
        jeton = !jeton;

    jeton = avancerHistorique(plateau);

    // Pas d'oubli du user :
    refreshBoard(plateau);
    (affJouable) ? montrerCoupsPossibles(jeton,plateau) : "";

    // Si jamais on est en fin de partie ie personne ne peut jouer :
    // Sinon on tombe en boucle infinie et ça c'est pas cool ...
    if (!peutJouer(jeton,plateau) && !peutJouer(!jeton,plateau))
    {
        score(plateau, points);
        if (points[BLANC] > points[NOIR])
            sprintf(message, "Fin de partie : les Blancs gagnent par %d contre %d.",
                    points[BLANC], points[NOIR]);
        else if (points[NOIR] > points[BLANC])
            sprintf(message, "Fin de partie : les Noirs gagnent par %d contre %d.",
                    points[NOIR], points[BLANC]);
        else
            sprintf(message, "Fin de partie : match nul, %d partout.", points[BLANC]);

        // On nettoie la variable des scores immédiatement après son affichage :
        // dans le cas d'un retour dans l'historique les points se trouvenet
        // curiosement aditionnés sinon :
        points[BLANC] = 0;
        points[NOIR] = 0;

        // On fait un pop/push pour éviter de remplir la pile. Et puis à qui
        // elle sert cette *** de pile ?
        // Pour le contexte 1, a qui le tour :
        gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                           GPOINTER_TO_INT(wOlono->contextId01));
        gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                            GPOINTER_TO_INT(wOlono->contextId01),"");

        // Pour le contexte 2, score :
        gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                           GPOINTER_TO_INT(wOlono->contextId02));
        gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                            GPOINTER_TO_INT(wOlono->contextId02),message);

        return;
    }
    //} while (!peutJouer(jeton,plateau));

    // On fait un pop/push pour éviter de remplir la pile. Et puis à quoi
    // elle sert cette *** de pile ?
    sprintf(message,"À %s de jouer", jeton ? "Noir" : "Blanc");
    gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                       GPOINTER_TO_INT(wOlono->contextId01));
    gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                        GPOINTER_TO_INT(wOlono->contextId01), message);
}

void on_menuBoutonNouvellePartie_activate (GtkWidget * widget, gpointer data)
{
    // On créé une variable locale pour un éventuel message en statusbar ;
    char message[100];

    // On réinitialise le plateau;
    int i=0;
    for (i=0;i<(taillePlateau*taillePlateau);i++)
    {
        plateau[i]->x       = i / taillePlateau;
        plateau[i]->y       = i % taillePlateau;
        plateau[i]->couleur = VIDE;
    }
    initPlateau(plateau);

    // On réinitialise les points :
    points[BLANC] = 0;
    points[NOIR]  = 0;

    // On réinitialise aPuJouer[] :
    aPuJouer[BLANC] = 1;
    aPuJouer[NOIR]  = 1;

    // On dégage les commentaires en statusBar :
    // --- D'abord le message du score :
    gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                       GPOINTER_TO_INT(wOlono->contextId02));
    // --- Puis le dernier messages de tour :
    gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                       GPOINTER_TO_INT(wOlono->contextId01));

    // On charge les (éventuelles) nouvelles rêgles
    couleurDepart = prefs.couleurDepart;
    allowIA       = prefs.allowIA;
    couleurIA     = prefs.couleurIA;
    affJouable    = prefs.affJouable;
    //taillePlateau = prefs.taillePlateau;
    /*    On verra plus tard l'implémentation du changement de taille automatique */

    // Et aussi l'historique :
    endHistorique();
    tronc = initHistorique(plateau,couleurDepart);

    // Celui qui commence :
    jeton = couleurDepart;


    // On rafraichit l'affichage;
    refreshBoard(plateau);
    (affJouable) ? montrerCoupsPossibles(jeton,plateau) : "";

    // Message Ready !
    (args.verbose) ? ecrire("Nouvelle partie !\n") : "";

    sprintf(message,"Nouvelle partie, à %s de commencer", jeton ? "Noir" : "Blanc");

    // On fait un pop/push pour éviter de remplir la pile. Et puis à qui
    // elle sert cette *** de pile ?
    gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                       GPOINTER_TO_INT(wOlono->contextId01));
    gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                        GPOINTER_TO_INT(wOlono->contextId01), message);

    // Si l'IA a le jeton, alors elle commence :
    if (jeton == couleurIA)
        joueTourIA();
}

//! Callback 1
void on_menuBoutonQuitter_select (GtkWidget * widget, gpointer data)
{
    if (args.verbose)
        ecrire("Nooooooon, ne quitte pas ce merveilleux programme.\n");
}

void on_menuBoutonQuitter_activate (GtkWidget * widget, gpointer data)
{
    if (args.verbose)
        ecrire("Trop tard :(\n");
    freeEverything();
    endHistorique();
    gtk_main_quit();
}

void on_buttonAppliquer_activate (GtkWidget * widget, gpointer data)
{
    int nbBits = 0;

    // Enregistrons les préférences :
    fichierPreferences = fopen(".olonoPrefs", "wb+");
    nbBits     = fwrite(&prefs,sizeof(preferences),1,fichierPreferences);

    // On change dynamiquement ce qu'il est possible de changer :
    allowIA    = prefs.allowIA;
    couleurIA  = prefs.couleurIA;
    affJouable = prefs.affJouable;

    (args.verbose) ? ecrire("Données enregistrées ! %d information écrite\n",nbBits):"";

    // Force la fermeture de la fenetre, comme glade est pas très fort pour ça :
    gtk_widget_hide(wOlono->pPrefWindow);
    gtk_main_iteration();

    // Si l'IA a le jeton, vu qu'elle peut avoir changé de couleur, alors elle joue :
    if (jeton == couleurIA)
        joueTourIA();
}


void on_case_clic (GtkWidget * widget, GdkEventButton *event, gpointer data)
{
    // Les cases sont inactives tant que l'IA joue
    if (allowIA && jeton == couleurIA)
        return;

    gchar     message[100]; // Normalement on segfault pas avec ça.

    // On récupère un pointeur sur gint :
    CASE * caseActuelle = (CASE*) data;

    // getting the position;
    if (args.verbose)
    {
        ecrire("clicked at position : x:%d y:%d\n",caseActuelle->x,caseActuelle->y);
        ecrire("Etat de aPuJouer : %d %d\n",aPuJouer[BLANC],aPuJouer[NOIR]);
    }

    // Essaye de jouer le coup, et enregistre si oui ou non il est possible.
    aPuJouer[jeton] = jouerCoup(caseActuelle->x,caseActuelle->y,jeton,plateau);

    // Si on a pas pu jouer :
    if (!aPuJouer[jeton])
    {
        if (args.verbose)
            ecrire("Coup illégal pour %s\n", jeton ? "Noir" : "Blanc");
        return;
    }
    else
    {
        if (args.verbose)
        {
            ecrire("Coup légal pour %s\n", jeton ? "Noir" : "Blanc");
        }
        refreshBoard(plateau);

        // On force GTK a évaluer la situation :
        gtk_main_iteration ();

    }

    // Si jamais on est en fin de partie ie personne ne peut jouer :
    if (!peutJouer(jeton,plateau) && !peutJouer(!jeton,plateau))
    {
        // On oublie pas de mettre à jour l'historique, des fois que certains
        // mécontent de leur mise à mort veuillent rejouer :
        majHistorique(plateau,jeton);

        if (args.verbose)
            ecrire("Fin de partie ...\n");
        score(plateau, points);

        if (points[BLANC] > points[NOIR])
            sprintf(message, "Fin de partie : les Blancs gagnent par %d contre %d.",
                    points[BLANC], points[NOIR]);
        else if (points[NOIR] > points[BLANC])
            sprintf(message, "Fin de partie : les Noirs gagnent par %d contre %d.",
                    points[NOIR], points[BLANC]);
        else
            sprintf(message, "Fin de partie : match nul, %d partout.", points[BLANC]);

        // On nettoie la variable des scores immédiatement après son affichage :
        // dans le cas d'un retour dans l'historique les points se trouvenet
        // curiosement aditionnés sinon :
        points[BLANC] = 0;
        points[NOIR] = 0;

        // On fait un pop/push pour éviter de remplir la pile. Et puis à qui
        // elle sert cette *** de pile ?
        // Pour le contexte 1, a qui le tour :
        gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                           GPOINTER_TO_INT(wOlono->contextId01));
        gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                            GPOINTER_TO_INT(wOlono->contextId01),"");

        // Pour le contexte 2, score :
        gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                           GPOINTER_TO_INT(wOlono->contextId02));
        gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                            GPOINTER_TO_INT(wOlono->contextId02),message);

        return;
    }

    // Si le suivant n'a pas de coup :
    if (!peutJouer(!jeton,plateau))
    {
        // On indique qu'il n'a pas pu jouer son tour, et on recommence le tour
        // actuel
        aPuJouer[!jeton] = 0;

        if (args.verbose)
            ecrire("Encore a %s de jouer\n", jeton ? "Noir" : "Blanc");

        (affJouable) ? montrerCoupsPossibles(jeton,plateau) : "";

        sprintf(message,"Encore à %s de jouer", jeton ? "Noir" : "Blanc");

        // On fait un pop/push pour éviter de remplir la pile. Et puis à qui
        // elle sert cette *** de pile ?
        gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                           GPOINTER_TO_INT(wOlono->contextId01));
        gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                            GPOINTER_TO_INT(wOlono->contextId01), message);

        // Mise à jour de l'historique de jeu, d'abord, le coup actuel de jeton,
        // puis de l'autre !jeton qui n'a pas pu jouer.
        majHistorique(plateau,jeton);
        majHistorique(plateau,!jeton);

        return;
    }

    // Si tout s'est déroulé normalement, on passe au joueur suivant :
    majHistorique(plateau,jeton);
    jeton = !jeton;

    if (args.verbose)
        ecrire("A %s de jouer\n", jeton ? "Noir" : "Blanc");

    sprintf(message,"À %s de jouer", jeton ? "Noir" : "Blanc");

    // On fait un pop/push pour éviter de remplir la pile. Et puis à qui
    // elle sert cette *** de pile ?
    gtk_statusbar_pop (GTK_STATUSBAR (wOlono->pStatusBar),
                       GPOINTER_TO_INT(wOlono->contextId01));
    gtk_statusbar_push (GTK_STATUSBAR (wOlono->pStatusBar),
                        GPOINTER_TO_INT(wOlono->contextId01), message);

    refreshBoard (plateau);
    (affJouable) ? montrerCoupsPossibles(jeton,plateau) : "";

    // On force GTK a évaluer la situation :
    gtk_main_iteration ();

    /* **************************************** */
    /* AU TOUR DE L'IA, si elle peut jouer :    */
    /* **************************************** */
    // On sort si l'IA est pas sensé jouer
    if (!allowIA)
        return;

    joueTourIA();
}


void on_allowIA_toggled (GtkWidget * widget, gpointer data)
{
    // On active/désactive la case cochée selon les préférences en cours :
    prefs.allowIA = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (wOlono->pCheckAllowIA));
    args.verbose ? ecrire("%s de la propriété allowIA.\n", prefs.allowIA ? "Ajout" : "Retrait") : "";
}

void on_iaIsBlack_toggled (GtkWidget * widget, gpointer data)
{
    // On active/désactive la case cochée selon les préférences en cours :
    prefs.couleurIA = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (wOlono->pCheckiaIsBlack));
    args.verbose ? ecrire("L'ordinateur joue les %s.\n", prefs.couleurIA ? "Noirs" : "Blancs") : "";
}

void on_showPossibleMoves_toggled (GtkWidget * widget, gpointer data)
{
    // On active/désactive la case cochée selon les préférences en cours :
    prefs.affJouable = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (wOlono->pCheckAffJouable));
    args.verbose ? ecrire("%s de la propriété afficher coups jouables.\n", prefs.affJouable ? "Ajout" : "Retrait") : "";
}

void on_whiteStart_toggled (GtkWidget * widget, gpointer data)
{
    // On active/désactive la case cochée selon les préférences en cours :
    prefs.couleurDepart = !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (wOlono->pCheckCouleurDeb));
    args.verbose ? ecrire("Les %s commencent.\n", prefs.couleurDepart ? "Noirs" : "Blancs") : "";
}

void on_taillePlateau_value_changed (GtkWidget * widget, gpointer data)
{
    prefs.taillePlateau = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (wOlono->pSpinTT));
    args.verbose ? ecrire("Valeur de taillePlateau fixée à %d.\n", prefs.taillePlateau) : "" ;
}

