/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "syscall.h" 

void handler(void);

void task() {
    char* message = "Tarea B1";
    syscall_talk(message);
    syscall_setHandler(handler);

    while(1) { __asm __volatile("mov $4, %%eax":::"eax"); }
}

void handler() {
    syscall_informAction(Center);
}
