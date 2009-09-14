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

// Comme certaines librairies incluent cette lib et des libs qui l'incluent
// aussi, il ne faut le lire qu'une fois
#ifndef _LIB_CASES_H
#define _LIB_CASES_H

#include "libDefine.h"

//! Structure de case.
typedef struct _CASE
{
    char couleur; /**< Couleur de la case */
    int x, y;
} CASE;

typedef struct _NOEUD
{
    int nbDeFils;
    struct _NOEUD ** listeFils; // liste des pointeurs sur les fils (de taille nbDeFils)
    struct _CASE ** plateau;
    char x, y;
    char valeur;
    char couleur;
    char meilleurX;
    char meilleurY;
    struct _NOEUD * pere;
} NOEUD;

CASE * adresseParXY(int x, int y, CASE *plateau[]);
CASE * caseSuivante(CASE depart, int incX, int incY, CASE *plateau[]);
#endif

