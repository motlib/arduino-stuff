#include <stdio.h>

#include "diag.h"

int main(void)
{
    char buf[255];

    while(1) {
        printf("Cmd: ");
        fgets(buf, 255, stdin);
        printf("Entered cmd: %s\n", buf);
        
        diag_add_input(buf);
        diag_handle();
    }

    return 0;
}
