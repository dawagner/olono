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

#include "libCases.h"

char valide(int x, int y, char couleur, struct _CASE *plateau[], int taillePlateau, int listeCoups[]);
void retourne(int x, int y, struct _CASE *plateau[],int taillePlateau);
int caseDansLeTableau(int x, int y, int taillePlateau);
int jouerTour(char couleur, struct _CASE *plateau[], int taillePlateau);
int testFinPartie(struct _CASE *plateau[], int taillePlateau);
void score(struct _CASE *plateau[], int taillePlateau, int * points);
struct _CASE ** createPlateau(int taillePlateau);
void initPlateau(struct _CASE *plateau[],int taillePlateau);

