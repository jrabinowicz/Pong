/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

    Definiciones globales del sistema.
*/

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Bool */
/* -------------------------------------------------------------------------- */
#define TRUE	0x00000001
#define FALSE	0x00000000
#define ERROR	1

/* Atributos paginas */
/* -------------------------------------------------------------------------- */
#define PAG_P	0x00000001
#define PAG_R	0x00000000
#define PAG_RW	0x00000002
#define PAG_S	0x00000000
#define PAG_US	0x00000004

/* Misc */
/* -------------------------------------------------------------------------- */

/* Indices en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_COUNT 33

#define GDT_IDX_NULL_DESC           0
#define GDT_IDX_CS0_DESC            14
#define GDT_IDX_DS0_DESC            15
#define GDT_IDX_CS3_DESC            16
#define GDT_IDX_DS3_DESC            17
#define GDT_IDX_PP_DESC             18
#define GDT_IDX_IDLE_DESC           19
#define GDT_IDX_INIT_DESC           20
#define GDT_IDX_A1_DESC             21
#define GDT_IDX_A1_H_DESC           22
#define GDT_IDX_A2_DESC             23
#define GDT_IDX_A2_H_DESC           24
#define GDT_IDX_A3_DESC             25
#define GDT_IDX_A3_H_DESC           26
#define GDT_IDX_B1_DESC             27
#define GDT_IDX_B1_H_DESC           28
#define GDT_IDX_B2_DESC             29
#define GDT_IDX_B2_H_DESC           30
#define GDT_IDX_B3_DESC             31
#define GDT_IDX_B3_H_DESC           32

/* Offsets en la gdt */
/* -------------------------------------------------------------------------- */
#define GDT_OFF_NULL_DESC           (GDT_IDX_NULL_DESC << 3)
#define GDT_OFF_DS0_DESC            (GDT_IDX_DS0_DESC << 3)
#define GDT_OFF_CS0_DESC            (GDT_IDX_CS0_DESC << 3)
#define GDT_OFF_DS3_DESC            (GDT_IDX_DS3_DESC << 3)
#define GDT_OFF_CS3_DESC            (GDT_IDX_CS3_DESC << 3)
#define GDT_OFF_IDLE_DESC           (GDT_IDX_IDLE_DESC << 3)

/* Atributos segmentos */
/* -------------------------------------------------------------------------- */
#define SEG_TYPE_RW	0x00000002

/* Selectores de segmentos */
/* -------------------------------------------------------------------------- */

/* Direcciones de memoria */
/* -------------------------------------------------------------------------- */
#define BOOTSECTOR	0x00001000 /* direccion fisica de comienzo del bootsector (copiado) */
#define KERNEL		0x00001200 /* direccion fisica de comienzo del kernel */
#define VIDEO		0x000B8000 /* direccion fisica del buffer de video */

/* Direcciones virtuales de código, pila y datos */
/* -------------------------------------------------------------------------- */
#define TASK_CODE	0x08000000 /* direccion virtual del codigo */

/* Direcciones fisicas de codigos */
/* -------------------------------------------------------------------------- */
/* En estas direcciones estan los códigos de todas las tareas. De aqui se
 * copiaran al destino indicado por TASK_<i>_CODE_ADDR.
 */

/* Direcciones fisicas de directorios y tablas de paginas del KERNEL */
/* -------------------------------------------------------------------------- */
#define KERNEL_PAGE_DIR		0x00027000
#define KERNEL_PAGE_TABLE_0	0x00028000

#define INICIO_PAGINAS_LIBRES_KERNEL	0x100000
#define INICIO_PAGINAS_LIBRES_TASK		0x400000

#endif  /* !__DEFINES_H__ */
