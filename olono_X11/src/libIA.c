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

    if (args.verbose)
        erreur("Valeur de la case: %d\n", valeurCase(*x, *y));

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
        int score = valeurPlateau(!noeud->couleur, plateau);
        return score;
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

int valeurPlateau(char couleur, CASE *plateau[])
{
    int i;

    int scorePondere[2]={0, 0}, scoreMobilite[2]={0, 0},
        scoreLignes[2]={0, 0}, scoreBrut[2]={0, 0};

    float totalLignes=0, totalMobilite=0,
          totalBrut=0, totalPondere=0;

    int fPondere , fMobilite,
        fLignes, fBrut; // les facteur qui viennent ponderer les scores
    /*
     * La mobilite prend plus d'importance au fur et a mesure qu'on avance dans
     * le jeu, idem pour le score brut mais l'importance vient plus
     * progressivement
     *
     * Le score pondéré prend de l'importance en milieu de jeu et en perd au
     * fur et à mesure
     *
     * Le score des lignes prend de l'importance en milieu de jeu et en gagne
     * petit a petit, mais jamais trop
     *
     * Le score brut prend de l'importance tardivement mais devient presque la
     * seule vers la fin
     */


    /* * * * *\
        BRUT 
    \* * * * */

    score(plateau, scoreBrut);
    if (scoreBrut[BLANC] + scoreBrut[NOIR] > 0) // Au moins un joueur a des lignes
    {
        // Le total est la proportion des points qu'a le joueur
        totalBrut = proportion((float) scoreBrut[(int) couleur], (float) scoreBrut[(int) !couleur]);
        fBrut = 1;
    }
    else // Personne n'a de ligne
    {
        fBrut = 0;
    }
    //erreur("Total Brut: %f\n", totalBrut);


    /* * * * *\
      PONDERE
    \* * * * */

    for (i=0; i<taillePlateau*taillePlateau; i++)
    {
        int x = i%taillePlateau, y=i/taillePlateau;
        char contenu = adresseParXY(x, y, plateau)->couleur;
        if (contenu == BLANC || contenu == NOIR)
            scorePondere[(int) contenu] += valeurCase(x, y);
        //erreur("Valeur de la case %d,%d: %d\n", x, y, valeurCase(x, y));
    }
    if (scoreBrut[BLANC] + scoreBrut[NOIR] > 0) // Au moins un joueur a des lignes
    {
        // Le total est la proportion des points qu'a le joueur
        totalPondere = proportion((float) scorePondere[(int) couleur], (float) scorePondere[(int) !couleur]);
        fPondere = 5;
    }
    else // Personne n'a de ligne
    {
        fPondere = 0;
    }
    //erreur("Total pondéré:%f\n", totalPondere);


    /* * * * *\
       LIGNES
    \* * * * */

    for(i=0; i<taillePlateau; i++)
    {
        int lin = valeurLigne(i, couleur, plateau);
        int col = valeurColonne(i, couleur, plateau);

        if (lin>0)
            scoreLignes[(int) couleur] += lin;
        else
            scoreLignes[(int) !couleur] -= lin;

        if (col>0)
            scoreLignes[(int) couleur] += col;
        else
            scoreLignes[(int) !couleur] -= col;
    }
    if (scoreLignes[BLANC] + scoreLignes[NOIR] > 0) // Au moins un joueur a des lignes
    {
        // Le total est la proportion des points qu'a le joueur
        totalLignes = proportion((float) scoreLignes[(int) couleur], (float) scoreLignes[(int) !couleur]);
        fLignes = 1.5;
        //erreur("Total Lignes:%f\n", totalLignes);
    }
    else // Personne n'a de ligne
    {
        fLignes = 0;
    }

    /* * * * *\
     MOBILITE
    \* * * * */
    int * temp = malloc(taillePlateau*taillePlateau*sizeof(int));
    scoreMobilite[BLANC] = nbCoupsValides(BLANC, plateau, temp);
    scoreMobilite[NOIR] = nbCoupsValides(NOIR, plateau, temp);
    free(temp);
    if (scoreMobilite[BLANC] + scoreMobilite[NOIR] > 0)
    {
        totalMobilite = proportion((float) scoreMobilite[(int) couleur], (float) scoreMobilite[(int) !couleur]);
        fMobilite = 2;
    }
    else
    {
        fMobilite = 0;
    }

    int retour = 100 * ((totalPondere*fPondere) +
                        (totalBrut*fBrut) +
                        (totalLignes*fLignes) +
                        (totalMobilite*fMobilite)) /
                        (fPondere+fBrut+fLignes+fMobilite);

    //erreur("Valeur du plateau suivant:%d", retour);
    //affichePlateau(plateau);

    return retour;
}

int valeurColonne(int colonne, char couleur, CASE *plateau[])
{
    int pions[4] = {0, 0, 0, 0}; // Seuls les deux premieres cases seront lues.
                                 // En avoir 4 permet seulement d'avoir moins
                                 // de tests
    int i;
    
    for (i=0; i<taillePlateau; i++)
        (pions[(int) adresseParXY(i, colonne, plateau)->couleur])++;

    if (pions[(int) couleur] + pions[JOKER] >= taillePlateau-1) // La colonne est au joueur: Il a toute la colonne (sauf éventuellement un point)
        return max(1, abs( (taillePlateau/2) - colonne)); // sans le max, renverrait 0 pour la colonne du milieu...

    // On renvoie un négatif si c'est l'ennemi
    if (pions[!couleur] + pions[JOKER] >= taillePlateau-1)
        return - max(1, abs( (taillePlateau/2) - colonne));

    return 0;
}

int valeurLigne(int ligne, char couleur, CASE *plateau[])
{
    int pions[4] = {0, 0, 0, 0}; // Seuls les deux premieres cases seront lues.
                                 // En avoir 4 permet seulement d'avoir moins
                                 // de tests
    int i;
    
    for (i=0; i<taillePlateau; i++)
        (pions[(int) adresseParXY(ligne, i, plateau)->couleur])++;

    if (pions[(int) couleur] + pions[JOKER] >= taillePlateau-1) // La ligne est au joueur: Il a toute la ligne (sauf éventuellement un point)
        return max(1, abs( (taillePlateau/2) - ligne)); // sans le max, renverrait 0 pour la ligne du milieu...

    // On renvoie un négatif si c'est l'ennemi
    if (pions[!couleur] + pions[JOKER] >= taillePlateau-1)
        return - max(1, abs( (taillePlateau/2) - ligne));

    return 0;
}

int valeurCase(int x, int y)
{
    x = min(x, taillePlateau-x-1);
    y = min(y, taillePlateau-y-1);
    if (x > 1 && y > 1) // On est au milieu du plateau
        return 40*(x+y)*taillePlateau;

    if (x == 1 || y == 1) // On est sur l'avant dernière ligne
    {
        if (x==y) // On est en diagonale du coin
            return 0; // La plus basse valeur
        if (x==0 || y==0) // On est sur un bord, à coté d'un coin
            return 50*taillePlateau;
        // Sinon:
        return 150*taillePlateau;
    }

    if (x == 0 || y == 0) // On est sur un bord, mais pas a coté du coin
    {
        if (x==y) // On est sur le coin !!!
            return 3500*taillePlateau;
        // Sinon, on est juste sur un bord:
        return 600*taillePlateau;
    }

    if (args.verbose)
        erreur("PAS GLOP!!!!\n");
    return 0;

}

