/* src/write.h
* Write back stage of the pipeline
*/

#ifndef _WRITE_H
#define _WRITE_H

#include "types.h"
#include "util.h"
#include "registers.h"

void writeback(control_t *memwb);

#endif
