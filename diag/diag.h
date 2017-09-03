#ifndef DIAG_H
#define DIAG_H

/*
 * Request:
 * <cmd> <section> <endpoint> <params>
 * cmd = r|w
 *
 * 
 */

void diag_add_input_char(char c);
void diag_add_input(char *input);
void diag_handle(void);



#endif /* DIAG_H */
