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
 * libDisplay.h
 * Prototypes de libDisplay.c
 */

#ifndef _LIB_DISPLAY_H
#define _LIB_DISPLAY_H

#include "libCases.h"

extern int taillePlateau;

char demanderAction(char couleur);
void demanderXY(int coords[]);
void affichePlateau(CASE *plateau[]);
void direBonjour();
void demanderXY(int coords[]);
void ecrire(char *chaine, ...);
void erreur(char *chaine, ...);
int demanderTaillePlateau();
void envoyerXY(int x, int y);
void pasDeCoup();
int lireCoup(char * x, int * y);
void afficherScore(int points[]);

#endif

