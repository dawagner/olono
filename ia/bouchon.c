#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/select.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<signal.h>
#include<errno.h>

int joueHM(int taille, char * p2, char *log);
int joueMH(int taille, char * p1, char *log);
int joueMM(int taille, char * p1, char * p2, char *log);
int dansGrille(char col ,int lig, int taille);
void finMachineSimple(int from1, char *nom1, int status, int from2, int to2, int pid2, char *nom2, FILE *logFile);
int safeWrite(int fd, char *s);

int main(int argc, char *argv[]){

  int taille;
  char joueur1, joueur2, trace, rep;
  char *path1 = NULL, *path2 = NULL;
  char *logFile = NULL;
  signal(SIGPIPE,SIG_IGN); /* on ignore les signaux de tube ferme */
  printf("Bonjour\n");
  printf("procedure simplifiee ? (O/N): ");
  scanf(" %c",&rep);
  if (rep == 'N') {
    /* filtre de saisie: taille impaire, entre 5 et 25 */
    do {
      printf("Quelle est la taille (impaire) pour la grille ? ");
      scanf("%d",&taille);
    } while ((taille%2 ==0)||(taille>26)||(taille<4));
    /* filtre de saisie: premier joueur humain ou programme */
    do {
      printf("Qui joue les blancs ? Humain ? Machine ? (H/M): ");
      scanf(" %c",&joueur1);
    } while ((joueur1 != 'H') && (joueur1 != 'M'));
    /* filtre de saisie: second joueur humain ou programme */
    do {
      printf("Qui joue les noirs ? Humain ? Machine ? (H/M): ");
      scanf(" %c",&joueur2);
    } while ((joueur2 != 'H') && (joueur2 != 'M'));
    
    if (joueur1 == 'M'){
      /* le chemin d'acces au programme joueur 1 */
      path1 = malloc(513 * sizeof(char));
      printf("Veuillez entrer le chemin du programme joueur 1: ");
      scanf("%s",path1); 
      while(access(path1,R_OK|X_OK|F_OK) != 0) {
	printf("Ce fichier n'existe pas ou bien n'est pas executable\n");
	printf("Veuillez entrer le chemin du programme joueur 1: ");
	scanf("%s",path1);  
      }
    }
    if (joueur2 == 'M'){
      /* le chemin d'acces au programme joueur 2 */

      path2 = malloc(513 * sizeof(char));
      printf("Veuillez entrer le chemin du programme joueur 2: ");
      scanf("%s",path2); 
      while(access(path2,R_OK|X_OK|F_OK) != 0) {
	printf("Ce fichier n'existe pas ou bien n'est pas executable\n");
	printf("Veuillez entrer le chemin du programme joueur 2: ");
	scanf("%s",path2);  
      }
    }
    
    /* si jamais on veut une trace: non implementee pour l'instant */
    do {
      printf("voulez-vous enregistrer la trace d'execution ? (O/N): ");
      scanf(" %c",&trace);
    } while (trace != 'O' && trace != 'N');
    
    if (trace == 'O'){
      logFile = malloc(513 * sizeof(char));
      printf("Veuillez entrer le chemin du fichier de log: ");
      scanf("%s",logFile);  
    }
  } else { /* remplissage par defaut */
    joueur1 = 'M';
    joueur2 = 'M';
    taille = 25;
    path1 = malloc(30*sizeof(char));
    strcpy(path1,"./olono");
    path2 = malloc(30*sizeof(char));
    strcpy(path2,"./olono-rand");
    logFile = malloc(30*sizeof(char));
    strcpy(logFile,"test");
  }
  /* maintenant on a toutes les informations qu'il faut */
  /* pour l'instant, on n'implemente pas le fichier de log */
  int retval;
  if ((joueur1 == 'H') && (joueur2 == 'H')){
    printf("Deux joueurs humains, ce n'est pas mon boulot\n");
  } else if((joueur1 == 'H') && (joueur2 == 'M')) {
    printf("Humain contre machine (%s), taille %d\n",path2,taille);
    retval = joueHM(taille,path2,logFile);
  } else if((joueur1 == 'M') && (joueur2 == 'H')) {
    printf("Machine (%s) contre humain, taille %d\n",path1,taille);
    retval = joueMH(taille,path1,logFile);
  } else {
    printf("Machine (%s) contre machine (%s)\n",path1,path2);
    retval = joueMM(taille,path1,path2,logFile);
  }


  printf("\n\nLe jeu est termine ! \n\n");
  free(path1);
  free(path2);
  free(logFile);
  return retval;
}

int dansGrille(char col, int lig, int taille){
return (((col -'A') >= 0) && ((col - 'A') < taille) && 
	  (lig >= 0) && (lig < taille));
}


/* jeu d'un homme contre une machine */
int joueHM(int taille, char *p2, char *log){
  int versProgramme[2]; /* tube pour envoyer des infos VERS le programme */
  int deProgramme[2]; /* tube pour recuperer des infos DEPUIS le programme */
  FILE *logFile = NULL;
  if (log != NULL){
    printf("fichier d'enregistrement: %s\n",log);
    logFile = fopen(log,"w");
  }
  pipe(versProgramme); 
  pipe(deProgramme);
  int pid = fork();
  if (pid == -1){
    perror("Fork n'a pas reussi\n");
    return(-1);
  } else if (pid == 0){
    /* on est l'enfant: lancer le programme apres avoir
     * redirige entree et sortie standard vers les tubes */
    dup2(versProgramme[0],STDIN_FILENO);
    dup2(deProgramme[1],STDOUT_FILENO);
    close(versProgramme[1]); // pas besoin de lire ce qu'on ecrit
    close(deProgramme[0]); // et inversement
    char t[40];
    sprintf(t,"%d",taille);
    execlp(p2,p2,t,"grilleOn","ia","N",(char *) NULL);
    perror("Pas reussi a executer le programme. Desole");
    free(p2);
    exit(-4);
  } else {
    /* on est le pere et pid est le pid du fils */
    /* on recupere les infos du tube. On veut bien sur aussi 
     * garder entree et sortie standard */
    close(versProgramme[0]);
    close(deProgramme[1]);
    printf("Vous etes les blancs, vous commencez\n");
    char *coupBlanc = malloc(513 * sizeof(char));
    char *coupNoir = malloc(513 * sizeof(char));
    coupNoir[512] = '\0';
    char colonneB, colonneN;
    int ligneB, ligneN;
    int nbRead;
    int status;
    int changed;
    do {
      changed = waitpid(pid,&status,WNOHANG); // on regarde si le fils n'a pas termine
      if (changed == 0) {
	do {
	  printf("Votre coup (blancs) : ");
	  scanf("%s",coupBlanc);
	  colonneB = coupBlanc[0];
	  sscanf(coupBlanc+1,"%d",&ligneB);
	} while (dansGrille(colonneB,ligneB,taille) == 0);
	printf("Vous jouez le coup: %s\n",coupBlanc);
	write(versProgramme[1],coupBlanc,strlen(coupBlanc));
	if (logFile != NULL){
	  fprintf(logFile,coupBlanc);
	  fflush(logFile);
	}
	/* attente bloquante */
	nbRead = read(deProgramme[0],coupNoir,512);
	printf("Lu %d caracteres\n",nbRead);
	if (nbRead >= 5){
	  printf("Les noirs declarent %s\n",coupNoir);
	} else { /* un coup legal */
	  printf("Les noirs jouent %s\n",coupNoir);
	}	  
	if (logFile != NULL){
	    fprintf(logFile,coupNoir);
	    fflush(logFile);
	  }
      } else {
	printf("Les noirs ont termine de jouer: %d\n",WEXITSTATUS(status));
      }
    } while (changed == 0); 
    /* on continue tant que le fils n'a pas termine */
    /* puis on libere tout */
    free(coupNoir);
    free(coupBlanc);
    close(versProgramme[1]);
    close(deProgramme[0]);
    return 0;
  }
}


/* jeu d'une machine contre un homme */
int joueMH(int taille, char *p1, char *log){
  int versProgramme[2]; /* tube pour envoyer des infos VERS le programme */
  int deProgramme[2]; /* tube pour recuperer des infos DEPUIS le programme */
  FILE *logFile = NULL;
  if (log != NULL){
    printf("fichier d'enregistrement: %s\n",log);
    logFile = fopen(log,"w");
  }
  pipe(versProgramme); 
  pipe(deProgramme);
  int pid = fork();
  if (pid == -1){
    perror("Fork n'a pas reussi\n");
    return(-1);
  } else if (pid == 0){
    /* on est l'enfant: lancer le programme apres avoir
     * redirige entree et sortie standard vers les tubes */
    dup2(versProgramme[0],STDIN_FILENO);
    dup2(deProgramme[1],STDOUT_FILENO);
    close(versProgramme[1]); // pas besoin d ecrire ce qu'on lit
    close(deProgramme[0]); // et inversement
    char t[40];
    sprintf(t,"%d",taille);
    execlp(p1,p1,t,"grilleOn","ia","B",(char *) NULL);
    perror("Pas reussi a executer le programme. Desole");
    free(p1);
    exit(-4);
  } else {
    /* on est le pere et pid est le pid du fils */
    /* on recupere les infos du tube. On veut bien sur aussi 
     * garder entree et sortie standard */
    close(versProgramme[0]);
    close(deProgramme[1]);
    printf("Vous etes les noirs, vous jouez en second\n");
    char *coupBlanc = malloc(513 * sizeof(char));
    char *coupNoir = malloc(513 * sizeof(char));
    coupBlanc[512] = '\0';
    char colonneB, colonneN;
    int ligneB, ligneN;
    int nbRead;
    int status;
    int changed;
    do {
      changed = waitpid(pid,&status,WNOHANG); // on regarde si le fils n'a pas termine
      if (changed == 0) {
	/* attente bloquante ??? */
	nbRead = read(deProgramme[0],coupBlanc,512*sizeof(char));
	if (nbRead >= 5){
	  printf("Les blancs declarent: %s",coupBlanc);
	} else { /* un coup legal */
	  printf("Les blancs jouent: %s",coupBlanc);
	}
	if (logFile != NULL){
	  fprintf(logFile,coupBlanc);
	  fflush(logFile);
	}
	changed = waitpid(pid,&status,WNOHANG); // on teste encore une fois le fils
	if (changed == 0) {
	  do {
	    printf("Votre coup (noirs) : ");
	    scanf("%s",coupNoir);
	    colonneN = coupNoir[0];
	    sscanf(coupNoir+1,"%d",&ligneN);
	  } while (dansGrille(colonneN,ligneN,taille) == 0);
	  printf("Vous jouez le coup: %s\n",coupNoir);
	  write(versProgramme[1],coupNoir,strlen(coupNoir));	  
	  if (logFile != NULL){
	    fprintf(logFile,coupNoir);
	    fflush(logFile);
	  }
	}
      } else {
	printf("Les blancs ont termine de jouer: %d\n",WEXITSTATUS(status));
      }
    } while (changed == 0); 
    /* on continue tant que les blancs n'ont pas termine */
    /* puis on libere tout */
    free(coupNoir);
    free(coupBlanc);
    close(versProgramme[1]);
    close(deProgramme[0]);
    return 0;
  }
}

int joueMM(int taille, char *p1, char *p2, char *log){  
  FILE *logFile = NULL;
  if (log != NULL){
    printf("fichier d'enregistrement: %s\n",log);
    logFile = fopen(log,"w");
  }
  int versProgrammeA[2], versProgrammeB[2];
  int deProgrammeA[2], deProgrammeB[2];
  pipe(versProgrammeA); 
  pipe(deProgrammeA);
  int pidA = fork();
  if (pidA == -1){
    perror("Fork n'a pas reussi");
    return(-1);
  } else if (pidA == 0){
    /* fils A: les blancs */
    dup2(versProgrammeA[0],STDIN_FILENO);
    dup2(deProgrammeA[1],STDOUT_FILENO);
    close(versProgrammeA[1]);
    close(deProgrammeA[0]);
    fclose(logFile);
    char t[40];
    sprintf(t,"%d",taille);
    execlp(p1,p1,t,"grilleOn","ia","B",(char *) NULL);
    perror("Pas reussi a executer le programme A. Desole");
    return -4;
    
  } else {
    /* pere */
    pipe(versProgrammeB); 
    pipe(deProgrammeB);
    close(versProgrammeA[0]);
    close(deProgrammeA[1]);
    int pidB = fork();
    if (pidB == -1){
      perror("Fork 2 n'a pas reussi");
      return -1; /* le fils A est tue quand le pere meurt */
    } else if (pidB == 0) {
      
      /* fils B: les noirs */
      close(versProgrammeA[1]);
      close(deProgrammeA[0]);
      close(versProgrammeB[1]);
      close(deProgrammeB[0]);
      fclose(logFile);
      dup2(versProgrammeB[0],STDIN_FILENO);
      dup2(deProgrammeB[1],STDOUT_FILENO);
      char t[40];
      sprintf(t,"%d",taille);
      execlp(p2,p2,t,"grilleOff","ia","N",(char *) NULL);
      perror("Pas reussi a executer le programme B. Desole");
      return -5;
      
    } else {
      /* le pere */
      close(versProgrammeB[0]);
      close(deProgrammeB[1]);
      char *coupBlanc = malloc(513 * sizeof(char));
      char *coupNoir = malloc(513 * sizeof(char));
      int nbRead;
      int status;
      int changed;
      int i=0;
      do {
	changed = waitpid(pidA,&status,WNOHANG);
	if (changed == 0) {
	  nbRead = read(deProgrammeA[0],coupBlanc,512);
	  coupBlanc[nbRead] = '\0';
	  if (nbRead >= 5){
	    printf("Les blancs declarent: %s",coupBlanc);
	  } else { /* un coup legal: colonne, ligne, \n et \0 */
	    printf("Les blancs jouent: %s",coupBlanc);
	  }
	  safeWrite(versProgrammeB[1],coupBlanc);
	  if (logFile != NULL){
	    fprintf(logFile,coupBlanc);
	    fflush(logFile);
	  }
	  changed = waitpid(pidB,&status,WNOHANG);
	  if (changed == 0){
	    nbRead = read(deProgrammeB[0],coupNoir,512);
	    coupNoir[nbRead] = '\0';
	    if (nbRead >= 5){
	      printf("Les noirs declarent: %s",coupNoir);
	    } else { /* un coup legal: colonne, ligne, \n et \0 */
	      printf("Les noirs jouent: %s",coupNoir);
	    }
	  /* tester si on peut ecrire avant de le faire ! Sinon ca plante */
	    safeWrite(versProgrammeA[1],coupNoir);
	    if (logFile != NULL){
	    fprintf(logFile,coupNoir);
	    fflush(logFile);
	    }
	  } else {
	    finMachineSimple(deProgrammeB[0],"noirs",status,deProgrammeA[0],versProgrammeA[1],pidA,"blancs",logFile);
	  }
	} else {
	  finMachineSimple(deProgrammeA[0],"blancs",status,deProgrammeB[0],versProgrammeB[1],pidB,"noirs",logFile);
	}
      } while (changed == 0); 
      free(coupNoir);
      free(coupBlanc);
      if (logFile != NULL){
	fprintf(logFile,"\n---- partie terminee ----\n ");
	fflush(logFile);
	fclose(logFile);
      }
      close(versProgrammeA[1]);
      close(deProgrammeA[0]);
      close(versProgrammeB[1]);
      close(deProgrammeB[0]);
      return 0;
    } // fin second fork
  } // fin premier fork
  
}


void finMachineSimple(int from1, char *nom1, int status, int from2, int to2, int pid2, char *nom2, FILE *logFile){
  int nbRead;
  char *coup1 = malloc(513 * sizeof(char));
  char *coup2 = malloc(513 * sizeof(char));
  struct rusage infoFils;
  double uFils1, sFils1, uFils2, sFils2;
  printf("Les %s ont termine de jouer: %d!\n",nom1,WEXITSTATUS(status));
  getrusage(RUSAGE_CHILDREN,&infoFils);
  /* on recupere des secondes et des micro secondes pour le temps CPU passe
   * en calculs utilisateurs et en appel de fonctions systeme du fils 1 */
  sFils1 = ((double) infoFils.ru_stime.tv_sec) + ((double) infoFils.ru_stime.tv_usec)/1000000;
  uFils1 = ((double) infoFils.ru_utime.tv_sec) + ((double) infoFils.ru_utime.tv_usec)/1000000;
  printf("Les %s ont utilise %lf sec de systeme et %lf de temps utilisateur\n",
	 nom1,sFils1,uFils1);
  nbRead = read(from1,coup1,512);
  if (nbRead > 0){
    coup1[nbRead] = '\0';
    printf("Les %s ont un dernier message: %s",nom1,coup1);
    safeWrite(to2,coup1);
    if (logFile != NULL){
      fprintf(logFile,coup1);
      fflush(logFile);
    }
  }
  sleep(1); /* on attend une seconde que l'autre ait aussi termine ! */
  if (waitpid(pid2,&status,WNOHANG) == 0){
    printf("Cependant, les %s n'ont pas termine de jouer ... tant pis.\n",nom2);
    /* on les tue */
    kill(pid2,SIGKILL);
    waitpid(pid2,&status,WNOHANG);
  } else {
    printf("Les %s on aussi termine: %d!\n",nom2,WEXITSTATUS(status));
  }
  getrusage(RUSAGE_CHILDREN,&infoFils);
  /* idem, sauf qu'on recupere la somme du fils 1 et du fils 2 ... */
  sFils2 = ((double) infoFils.ru_stime.tv_sec) + ((double) infoFils.ru_stime.tv_usec)/1000000;
  uFils2 = ((double) infoFils.ru_utime.tv_sec) + ((double) infoFils.ru_utime.tv_usec)/1000000;
  /* idem, sauf qu'on recupere la somme du fils 1 et du fils 2  */
  sFils2 = sFils2 - sFils1;
  uFils2 = uFils2 - uFils1;
  printf("Les %s ont utilise %lf sec de systeme et %lf de temps utilisateur\n",
	 nom2,sFils2,uFils2);
  nbRead = read(from2,coup2,512);
  if(nbRead > 0){
    coup2[nbRead] = '\0';
    printf("Les %s ont un dernier message: %s",nom2,coup2);
    if (logFile != NULL){
      fprintf(logFile,coup2);
      fflush(logFile);
    }
  }
}

/* ecrit de facon "sure" dans un tube: si on essaie d'ecrire dans un tube * 
 * ferme, on recoit un signal qui fait terminer le programme */
/* retourne le nombre de caracteres ecrit et -1 si ca s'est mal passe */
int safeWrite(int fd, char *s){
  if (write(fd,s,strlen(s)) == -1)
    printf("Impossible d'ecrire dans le tube: il est ferme en lecture\n");
  return 0;
}





/* vieux machin complique et qui marche pas 

void finMachine(int from1, char *nom1, int status, int from2, int to2, int pid2, char *nom2, FILE *logFile){
  int nbRead;
  fd_set set;
  struct timeval timeout = {1,0};// timeout a 1 seconde
  char *coup1 = malloc(513 * sizeof(char));
  char *coup2 = malloc(513 * sizeof(char));
  printf("Les %s ont termine de jouer: %d!\n",nom1,WEXITSTATUS(status));
  FD_ZERO(&set);
  FD_SET(from1,&set);
  // s'il y a encore des choses a lire
  if (select(from1+1,&set,NULL,NULL,&timeout) == 0){
    printf("Les %s n'ont rien d'autre a ajouter\n",nom1);
  } else {
    nbRead = read(from1,coup1,512);
    coup1[nbRead] = '\0';
    printf("Les %s ont un dernier message: %s",nom1,coup1);
    safeWrite(to2,coup1);
    if (logFile != NULL){
      fprintf(logFile,coup1);
      fflush(logFile);
    }
  }
  if (waitpid(pid2,&status,WNOHANG) == 0){
    printf("Cependant, les %s n'ont pas termine de jouer ... tant pis.\n",nom2);
  } else {
    printf("Les %s on aussi termine: %d!\n",nom2,WEXITSTATUS(status));
  }
  FD_ZERO(&set);
  FD_SET(from2,&set);
  if (select(from2+1,&set,NULL,NULL,&timeout) == 0){
    printf("Les %s n'ont rien a dire\n",nom2);
  } else {
    nbRead = read(from2,coup2,512);
    coup2[nbRead] = '\0';
    printf("Les %s ont un dernier message: %s",nom2,coup2);
    if (logFile != NULL){
      fprintf(logFile,coup2);
      fflush(logFile);
    }
  }

}
*/

