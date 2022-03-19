/*Programme de boucle infinie*/

#include <stdio.h>
#include <unistd.h>

void b(int *var)
{
    printf("%d\n", *var);
    sleep(1);
}

void a(int *var)
{
    b(var);
}

int main(void)
{
    printf("pid %d\n", getpid());
    int var = 100;
    while (1)
    {
        a(&var);
    }

    return 0;
}