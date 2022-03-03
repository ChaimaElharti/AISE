/* ============== CODE PERMETTANT D'ANALYSER LE PROGRAMME ============== */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>


int main(int argc, char ** argv )
{

    /* ============== INFOS DE BASE DU PROCESSUS ============== */

    //On implémente comment retrouver la localisation du programme 

        char path[400];
	    getcwd(path, 400);
	    printf("%s\n", path);

    /*On retourne également l'identifiant du processus, celui de son parent, 
    et celui de son groupe. */
    
	fprintf(stderr, "PID %d PPID %d GID %d \n",
			getpid(),
			getppid(),
            getgid());



	return 0;
}