/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#ifndef __GAME_H__
#define __GAME_H__

#include "stdint.h"
#include "defines.h"
#include "screen.h"
#include "mmu.h"
#include "sched.h"
#include "colors.h"

typedef void (*f_handler_t)();

typedef enum e_action {
    Up = 1,
    Center = 2,
    Down = 3,
} e_action_t;

void game_init();

void idt_init();
void sys_talk(char* m);
uint32_t sys_where_x();
uint32_t sys_where_y();
void sys_setHandler();

void movJugadores();
void movPelotas();
int32_t esHandler(uint16_t tarea);
void matarTarea(int32_t tarea);
void printGoles(uint16_t jug);
void leerTeclado(uint8_t letra);
void crearPelota(uint8_t jug, uint8_t tipo);
uint16_t selTareaActual();
void modoDebug();
uint32_t estaEnPausa();
void excepcion(uint32_t nro);
void terminoJuego();

extern f_handler_t* fun_handlers[6];
extern e_action_t acciones[6];
extern uint32_t contador;
extern uint32_t modo_debug;

#endif  /* !__GAME_H__ */
