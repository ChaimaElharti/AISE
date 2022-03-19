/* Programme SegFault avec une fonction pointeur. */

#include <stdio.h>
#include <stdlib.h>


int main(){
    
    int (*func) = NULL;
    (*func) = 1;
    
    return 0;
}