/*Code permettant de déterminer l'adresse du crash*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>


void crit_err_hdlr(int sig_num, siginfo_t * info, void * ucontext)
{
 void *             array[50];
 void *             caller_address;
 char **            messages;
 int                size, i;
 ucontext_t *   uc;
 size_t rip;

 uc = (ucontext_t *)ucontext;

 /* Get the address at the time the signal was raised */
#if defined(__i386__) // gcc specific
 caller_address = (void *) uc->uc_mcontext.eip; // EIP: x86 specific
 #elif defined(__x86_64__) // gcc specific
 caller_address = (void *) rip; // RIP: x86_64 specific
#else
#error Unsupported architecture. 
#endif

 size = backtrace(array, 50);

 /* overwrite sigaction with caller's address */
 array[1] = caller_address;

 messages = backtrace_symbols(array, size);

 /* skip first stack frame (points here) */
 for (i = 1; i < size && messages != NULL; ++i)
 {
  fprintf(stderr, "[bt]: (%d) %s\n", i, messages[i]);
 }

 free(messages);

 exit(EXIT_FAILURE);
}

int crash()
{
 char * p = NULL;
 *p = 0;
 return 0;
}

int foo4()
{
 crash();
 return 0;
}

int foo3()
{
 foo4();
 return 0;
}

int foo2()
{
 foo3();
 return 0;
}

int foo1()
{
 foo2();
 return 0;
}


int main(int argc, char ** argv)
{
 struct sigaction sigact;

 sigact.sa_sigaction = crit_err_hdlr;
 sigact.sa_flags = SA_RESTART | SA_SIGINFO;

 if (sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL) != 0)
 {
  fprintf(stderr, "Error setting signal handler for %d (%s)\n",
    SIGSEGV, strsignal(SIGSEGV));

  exit(EXIT_FAILURE);
 }

 foo1();
 exit(EXIT_SUCCESS);
}

