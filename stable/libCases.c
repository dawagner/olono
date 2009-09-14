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

CASE * adresseParXY(int x, int y, CASE *plateau[], int taillePlateau)
{
    if (x>=taillePlateau || y>=taillePlateau
        || x<0 || y<0)
        return NULL;

    return plateau[x*taillePlateau + y];
}

/**
 * Prend une case et un vecteur en argument, ainsi que la taille du plateau et
 * la liste des cases. Renvoie l'adresse de la case suivante sur ce vecteur
 */
CASE * caseSuivante(CASE depart, int incX, int incY, CASE *plateau[], int taillePlateau)
{
    int x, y;
    
    // Si on arrive au bout du plateau (depart.x+incX = taillePlateau), on
    // repart à 0. idem pour y
    x = depart.x + incX;
    if (x == taillePlateau)
        x = 0;
    else if (x == -1)
        x = taillePlateau-1;

    y = depart.y + incY;
    if (y == taillePlateau)
        y = 0;
    else if (y == -1)
        y = taillePlateau-1;

    return adresseParXY(x, y, plateau, taillePlateau);
}
