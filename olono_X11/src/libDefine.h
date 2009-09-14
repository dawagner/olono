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
 * libDefine.h
 * Contient des macros et des structures pour tout le programme.
 * Elles définissent les valeurs d'int associées aux 4 types de cases. De la
 * même manière est définit la taille d'un plateau par défaut.
 */

#ifndef _LIB_DEFINE_H
#define _LIB_DEFINE_H

#define VIDE  3
#define BLANC 0
#define NOIR  1
#define JOKER 2

#define INFINI 0x7FFFFFFF // 2^31

#define abs(X)              ((X) > 0 ? (X) : -(X))
#define max(X, Y)           ((X) > (Y) ? (X) : (Y))
#define min(X, Y)           ((X) < (Y) ? (X) : (Y))

#define proportion(X, Y)    ((X)) / ((X)+(Y))

/* This structure is used by main to communicate with parse_opt. */
typedef struct _arguments
{
    int  taillePlateau;    /* Arguments for -t */
    char verbose;          /* 1 si on doit aficher la grille, 0 sinon */
    char type[7];          /* humain ou ia */
    char couleur;          /* 'N' ou 'B' */
    int  Tore;             /* Arguments for -T */
} arguments;

typedef struct _preferences
{
    int couleurDepart;
    int allowIA;
    int couleurIA;
    int affJouable;
    int taillePlateau;
} preferences;

#endif

