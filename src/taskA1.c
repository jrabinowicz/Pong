/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "syscall.h" 
#include "i386.h"

void handler(void);

void task() {
    char* message = "Tarea A1";
    //message = (char*)0x0;
    //*message = 3;
    syscall_talk(message);
    syscall_setHandler(handler);

    while(1) { __asm __volatile("mov $1, %%eax":::"eax"); }
}

void handler() {
    syscall_informAction(Center);
}
