/*Programme générant un Stack smashing*/

#include <stdio.h>
#include <stdlib.h>

int foo(int a, int b)
{
    return a + b;
}
int main()
{
    int a = 23;
    static char c;

    int x = foo(a, 23);
    int tab[10];
    tab[11] = 3;
    printf("resultat %d", x);
    return 0;
}