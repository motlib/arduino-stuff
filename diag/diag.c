#include "diag.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DIAG_IN_BUF_LEN 32

#define DIAG_MAX_CMD_LEN 8

typedef enum
{
    DiagErr_Ok,
    DiagErr_Ovflw,
    DiagErr_InvCmd,
} DiagErr_T;

typedef struct {
    char in[DIAG_IN_BUF_LEN];
    uint8_t in_pos;
    DiagErr_T err;
} DiagData_T;

typedef enum {
    DiagCmd_Read,
    DiagCmd_Write,
    DiagCmd_Describe,
    DiagCmd_Invalid,
} DiagCmd_T;

typedef struct {
    DiagCmd_T cmd;
    uint8_t sec;
    uint8_t id;
    // no support for writing yet
} DiagReq_T;

static DiagData_T ddata = { {0}, 0, DiagErr_Ok};
    

static DiagItem_T d_items[] = {
    { &ddata.err, 1, DIAG_TYPE_INT, "Last Err" },
};


static DiagSect_T d_sect = {
    d_items,
    1,
    "Int. diag data"
};


/* Table of all diag sections. */
static DiagSect_T * dtbls[1] = {
    &d_sect,
};


static void gettok(char *buf, uint8_t* pos)
{
    uint8_t i = 0;

    while((ddata.in[*pos + i] != ' ') && (ddata.in[*pos+i] != '\0'))
    {
        buf[i] = ddata.in[*pos + i];
        i += 1;
    }
    buf[i] = '\0';

    printf("token %s\n", buf);
    
    *pos += i + 1;
}

static DiagItem_T* diag_get_item(DiagReq_T *dreq)
{
    uint8_t num_tbls = sizeof(dtbls) / sizeof(dtbls[0]);
    DiagSect_T *tbl;
    DiagItem_T *item;
    
    if(dreq->sec < num_tbls)
    {
        tbl = dtbls[dreq->sec];
    }
    else
    {
        return NULL;
    }

    if(dreq->id < tbl->size)
    {
        item = &(tbl->items[dreq->id]);
    }
    else
    {
        return NULL;
    }

    return item;
}


static void diag_parse_request(DiagReq_T *dreq)
{
    uint8_t pos = 0;
    char buf[8];

    ddata.in[ddata.in_pos] = 0;
    
    gettok(buf, &pos);
    
    if(buf[0] == 'r')
    {
        dreq->cmd = DiagCmd_Read;
    }
    else if(buf[0] == 'w')
    {
        dreq->cmd = DiagCmd_Write;
    }
    else if(buf[0] =='d')
    {
        dreq->cmd = DiagCmd_Describe;
    }
    else
    {
        dreq->cmd = DiagCmd_Invalid;
        ddata.err = DiagErr_InvCmd;
        return;
    }

    gettok(buf, &pos);
    dreq->sec = (uint8_t)atoi(buf);

    gettok(buf, &pos);
    dreq->id = (uint8_t)atoi(buf);

    //printf("Command %i, sec %i, id %i\n", dreq->cmd, dreq->sec, dreq->id);
}

static void diag_handle_read(DiagReq_T *dreq)
{
    DiagItem_T *item = diag_get_item(dreq);
    if(item == NULL)
    {
        printf("Could not get diag item.\n");
        return;
    }

    if(item->flags & DIAG_TYPE_INT) {
        if(item->size == 1) {
            uint8_t val = *((uint8_t*)item->data);
            printf("Read value: %i\n", val);
        }
    }
}

static void diag_handle_describe(DiagReq_T *dreq)
{
    DiagItem_T *item = diag_get_item(dreq);
    DiagSect_T *sect;
    
    if(item == NULL)
    {
        printf("Could not get diag item.\n");
        return;
    }

    sect = dtbls[dreq->sec];

    printf("Section %i (%i items): %s\n", dreq->sec, sect->size, sect->name);
    printf("Item %i: %s\n", dreq->id, item->name);
}

void diag_handle(void)
{
    DiagReq_T dreq = {DiagCmd_Invalid, 0, 0};

    diag_parse_request(&dreq);

    switch(dreq.cmd)
    {
    case DiagCmd_Read:
        diag_handle_read(&dreq);
        break;
    case DiagCmd_Describe:
        diag_handle_describe(&dreq);
        break;
    default:
        break;
    }

    /* After handling input, reset pos to 0. */
    ddata.in_pos = 0;
}


void diag_add_input_char(char c)
{
    if(c == '\n') {
        // handle
    }
    else
    {
        if(ddata.in_pos >= DIAG_IN_BUF_LEN)
        {
            /* Input too long. Set buffer overflow flag. */
            ddata.err = DiagErr_Ovflw;
        }
        else
        {
            /* Add char to input buffer */
            ddata.in[ddata.in_pos] = c;
            ++ddata.in_pos;
        }
    }
}

void diag_add_input(char *input)
{
    uint8_t i = 0;

    while(input[i] != '\0')
    {
        diag_add_input_char(input[i]);
        ++i;
    }
}
