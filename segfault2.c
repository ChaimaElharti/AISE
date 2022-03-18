/*Programme générant un SegFault avec un malloc et 2 free. */ 

#include <stdio.h>
#include <stdlib.h>


int main(){

    int N = 10;
    int *r;
    r = malloc(N * sizeof(int));

    free(r);
    free(r);
    
    return 0;
}