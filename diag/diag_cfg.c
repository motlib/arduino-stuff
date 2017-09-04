#include "diag.h"
#include "diagtest.h"

#include "diag_cfg.h"

/**
 * Table of all diagnostic sections.
 */
DiagSect_T * dtbls[DIAG_SECT_COUNT] =
{
    &diag_sect,
    &diagtest_sect,
};
