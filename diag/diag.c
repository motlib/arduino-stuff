#include "diag.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "json_helper.h"
#include "diag_cfg.h"


#define DIAG_IN_BUF_LEN 32

#define DIAG_MAX_CMD_LEN 8


/**
 * Diagnostic commands
 */
#define DIAG_CMD_READ 'r'
#define DIAG_CMD_DESCRIBE 'd'
#define DIAG_CMD_INVALID '-'

/**
 * Enumeration of diag handling errors.
 */
typedef enum
{
    /** Everything is ok. */
    DiagErr_Ok,
    /** Input buffer overflow (input too long). */
    DiagErr_Ovflw,
    /** Invalid command received. */
    DiagErr_InvCmd,
    /** Invalid section or item accessed. */
    DiagErr_NoItem,
    /** Invalid data type. */
    DiagErr_InvalidType,
    /** Diag config error. */
    DiagErr_Cfg,
} DiagErr_T;


/**
 * Structure of internal diagnostic data and state.
 */
typedef struct
{
    /** Input buffer */
    char in[DIAG_IN_BUF_LEN];
    /** Next storage pos in input buffer. */
    uint8_t in_pos;
    /** Global error status. */
    DiagErr_T err;
} DiagData_T;




/**
 * Diagnostic request data.
 */
typedef struct {
    /** Received command */
    char cmd;
    /** Section to acces */
    uint8_t sec;
    /** Id to access */
    uint8_t id;
} DiagReq_T;


static DiagData_T ddata =
{
    /* Input buffer */
    {0},
    /* Input position */
    0,
    /* State of diagnostic module. */
    DiagErr_Ok
};
    

/*
 * Item table for diagnostic items in diag module (self-diagnosis :-) ).
 */
static DiagItem_T d_items[] =
{
    { &ddata.err, 1, DIAG_TYPE_INT, "Last Err" },
};


/**
 * Section info for diagnostics in diag module.
 */
DiagSect_T diag_sect =
{
    d_items,
    1,
    "Int. diag data"
};


/* Table of all diagnostic sections. */
extern DiagSect_T * dtbls[DIAG_SECT_COUNT];


static void gettok(char *buf, uint8_t* pos)
{
    uint8_t i = 0;

    while(((*pos + i) < ddata.in_pos) && (ddata.in[*pos + i] != ' ') && (ddata.in[*pos+i] != '\0'))
    {
        buf[i] = ddata.in[*pos + i];
        i += 1;
    }
    //FIXME: Pure hope that we do not overflow the buffer.
    buf[i] = '\0';
    
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
    dreq->cmd = buf[0];

    gettok(buf, &pos);
    dreq->sec = (uint8_t)atoi(buf);

    gettok(buf, &pos);
    dreq->id = (uint8_t)atoi(buf);
}


/**
 * Handle the diagnostic read command.
 */
static void diag_handle_read(DiagReq_T *dreq)
{
    DiagItem_T *item = diag_get_item(dreq);
    
    if(item == NULL)
    {
        ddata.err = DiagErr_NoItem;
        return;
    }

    if(item->flags & DIAG_TYPE_INT)
    {
        uint32_t val;
        
        switch(item->size)
        {
        case 1:
            val = *((uint8_t*)item->data);
            break;
        case 2:
            val = *((uint16_t*)item->data);
            break;
        case 4:
            val = *((uint32_t*)item->data);
            break;
        default:
            ddata.err = DiagErr_Cfg;
            return;
        }
        
        JSON_DICT_INT("v", val);
        JSON_SEP;
    }
    else if(item->flags & DIAG_TYPE_FLOAT)
    {
        float val = *((float*)item->data);
        JSON_DICT_FLOAT("v", val);
        JSON_SEP;
    }
    else
    {
        ddata.err = DiagErr_InvalidType;
    }
}


/**
 * Handle diagnostic describe command.
 */
static void diag_handle_describe(DiagReq_T *dreq)
{
    DiagItem_T *item = diag_get_item(dreq);
    DiagSect_T *sect;
    
    if(item == NULL)
    {
        ddata.err = DiagErr_NoItem;
        return;
    }

    sect = dtbls[dreq->sec];

    JSON_DICT_INT("sect", dreq->sec);
    JSON_SEP;
    JSON_DICT_INT("s_size", sect->size);
    JSON_SEP;
    JSON_DICT_STR("s_name", sect->name);
    JSON_SEP;
    JSON_DICT_INT("i_id", dreq->id);
    JSON_SEP;
    JSON_DICT_STR("i_name", item->name);
}


/**
 * Handle a diagnostic request stored in the internal input buffer.
 */
void diag_handle(void)
{
    DiagReq_T dreq = {DIAG_CMD_INVALID, 0, 0};

    /* We assume everything is ok. Later this will be written with the correct
     * error code if necessary. */
    ddata.err = DiagErr_Ok;

    diag_parse_request(&dreq);

    JSON_DICT_START;
    
    switch(dreq.cmd)
    {
    case DIAG_CMD_READ:
        diag_handle_read(&dreq);
        break;
    case DIAG_CMD_DESCRIBE:
        diag_handle_describe(&dreq);
        break;
    default:
        ddata.err = DiagErr_InvCmd;
        break;
    }

    JSON_DICT_INT("s", ddata.err);
    JSON_DICT_END;

    /* After handling input, reset pos to 0. */
    ddata.in_pos = 0;
}


/**
 * Add a character to the input buffer.
 */
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
