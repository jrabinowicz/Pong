/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de la tabla de descriptores globales
*/

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "stdint.h"
#include "game.h"

#define LS_INLINE static __inline __attribute__((always_inline))

/*
 * Syscalls
 */

LS_INLINE void syscall_talk(char* message) {
    int32_t func = 0x80000001;
  __asm__ volatile (
    "int $47"        /* make the request to the OS */
    :                /* */
    : "a"  (func),   /* pass in eax ("a") */
      "b"  (message) /* pass in ebx ("b") */
    : "memory", "cc"); /* announce to the compiler that the memory and condition codes have been modified */
}

LS_INLINE void syscall_where(uint32_t* x, uint32_t* y) {
    int32_t func = 0x80000002;
    uint32_t xx, yy;
  __asm__ volatile (
    "int $47"      /* make the request to the OS */
    : "=b"  (xx),   /* pass in ebx ("b") */
      "=c"  (yy)    /* pass in ecx ("c") */
    : "a"   (func)  /* pass in eax ("a") */
    : "memory", "cc"); /* announce to the compiler that the memory and condition codes have been modified */
    *x = xx;
    *y = yy;
}

LS_INLINE void syscall_setHandler(f_handler_t handler) {
    int32_t func = 0x80000003;
  __asm__ volatile (
    "int $47"          /* make the request to the OS */
    :                  /* */
    : "a"  (func),     /* pass in eax ("a") arg "func"*/
      "b"  (handler)   /* pass in ebx ("b") arg "handler"*/                
    : "memory", "cc"); /* announce to the compiler that the memory and condition codes have been modified */
}

LS_INLINE void syscall_informAction(e_action_t action) {
    int32_t func = 0x8000ffff;
  __asm__ volatile (
    "int $47"      /* make the request to the OS */
    : /*  */
    : "a"  (func),  /* pass in eax ("a") */
      "b"  (action) /* pass in ebx ("b") */
    : "memory", "cc"); /* announce to the compiler that the memory and condition codes have been modified */
}

#endif  /* !__SYSCALL_H__ */
