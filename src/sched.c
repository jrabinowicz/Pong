/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del scheduler
*/

#include "sched.h"

uint32_t scheduler[6]; //present
uint32_t actual; //id, 6 = idle
uint32_t tss_selectores[6];
uint32_t pos_x_tasks[6];
uint32_t pos_y_tasks[6];

void sched_init() {
	for (int i = 0; i < 6; ++i){
		scheduler[i] = 0;
		tss_selectores[i] = (GDT_IDX_A1_DESC + 2 * i) << 3;
		pos_y_tasks[i] = 0;
		pos_x_tasks[i] = 0;
	}
	actual = 6; //cuando empezamos corre la idle
}

int16_t sched_nextTask() {
	int16_t res = GDT_IDX_IDLE_DESC << 3;
	actual = (actual + 1) % 7;
	if(actual != 6){ //si actual es 6 corre la idle
		if(scheduler[actual] != 0){
			if(handlers[actual] != 0){
				res = handlers[actual];
				TSS_ARRAY[actual + 1 + 6]->eip = (uint32_t)fun_handlers[actual];
			    TSS_ARRAY[actual + 1 + 6]->esp = 128 * 0x100000 + 8 * 0x400; //128MB + 8KB
			    TSS_ARRAY[actual + 1 + 6]->ebp = 0;
			    TSS_ARRAY[actual + 1 + 6]->esp0 = pila0_tareas[actual] + 4*0x400;
			    TSS_ARRAY[actual + 1 + 6]->ss0 = GDT_OFF_DS0_DESC;
			    TSS_ARRAY[actual + 1 + 6]->ds = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
			    TSS_ARRAY[actual + 1 + 6]->cs = GDT_OFF_CS3_DESC + 3;
			    TSS_ARRAY[actual + 1 + 6]->ss = GDT_OFF_DS3_DESC + 3;
			    TSS_ARRAY[actual + 1 + 6]->fs = GDT_OFF_DS3_DESC + 3;
			    TSS_ARRAY[actual + 1 + 6]->es = GDT_OFF_DS3_DESC + 3;
			    TSS_ARRAY[actual + 1 + 6]->gs = GDT_OFF_DS3_DESC + 3;
			    TSS_ARRAY[actual + 1 + 6]->eflags = 0x202; //para habilitar interrupciones
			    TSS_ARRAY[actual + 1 + 6]->edi = 0;
			    TSS_ARRAY[actual + 1 + 6]->esi = 0;
			    TSS_ARRAY[actual + 1 + 6]->ebx = 0;
			    TSS_ARRAY[actual + 1 + 6]->edx = 0;
			    TSS_ARRAY[actual + 1 + 6]->ecx = 0;
			    TSS_ARRAY[actual + 1 + 6]->eax = 0;
			    TSS_ARRAY[actual + 1 + 6]->edi = 0;
			} else {
				res = tss_selectores[actual];
			}
		}
	}
	return res;
}