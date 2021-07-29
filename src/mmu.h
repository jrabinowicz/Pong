/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#ifndef __MMU_H__
#define __MMU_H__

#include "stdint.h"
#include "defines.h"
#include "i386.h"
#include "tss.h"
#include "game.h"

typedef struct str_pd_entry {
    uint8_t   p:1;
    uint8_t   rw:1;
    uint8_t   us:1;
    uint8_t   pwt:1;
    uint8_t   pcd:1;
    uint8_t   a:1;
    uint8_t   ignored:1;
    uint8_t   ps:1;
    uint8_t   g:1;
    uint8_t   disponible:3;
    uint32_t  base_31_12:20;
    //si no anda es el aligned
} __attribute__((__packed__, aligned (4))) pd_entry;

typedef struct str_pt_entry {
    uint8_t   p:1;
    uint8_t   rw:1;
    uint8_t   us:1;
    uint8_t   pwt:1;
    uint8_t   pcd:1;
    uint8_t   a:1;
    uint8_t   d:1;
    uint8_t   pat:1;
    uint8_t   g:1;
    uint8_t   disponible:3;
    uint32_t  base_31_12:20;
    //si no anda es el aligned
} __attribute__((__packed__, aligned (4))) pt_entry;

void mmu_init();

uint32_t mmu_nextFreeKernelPage();

uint32_t mmu_nextFreeTaskPage();

void mmu_mapPage(uint32_t virtual, uint32_t cr3, uint32_t phy, uint8_t us, uint8_t rw);

uint32_t mmu_unmapPage(uint32_t virtual, uint32_t cr3);

uint32_t mmu_initKernelDir();

void mapearTareas();

void copiarCodigo(uint8_t jugador, uint8_t tipoTarea, uint32_t indice);

extern pd_entry* PD_TAREAS[6];

#endif	/* !__MMU_H__ */




