/* src/fetch.h
* Instruction fetch stage of the pipeline
*/


#ifndef _FETCH_H
#define _FETCH_H


#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "util.h"
#include "memory.h"



void fetch(control_t *, pc_t * pc);


#endif
