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

#include <stdlib.h>
#include <time.h>
#include "libDefine.h"
#include "libIA.h"
#include "libPlay.h"
#include "libDisplay.h" // Pour les messages de debug

extern int taillePlateau;
extern arguments args;

//! Place les meilleures coordonnées dans x et y
int meilleurXY(CASE *plateau[], char couleur, int * x, int * y)
{
    NOEUD noeud; // On crée le noeud-père
    noeud.couleur = !couleur; // C'est l'adversaire qui vient de jouer
    
    // On récupère ici la valeur du plateau.
    // On ne s'en sert actuellement pas mais bon...
    int valeur = minmax(plateau, &noeud, 6, -INFINI, INFINI);
    if (args.verbose)
        erreur("Veleur supposée du coup: %d\n", valeur);

    // On récupère les coordonnées du meilleur coup directement fils
    *x = noeud.meilleurX;
    *y = noeud.meilleurY;

    return 0;
}

//! Calcule l'arbre des possibilités et renvoie la valeur du noeud en cours
/**
 * Implémentation de l'algorithme Minimax
 * S'appelle récursivement pour calculer la valeur d'un noeud à partir des
 * noeuds fils.
 * Si un noeud est terminal, sa valeur est sa valeur heuristique (pour le
 * moment, c'est tout simplement le différentiel de score. Il n'y a pas de
 * fonction heuristique). Sinon, elle est égale à la plus basse valeur des
 * fils. (Grâce à la simplification Negamax, pas besoin d'alterner entre plus
 * bas et plus haut selon qu'on est sur un noeud ennemi ou allié).
 * A venir: heuristique; élagage
 */
int minmax(CASE *plateau[], NOEUD * noeud, int profondeur, int alpha, int beta)
{
    if (profondeur <= 0) // Si on est un noeud terminal on renvoie la valeur du plateau
    {
        int points[] = {0, 0};
        score(plateau, points); // Pour le moment, il n'y a pas de fonction heuristique, on prend le score brut
        return points[(int) noeud->couleur] - points[(int) !(noeud->couleur)]; // Valeur du noeud;
    }
    
    int index;
    int i, j;
    int * listeValides = malloc(taillePlateau * taillePlateau * sizeof(int));
    
    // On calcule le nombre de fils et on alloue la liste des fils en
    // onséquence.
    noeud->nbDeFils = nbCoupsValides(!(noeud->couleur), plateau, listeValides);
    //if (noeud->nbDeFils > 2)
    //    noeud->nbDeFils = 2;
    if (noeud->nbDeFils)
        noeud->listeFils = calloc(noeud->nbDeFils, sizeof(NOEUD *)); // calloc met la mémoire à 0
    else
        noeud->listeFils = malloc(1*sizeof(NOEUD)); // Si le joueur ne peut pas jouer, il passe: 1 noeud

    for(i=0; i<(noeud->nbDeFils); i++)
    {
        // On alloue le noeud fils
        (noeud->listeFils)[i] = malloc(sizeof(NOEUD));
        NOEUD * fils = (noeud->listeFils)[i];
        fils->listeFils = NULL;
        fils->nbDeFils = 0;

        // On crée une copie de travail du plateau
        // (Très consommateur de mémoire)
        fils->plateau = copiePlateau(plateau);

        // On calcule ses coordonnées
        fils->x = listeValides[i]/taillePlateau;
        fils->y = listeValides[i]%taillePlateau;
        fils->couleur = !(noeud->couleur);

        // On simule le coup sur la copie
        jouerCoup(fils->x, fils->y, fils->couleur, fils->plateau);

        // Convention Negamax
        fils->valeur = -minmax(fils->plateau, fils, profondeur-1, -beta, -alpha);

        // Libérez la mémoire! Libérez la mémoire!
        for(j=0; j<(taillePlateau*taillePlateau); j++)
            free(fils->plateau[j]);
        free(fils->plateau);

        // Coupure alpha/beta:
        if (fils->valeur > alpha)
        {
            noeud->meilleurX = fils->x;
            noeud->meilleurY = fils->y;
            alpha = fils->valeur;
            index = i;
            if (alpha >= beta)
            {
                //On élague !
                break;
            }
        }
    }

    if (noeud->nbDeFils == 0) // Je ne peux pas jouer
    {
        noeud->nbDeFils = 1; // J'ai donc une seule branche: passer
        
        (noeud->listeFils)[0] = malloc(sizeof(NOEUD));
        NOEUD * fils = (noeud->listeFils)[0];
        fils->listeFils = NULL;
        fils->nbDeFils = 0;
        fils->plateau = copiePlateau(plateau);

        fils->couleur = !(noeud->couleur);
        fils->x = -1; // Inutile pour le moment
        fils->y = -1;

        // Je saute donc l'étape "jouer le coup"
        // Et je calule la valeur des fils
        fils->valeur = -minmax(fils->plateau, fils, profondeur-1, -beta, -alpha);
        alpha = fils->valeur;
        for(j=0; j<(taillePlateau*taillePlateau); j++)
            free(fils->plateau[j]);
        free(fils->plateau);
    }

    
    // Le Front de Libération de la Mémoire intervient

    free(listeValides);

    for(i=0; i<(noeud->nbDeFils); i++)
        free((noeud->listeFils)[i]);
    free(noeud->listeFils);

    return alpha;
}

