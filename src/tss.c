/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de estructuras para administrar tareas
*/

#include "tss.h"
#include "mmu.h"
#include "gdt.h"
#include "i386.h"
#include "defines.h"

tss tss_initial;
tss tss_idle;
tss tss_A1;
tss tss_A1_handler;
tss tss_A2;
tss tss_A2_handler;
tss tss_A3;
tss tss_A3_handler;
tss tss_B1;
tss tss_B1_handler;
tss tss_B2;
tss tss_B2_handler;
tss tss_B3;
tss tss_B3_handler;
uint32_t handlers[6]; //tiene el selector de la tss del handler si ya se seteo por la tarea, si no 0
uint32_t pila0_tareas[6]; //para las pilas de nivel 0
uint8_t slots_disponibles[6]; //si hay 0 está desocupado, si no está el tipo de tarea
                              //Las primeras 3 posiciones son del jug1
tss* TSS_ARRAY[13];

void tss_init() {

	for (int i = 0; i < 6; ++i){
		pila0_tareas[i] = mmu_nextFreeKernelPage();
		handlers[i] = 0;
		slots_disponibles[i] = 0;
	}

	tss_idle.eip = 0x1C000;
	tss_idle.esp = 0x2B000; //direccion de la pila de kernel s/ enunciado
	tss_idle.ebp = 0x2B000;
	tss_idle.ds = GDT_OFF_DS0_DESC;
	tss_idle.cs = GDT_OFF_CS0_DESC;
	tss_idle.gs = GDT_OFF_DS0_DESC;
	tss_idle.fs = GDT_OFF_DS0_DESC;
	tss_idle.ss = GDT_OFF_DS0_DESC;
	tss_idle.es = GDT_OFF_DS0_DESC;
	tss_idle.cr3 = rcr3(); //como la funcion es llamada por el kernel entonces va a tener el mismo cr3
	tss_idle.eflags = 0x202; //para habilitar interrupciones
	tss_idle.edi = 0;
	tss_idle.esi = 0;
	tss_idle.ebx = 0;
	tss_idle.edx = 0;
	tss_idle.ecx = 0;
	tss_idle.eax = 0;
	tss_idle.edi = 0;

	TSS_ARRAY[0] = &tss_idle;
	TSS_ARRAY[1] = &tss_A1;
	TSS_ARRAY[2] = &tss_A2;
	TSS_ARRAY[3] = &tss_A3;
	TSS_ARRAY[4] = &tss_B1;
	TSS_ARRAY[5] = &tss_B2;
	TSS_ARRAY[6] = &tss_B3;
	TSS_ARRAY[7] = &tss_A1_handler;
	TSS_ARRAY[8] = &tss_A2_handler;
	TSS_ARRAY[9] = &tss_A3_handler;
	TSS_ARRAY[10] = &tss_B1_handler;
	TSS_ARRAY[11] = &tss_B2_handler;
	TSS_ARRAY[12] = &tss_B3_handler;

	completarGDT();
}

void completarGDT(){
	gdt[GDT_IDX_IDLE_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_idle & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_idle) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_idle) >> 24), /* base[31:24]  */
    };

    gdt[GDT_IDX_INIT_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_initial & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_initial) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,              /* type         */
        (uint8_t)     0x00,              /* s            */
        (uint8_t)     0x00,              /* dpl          */
        (uint8_t)     0x01,              /* p            */
        (uint8_t)     0x00,              /* limit[16:19] */
        (uint8_t)     0x01,              /* avl          */
        (uint8_t)     0x00,              /* l            */
        (uint8_t)     0x01,              /* db           */
        (uint8_t)     0x00,              /* g            */
        (uint8_t)     (((uint32_t)&tss_initial) >> 24), /* base[31:24]  */
    };

	gdt[GDT_IDX_A1_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_A1 & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_A1) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_A1) >> 24), /* base[31:24]  */
    };

    gdt[GDT_IDX_A1_H_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_A1_handler & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_A1_handler) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_A1_handler) >> 24), /* base[31:24]  */
    };

    gdt[GDT_IDX_A2_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_A2 & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_A2) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_A2) >> 24), /* base[31:24]  */
    };

    gdt[GDT_IDX_A2_H_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_A2_handler & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_A2_handler) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_A2_handler) >> 24), /* base[31:24]  */
    };

    gdt[GDT_IDX_A3_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_A3 & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_A3) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_A3) >> 24), /* base[31:24]  */
    };

    gdt[GDT_IDX_A3_H_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_A3_handler & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_A3_handler) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_A3_handler) >> 24), /* base[31:24]  */
    };

	gdt[GDT_IDX_B1_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_B1 & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_B1) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_B1) >> 24), /* base[31:24]  */
    };

    gdt[GDT_IDX_B1_H_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_B1_handler & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_B1_handler) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_B1_handler) >> 24), /* base[31:24]  */
    };

	gdt[GDT_IDX_B2_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_B2 & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_B2) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_B2) >> 24), /* base[31:24]  */
    };

    gdt[GDT_IDX_B2_H_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_B2_handler & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_B2_handler) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_B2_handler) >> 24), /* base[31:24]  */
    };

	gdt[GDT_IDX_B3_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_B3 & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_B3) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_B3) >> 24), /* base[31:24]  */
    };

    gdt[GDT_IDX_B3_H_DESC] = (gdt_entry) {
        (uint16_t)    0x0067,            /* limit[0:15]  */
        (uint16_t)    ((uint32_t)&tss_B3_handler & 0xFFFF),       /* base[0:15]   */
        (uint8_t)     (((uint32_t)&tss_B3_handler) >> 16 &  0xFF), /* base[23:16]  */
        (uint8_t)     0x09,           /* type         */
        (uint8_t)     0x00,           /* s            */
        (uint8_t)     0x00,           /* dpl          */
        (uint8_t)     0x01,           /* p            */
        (uint8_t)     0x00,           /* limit[16:19] */
        (uint8_t)     0x01,           /* avl          */
        (uint8_t)     0x00,           /* l            */
        (uint8_t)     0x01,           /* db           */
        (uint8_t)     0x00,           /* g            */
        (uint8_t)     (((uint32_t)&tss_B3_handler) >> 24), /* base[31:24]  */
    };    
}


void tss_init_task(uint8_t jugador, uint8_t tipo, uint32_t indice){
	//buscamos la tss libre
	tss* tarea = TSS_ARRAY[indice + 1];
	tss* handler = TSS_ARRAY[(indice + 1) + 6];

	//mapeamos pila de nivel 0
	mmu_mapPage(0x08000000 + 0x2000, (uint32_t)PD_TAREAS[indice], pila0_tareas[indice], 0, 0);

	//completamos las tss
	tarea->cr3 = (uint32_t)PD_TAREAS[indice];
    tarea->eip = 128 * 0x100000; //es a donde mapeamos el codigo de la tarea en mapearTareas
	tarea->esp = 128 * 0x100000 + 7 * 0x400; //128MB + 7KB
	tarea->ebp = 0; //como todo respeta convencion C, no hace falta inicializarlo en ningun valor en particular
	tarea->esp0 = pila0_tareas[indice] + 2*0x400;
	tarea->ss0 = GDT_OFF_DS0_DESC;
	tarea->ds = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
	tarea->cs = GDT_OFF_CS3_DESC + 3;
    tarea->ss = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
    tarea->es = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
    tarea->fs = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
    tarea->gs = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
	tarea->eflags = 0x202; //para habilitar interrupciones
	tarea->edi = 0;
	tarea->esi = 0;
	tarea->ebx = 0;
	tarea->edx = 0;
	tarea->ecx = 0;
	tarea->eax = 0;
	tarea->edi = 0;

	handler->cr3 = tarea->cr3;
	handler->eip = 0;
	handler->esp = 128 * 0x100000 + 8 * 0x400; //128MB + 8KB
	handler->ebp = 0;
	handler->esp0 = pila0_tareas[indice] + 4*0x400;
	handler->ss0 = GDT_OFF_DS0_DESC;
	handler->ds = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
	handler->cs = GDT_OFF_CS3_DESC + 3;
    handler->ss = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
    handler->es = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
    handler->fs = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
    handler->gs = GDT_OFF_DS3_DESC + 3; //el 3 es el RPL
	handler->eflags = 0x202; //para habilitar interrupciones
	handler->edi = 0;
	handler->esi = 0;
	handler->ebx = 0;
	handler->edx = 0;
	handler->ecx = 0;
	handler->eax = 0;
	handler->edi = 0;

	copiarCodigo(jugador, tipo, indice);
}