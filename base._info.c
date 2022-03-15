
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <stdlib.h>
#include <string.h>
/**
 * @brief Afficher les informations généraux sur le fichier dont le
 * pid est fourni en parametère
 * 
 * @param pid PID du processus
 */
void debuggee_status(pid_t pid)
{
    char string1[20];
    const char *string2 = string1;
    sprintf(string1,"%d",(int)pid);
    char buf[100];
    strcat(strcpy(buf, "/proc/"), string2);
    strcat(buf,"/status");
    char *args[] = {"cat", buf, NULL};
    execvp(args[0], args);
}


void debuggee_localisation(pid_t pid)
{
    char string1[20];
    const char *string2 = string1;
    sprintf(string1,"%d", (int)pid);
    char buf[100];
    strcat(strcpy(buf, "/proc/"),string2);
    strcat(buf,"/exe");
    char *args[] = {"ls", "-l", buf, NULL};
    execvp(args[0],args);
}

int main()
{
    pid_t PID = getpid();
    debuggee_status(PID);
   
    debuggee_localisation(PID);
    return 0;
}