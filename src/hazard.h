/* src/hazard.h
* Hazard detection unit. Control and data hazard detection and forwarding
*/

#ifndef _HAZARD_H
#define _HAZARD_H

#include "util.h"
#include "types.h"
#include "registers.h"

/*To be called after the execution of a clock cycle. Unit will forward any data
that will prevent a data hazard, insert nops into the pipeline if forwarding
can't prevent the data hazard, and flush IFID if a branch is taken
*/
//HAZARD UPDATES THE PC, SO IT MUST BE CALLED
int hazard(control_t *ifid, control_t *idex, control_t *exmem, control_t *memwb, pc_t *pc, cache_config_t *cache_cfg);

void hazard_init(void);

void restore(control_t *ifid, control_t *idex, control_t *exmem, control_t *memwb, pc_t *pc);

void backup(control_t *ifid, control_t *idex, control_t *exmem, control_t *memwb, pc_t *pc);

#endif /* _HAZARD_H */
