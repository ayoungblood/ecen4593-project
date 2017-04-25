/* src/decode.h
 * Instruction decode stage of the pipeline
 */

#ifndef _DECODE_H
#define _DECODE_H

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "types.h"
#include "util.h"
#include "registers.h"

int decode(control_t *ifid, control_t *idex);

// Helper functions
void setidexImmedArithmetic(control_t *idex);
void setidexLoad(control_t *idex);
void setidexStore(control_t *idex);

#endif /* _DECODE_H */
