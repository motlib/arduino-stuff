#ifndef DIAG_H
#define DIAG_H

#include <stdint.h>

/*
 * Request:
 * <cmd> <section> <endpoint> <params>
 * cmd = r|w
 *
 * 
 */


#define DIAG_TYPE_INT 1
#define DIAG_TYPE_FLOAT 2

/**
 * Descriptor for a single diag data item.
 */
typedef struct
{
    void* data;
    uint8_t size;
    uint8_t flags;
    char name[20];
} DiagItem_T;


/**
 * Description for a diag section (with probably multiple data items inside).
 */
typedef struct
{
    DiagItem_T *items;
    uint8_t size;
    char name[20];
} DiagSect_T;


extern DiagSect_T diag_sect;


void diag_add_input_char(char c);
void diag_add_input(char *input);
void diag_handle(void);



#endif /* DIAG_H */
