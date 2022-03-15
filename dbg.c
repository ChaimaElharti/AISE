/**=======================================================================
 *                          Programme de debuggage en c                  *
 *
 ====================================================================== */
#include <string.h>
#include "../include/dbg.h"

/** Utilisés pour ignorer les autres arguments passé à
 *  l'appel système ptrace hormis : la requête et le pid
 *
 * long ptrace(enum __ptrace_request request, pid_t pid,
            void *addr, void *data);
 *  */

static const pid_t ignored_pid;
static const void *ignored_ptr;

static const void *no_continue_signal = 0;

/**
 * @brief Cette fonction permettra d'afficher les informations généraux sur le processus
 * dont le pid est fourni
 *
 * @param pid
 */
void debuggee_status(pid_t pid)
{
    char string1[20];
    const char *string2 = string1;
    sprintf(string1, "%d", (int)pid);
    char buffer[100];
    strcat(strcpy(buffer, "/proc/"), string2);
    strcat(buffer, "/status");
    char *args[] = {"cat", buffer, NULL};
    execvp(args[0], args);
}

void debugge_localisation(pid_t pid)
{
    char string1[20];
    const char *string2 = string1;
    sprintf(string1, "%d", (int)pid);
    char buffer[100];
    strcat(strcpy(buffer, "/proc/"), string2);
    strcat(buffer, "/exe");
    char *args[] = {" sudo ls", "-l", buffer, NULL};
    execvp(args[0], args);
}

void breakpoint_callback(dbg_debuggee_t debuggee, dbg_breakpoint_t bp)
{

}

/**
 * @brief
 * Cette fonction a pour but de configurer le processus cible à
 * débugger, permettant ainsi, qu'un processus le suivent (ici le processus parent)
 */
static void setup_debuggee(const char *path, char *const argv[])
{
    printf("Début debugge. Lancement de '%s'\n", path);
    // printf("PID : %d\nPPID : %d \nGID : %d \n", (int)getpid(), (int)getppid(), (int)getgid());
    if (ptrace(PTRACE_TRACEME, ignored_pid, ignored_ptr, ignored_ptr) < 0)
    {
        perror("ptrace");
        return;
    }
    // execv(path, argv);
    pid_t pid = getpid();
}

/**
 * @brief : cette fonction permettra de suivre de près le processus cible
 *
 * @param pid : cest le pid du processus à suivre
 */
static void attach_to_debuggee(pid_t pid)
{

    /**
     * @brief : Une fois la fonction setup_debugge lancer (et donc execv)
     * le processus parent enverra un signal : "SIGTRAP" au processus afin de
     * pourvoir expérer prendre le contrôle avant l'exécution du binaire dont
     * l'image vient d'être charger en mémoire.
     *
     * Après cela le parent recevra alors une valeur notifiant la cause de l'arret du processus fils.
     * Et ceci par le biais de l'appel system "waitpid"
     *
     */
    while (1)
    {
        int status;
        int options = 0;
        waitpid(pid, &status, options);

        /**
         * @brief :
         * WIFSTOPPED(status) --> Non null si le fils s'est arreter suite à un signal
         * WSTOPSIG (status)  --> Le numéro du signa ayant causé l'arrêt du processus fils
         * WIFEXITED (status) --> Non null si le fils se termine normalement .
         *
         */
        if (WIFEXITED(status) && WSTOPSIG(status) == SIGTRAP)
        {
            printf("Debuggee à l'arrêt sur réception de SIGTRAP - continué...\n");
            ptrace(PTRACE_CONT, pid, ignored_ptr, no_continue_signal); // Redémarrer le fils.
        }
        else if (WIFEXITED(status))
        {
            printf("Debugger termine...\n");
            exit(0);
        }
    }
}

/**
 * @brief
 *
 * @param path : chemin d'accès au binaire cible
 * @param argv : liste des arguments de débuggage
 */
void dbg_debuggee_exec(const char *path, char *const argv[])
{
    pid_t child;

    do
    {
        child = fork(); // création du processus fils
        switch (child)
        {
        case 0:                         // Nous somme dans le processus fils
            setup_debuggee(path, argv); // Après appel à la fonction setup_debuggee
                                        // qui lui même appel execv, l'image du binaire à débugger sera charger en mémoire

            // Et écrasera celui du processus fils.
            break;
        case -1: // error
            perror("fork fail. \n");
            break;

        default:                       // Nous somme dans le processus debugger
            attach_to_debuggee(child); // commencer par suivre le processus cible
            break;
        }
    } while (child == -1 && errno == EAGAIN);
}