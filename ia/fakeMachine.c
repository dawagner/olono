#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>       
#include <sys/time.h>
#include <sys/resource.h>


long ack(long n, long m);

int main(int argc, char *argv[])
{

    /* des messages d'erreur si on a mal lance le programme */

    char *errmsg = malloc(257 * sizeof(char));
    if (argc < 5)
    {
        perror("Il faut 4 arguments:\n 1: taille de la grille\n2: affichage de la grille\n3: humain/ia\n4: B/N\n");
        free(errmsg);
        return -1;
    }

    char *hum = argv[3]; /* 3e argument: humain/ia */
    char *bw = argv[4]; /* 4eme argument: Blancs/Noirs */
    if ((strncmp(hum,"ia",2) != 0) || (bw[0] != 'N' && bw[0] != 'B'))
    {
        snprintf(errmsg,256,"Mauvais format de l'argument \"%s\" ou \"%s\"\n",hum,bw);
        perror(errmsg);
        return -2;
    }

    /* fin des messages d'erreurs */

    /**********************/
    /* CORPS DU PROGRAMME */
    /**********************/

    int i;
    int nbOctetsLus;
    char * input = malloc(256 * sizeof(char)); /* buffer d'entree */
    char * output = malloc(256 * sizeof(char)); /* coup joue: maximum 4 caracteres */

    if (bw[0] == 'B') {

        /* joue les blancs: 10 fois le meme coup */
        for (i=0;i<10;i++){
            /* joue un coup: ecrit son coup, identique a printf("A1\n"); */
            strcpy(output,"A1\n");
            write(STDOUT_FILENO,output,strlen(output));
            /* lit le coup joue, identique a scanf("%s",input) */
            nbOctetsLus = read(STDIN_FILENO,input,255);
            input[nbOctetsLus] = '\0';  // tableau de caracteres => chaine avec \0 a la fin
            /* on recommence */
        }
        snprintf(output,255,"Les blancs (moi) ont gagne ! Score: 30(B) - 15(N)\n");
        write(STDOUT_FILENO,output,strlen(output));

        /***************************************************************************
         * remarques sur l'utilisation de write: le 2nd argument est ce qu'on veut *
         * ecrire et le 3eme est le nb d'octets ecrire. Pour plus d'informations   *
         * man 2 write.Pour read: le 2d argument est l'adresse du buffer (alloue !)*
         * et le 3eme est le bf d'octets maximum que l'on veut lire (en general:   *
         * la taille en octets du buffer. Pour plus d'information, voir man read   *
         * enfin, dans les deux cas, le premier argument est la source (ou la      *
         * destination), en l'occurrence, l'entree et la sortie standard.          *
        /***************************************************************************/

    }
    
    else if (bw[0] == 'N')
    {


        /* joue les noirs: 10 fois le meme coup */
        for (i=0;i<10;i++){
            nbOctetsLus = read(STDIN_FILENO,input,255);
            input[nbOctetsLus] = '\0';
            strcpy(output,"B1\n");
            write(STDOUT_FILENO,output,strlen(output));
        }
        snprintf(output,255,"Les noirs (moi) ont gagne ! Score: 15(B) - 35(N)\n");
        write(STDOUT_FILENO,output,strlen(output));
    }

    /*****************************/
    /* FIN DU CORPS DU PROGRAMME */
    /*****************************/

    free(errmsg);
    free(input);
    free(output);
    return 0;
}



/* la celebre fonction d'Ackermann: impossible a calculer ne serait-ce que
 * ack(5,5). Prend un temps hyperexponentiel */
    long ack(long n, long m){
        if (n == 0)
            return m + 1;
        if (m == 0)
            return ack(n-1,1);
        else {
            return ack(n-1,ack(n,m-1));
        }

    }
