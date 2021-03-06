/* src/memory.h
 * Memory access stage of the pipeline
 */

#ifndef _MEMORY_H
#define _MEMORY_H

#include "types.h"
#include "util.h"
#include "main_memory.h"
#include "cache.h"

void memory(control_t *exmem, control_t *memwb, cache_config_t *cache_cfg);

#endif
