#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/user.h>
#include <sys/reg.h>


/* waitchild encapsule un appel à waitpid. Si le processus passe à l'état STOPPED, 
elle renvoie 0. Et si le debugger passe à l'état TERMINATED, elle renvoie 1.*/

int waitchild(pid_t pid) {
    int status;
    waitpid(pid, &status, 0);
    if(WIFSTOPPED(status)) /*returns  true if the child process 
                            was stopped by delivery of a signal*/
    { 
        return 0;
    }
    else if (WIFEXITED(status)) /*returns  true  if the child 
                                      terminated normally*/
    
    {
        return 1;
    }
    else {
        printf("%d raised an unexpected status %d", pid, status);
        return 1;
    }
}

// Conversion de caractère en entier long non signé
unsigned long to_ulong(char * s) {
  return strtol(s, NULL, 16);
}


unsigned long readRegister(pid_t tracee, int reg) {
  return ptrace(PTRACE_PEEKUSER, tracee, 8 * reg, NULL);
  //Read a word at offset addr in the tracee's USER area
  //The  word is  returned as the result of the ptrace() call.
}


void showregisters(pid_t tracee) {
  printf("RIP = %lx\nRAX = %lx\n",
        readRegister(tracee, RIP), readRegister(tracee, ORIG_RAX));
}

void presskey() {
  getchar();
}

int main(int argc, char ** argv) {
    unsigned long bpAddress = to_ulong(argv[1]);
    pid_t child = fork();
    unsigned long rip;
    if(child == 0) {
      //Indicate that this process is to be traced by its parent.
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execve(argv[2], argv + 2, NULL);
    }
    else {
        // wait for the child to stop
        waitchild(child);
        do {
          rip = readRegister(child, RIP);
          if(rip == bpAddress) {
            showregisters(child);
            presskey();
          }

          /*Restart the stopped tracee and arrange for the tracee 
          to be stopped after execution of a single instruction.*/
          
          ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
        } while(waitchild(child) < 1);
    }
    return 0;
}
