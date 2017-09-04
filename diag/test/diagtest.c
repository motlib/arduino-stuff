#include <stdio.h>

#include "diag.h"

static float f = 7.3f;


/*
 * Item table for diagnostic items in diag module (self-diagnosis :-) ).
 */
static DiagItem_T d_items[] =
{
    { &f, 1, DIAG_TYPE_FLOAT, "Float Test" },
};


/**
 * Section info for diagnostics in diag module.
 */
DiagSect_T diagtest_sect =
{
    d_items,
    sizeof(d_items) / sizeof(d_items[0]),
    "Int. diag data"
};



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
