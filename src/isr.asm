; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================
; definicion de rutinas de atencion de interrupciones

%include "print.mac"

BITS 32

sched_task_offset:     dd 0xFFFF
sched_task_selector:   dw 0xFFFFFFFF

;; PIC
extern pic_finish1

;; Sched
extern sched_nextTask

;; Syscalls
extern sys_talk
extern sys_where_x
extern sys_where_y
extern sys_setHandler
extern sys_informAction

;; AUXS
extern movJugadores
extern movPelotas
extern matarTarea
extern esHandler
extern leerTeclado
extern selTareaActual
extern estaEnPausa

extern excepcion
%define GDT_OFF_IDLE_DESC 19 << 3
extern terminoJuego

;;
;; Definición de MACROS
;; -------------------------------------------------------------------------- ;;

%macro ISR 1
global _isr%1

_isr%1:
    mov eax, %1
    push eax
    call excepcion
    pop eax
    jmp GDT_OFF_IDLE_DESC:0
.fin:    
    jmp $

%endmacro


;; Rutina de atención de las EXCEPCIONES
;; -------------------------------------------------------------------------- ;;
ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19

;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32 ;falta chequear el tr y ver si es  una tss de handler en cuyo caso hay que matar a la tarea
_isr32:
    pushad
    call pic_finish1
    call estaEnPausa ;por el modo debug
    cmp eax, 2 ;está en pausa
    je .fin
    call terminoJuego
    call nextClock
    call movJugadores
    call movPelotas
    xor ecx, ecx
    str cx
    xor eax, eax
    push ecx
    call esHandler ;devuelve un -1 si no es un handler, si no devuelve el índice de la tarea a matar
    add esp, 4
    push eax
    call matarTarea ;si recibe un cero no hace nada, si no mata tarea
    add esp, 4
    call sched_nextTask
    str cx ;por las dudas
    cmp ax, cx
    je .fin
    mov [sched_task_selector], ax
    jmp far [sched_task_offset]
.fin:
    popad
    iret

;; Rutina de atención del TECLADO
;; -------------------------------------------------------------------------- ;;
nros: db '1','2','3','4','5','6','7','8','9','0' 

global _isr33
_isr33:
    pushad
    call pic_finish1
    xor eax, eax
    in al, 0x60 ;Leemos del teclado a traves del puerto 0x60
    sub al, 0x80 ;Cuando se suelta la tecla se genera el break code y se suma 0x80 al valor del make code
    push eax
    call leerTeclado
    pop eax 
    sub al, 2
    cmp al, 9
    jg .fin
    cmp al, 0
    jl .fin
    xor ebx, ebx
    mov bl, al
    add ebx, nros
    xor ecx, ecx
    print_text_pm ebx, 1, 0x0f, 0, 79
   

.fin:    
    popad
    iret
    
;; Rutinas de atención de las SYSCALLS
;; -------------------------------------------------------------------------- ;;
global _isr47
_isr47:
    pushad
    cmp eax, 0x80000001
    je .talk
    cmp eax, 0x80000002
    je .where
    cmp eax, 0x80000003
    je .setHandler
    cmp eax, 0x8000FFFF
    je .informAction

.talk:
    push ebx ;char*m
    call sys_talk
    add esp, 4
    jmp .fin

.where:
    call sys_where_x
    mov [esp + 16], eax ;por el pushad ahi esta ebx
    call sys_where_y
    mov [esp + 24], eax ;ahi esta ecx
    jmp .fin        

.setHandler:
    push ebx
    call sys_setHandler
    add esp, 4
    jmp .fin

.informAction:
    push ebx
    call sys_informAction
    ;hay que correr el código de la tarea
    call selTareaActual
    str cx
    cmp ax, cx
    je .fin
    mov [sched_task_selector], ax
    jmp far [sched_task_offset]     

.fin:
    popad
    iret

;; Funciones Auxiliares
;; -------------------------------------------------------------------------- ;;
isrNumber:           dd 0x00000000
isrClock:            db '|/-\'
nextClock:
        pushad
        inc DWORD [isrNumber]
        mov ebx, [isrNumber]
        cmp ebx, 0x4
        jl .ok
                mov DWORD [isrNumber], 0x0
                mov ebx, 0
        .ok:
                add ebx, isrClock
                print_text_pm ebx, 1, 0x0f, 49, 79
                popad
        ret
