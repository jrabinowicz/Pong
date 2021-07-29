/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#ifndef __SCHED_H__
#define __SCHED_H__

#include "stdint.h"
#include "screen.h"
#include "tss.h"
#include "game.h"

void sched_init();

int16_t sched_nextTask();

extern uint32_t pos_x_tasks[6];
extern uint32_t pos_y_tasks[6];
extern uint32_t scheduler[6]; 
extern uint32_t actual;
extern uint32_t tss_selectores[6];

#endif	/* !__SCHED_H__ */
