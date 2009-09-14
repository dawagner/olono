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
 * libPlay.h
 * Prototypes de libPlay.c
 */

#ifndef _LIB_PLAY_H
#define _LIB_PLAY_H

#include "libCases.h"

char lister(int x, int y, char couleur, struct _CASE *plateau[], int listeCoups[]);
char valide(int x, int y, char couleur, struct _CASE *plateau[]);
char nbCoupsValides(char couleur, CASE *plateau[], int listeValides[]);
void retourne(int x, int y, struct _CASE *plateau[]);
int peutJouer(char couleur, CASE *plateau[]);
int jouerCoup(int x, int y, char couleur, struct _CASE *plateau[]);
int testFinPartie(struct _CASE *plateau[]);
void score(struct _CASE *plateau[], int * points);
struct _CASE ** createPlateau();
struct _CASE ** copiePlateau(struct _CASE *plateau[]);
void initPlateau(struct _CASE *plateau[]);
int jouerSonTour(char couleur, CASE *plateau[]);
int jouerTourAdverse(char couleur, CASE *plateau[]);
int jouerTourSimple(char couleur, CASE *plateau[]);
NOEUD * initHistorique(CASE *plateau[], char couleur);
NOEUD * majHistorique(CASE *plateau[], char couleur);
void endHistorique();
char reculerHistorique(CASE * plateau[]);
char sauterHistorique(CASE * plateau[], NOEUD * destination);
char avancerHistorique(CASE * plateau[]);
void ajouteFils(NOEUD * pere, NOEUD * fils);

#endif

