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

#include <stdio.h>
#include "libDefine.h"
#include "libCases.h"

extern arguments args;
extern int taillePlateau;

//! retourne l'adresse de la case (x, y)
/**
 * En plus de retourner un pointeur sur la case (x,y), elle peut permettre de
 * vérifier la validité des coordonnées (retourne NULL si la case est hors du
 * plateau.)
 */
CASE * adresseParXY(int x, int y, CASE *plateau[])
{
    if (x>=taillePlateau || y>=taillePlateau
        || x<0 || y<0)
        return NULL;

    return plateau[x*taillePlateau + y];
}

//! Renvoie un pointeur sur la case suivante selon un vecteur
/**
 * Prend une case et un vecteur en argument, ainsi que la taille du plateau et
 * la liste des cases. Renvoie l'adresse de la case suivante sur ce vecteur
 */
CASE * caseSuivante(CASE depart, int incX, int incY, CASE *plateau[])
{
    int x, y;
    
    // Si on arrive au bout du plateau (depart.x+incX = taillePlateau), on
    // repart à 0. idem pour y
    x = depart.x + incX;
    if (x == taillePlateau)
    {
        if (args.Tore)
            x = 0;
        else
            return NULL;
    }
    else if (x == -1)
    {
        if (args.Tore)
            x = taillePlateau-1;
        else
            return NULL;
    }

    y = depart.y + incY;
    if (y == taillePlateau)
    {
        if (args.Tore)
            y = 0;
        else
            return NULL;
    }
    else if (y == -1)
    {
        if (args.Tore)
            y = taillePlateau-1;
        else
            return NULL;
    }

    return adresseParXY(x, y, plateau);
}
