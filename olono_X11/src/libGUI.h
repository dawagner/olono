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
 * libGUI.h
 * Contient les entêtes des fonctions de libGUI.c
 */

/// Fichier de définition du package et des paths :
/// On pourra remplacer par un système de gestion dynamique de vérif, permettant de bouger le
/// programme sans le recompiler. Mais plus tard ...
#include "config.h"
#include "libCases.h"

#define GLADEDIR     "olono-gtk/glade"
#define GLADEFILE    "olonoGUI.glade"
#define IMGDIR       "olono-gtk/images"
#define IMGVIDE      "olono_caseV.png"
#define IMGBLANCHE   "olono_caseB.png"
#define IMGTBLANCHE  "olono_caseTB.png"
#define IMGNOIRE     "olono_caseN.png"
#define IMGTNOIRE    "olono_caseTN.png"
#define IMGJOKER     "olono_caseJ.png"

typedef struct _WINDOW
{
    GtkWidget   *pPlateau;
    GtkWidget   *pImage[625];
    GtkWidget   *pEvent[625];
    GtkWidget   *pWindow;
    GtkWidget   *pPrefWindow;
    GtkWidget   *pContainer;
    GtkWidget   *pStatusBar;
    GtkWidget   *pCheckAllowIA;
    GtkWidget   *pCheckiaIsBlack;
    GtkWidget   *pCheckAffJouable;
    GtkWidget   *pCheckCouleurDeb;
    GtkWidget   *pSpinTT;
    GdkPixbuf   *imgBlanche;
    GdkPixbuf   *imgTransBlanche;
    GdkPixbuf   *imgNoire;
    GdkPixbuf   *imgTransNoire;
    GdkPixbuf   *imgVide;
    GdkPixbuf   *imgJoker;
    GError      *gerror;
    guint        contextId01;
    guint        contextId02;
} window;

// Fonctions de modification annexe :
void refreshBoard                   (CASE ** plateau);
void montrerCoupsPossibles          (char couleur, CASE ** plateau);
void freeEverything                 (void);

// Fonctions de callback :
void on_boutonPrecedent_clicked             (GtkWidget * widget, gpointer data);
void on_boutonSuivant_clicked               (GtkWidget * widget, gpointer data);
void on_menuBoutonNouvellePartie_activate   (GtkWidget * widget, gpointer data);
void on_menuBoutonQuitter_select            (GtkWidget * widget, gpointer data);
void on_menuBoutonQuitter_activate          (GtkWidget * widget, gpointer data);
void on_case_clic                           (GtkWidget * widget,
        GdkEventButton *event, gpointer data);
void on_allowIA_toggled                     (GtkWidget * widget, gpointer data);
void on_iaIsBlack_toggled                   (GtkWidget * widget, gpointer data);
void on_showPossibleMoves_toggled           (GtkWidget * widget, gpointer data);
void on_whiteStart_toggled                  (GtkWidget * widget, gpointer data);
void on_taillePlateau_value_changed         (GtkWidget * widget, gpointer data);

