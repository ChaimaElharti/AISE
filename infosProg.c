/* ============== CODE SUR LES INFOS DE BASE DU PROGRAMME ============== */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <elf.h>


    /*Structure permettant de déterminant les fonctions 
        du programme et de les retourner. */


#define FNUM 3

struct fnc {
    void *addr;
    char name[32];
};

void (*f[FNUM])();
struct fnc fnames[FNUM];

char *getfname(void *p)
        {
            for (int i = 0; i < FNUM; i++) {
                if (fnames[i].addr == p)
                        return fnames[i].name;}
        return NULL;
        }

void functions(void)
{
     for (int i = 0; i < FNUM; i++) {
        printf("Function f[%d] is called '%s'\n", i, getfname(f[i]));
        printf("The pointer of the current function is %p\n", getfnp(__func__));
        printf("The name of this function is %s\n", getfname(getfnp(__func__)));
     }  
}


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


    /* ============== ÉTAT DE SA MÉMOIRE ============== */


    /*NOMS DES FONCTIONS*/
    functions();
        



	return 0;
}