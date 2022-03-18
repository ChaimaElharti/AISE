/*Programme générant un SegFault avec des pointeurs*/

#include <stdio.h>
#include <stdlib.h>

int main(){

    int *foo = NULL;
    *foo = 1;
    return 0;
}