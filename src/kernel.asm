; ** por compatibilidad se omiten tildes **
; ==============================================================================
; TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
; ==============================================================================

%include "print.mac"

global start
extern GDT_DESC
extern IDT_DESC
extern idt_init
extern pic_reset
extern pic_enable
extern mmu_init
extern mmu_initKernelDir
extern mmu_initTaskDir
extern mapearTareas
extern tss_init
extern sched_init
extern game_init
%define C_FG_WHITE 0xF
%define C_BG_CYAN 0x30

%define GDT_OFF_DS0_DESC 15 << 3
%define GDT_OFF_IDLE_DESC 19 << 3
%define GDT_OFF_INIT_DESC 20 << 3

;; Saltear seccion de datos
jmp start

;;
;; Seccion de datos.
;; -------------------------------------------------------------------------- ;;
start_rm_msg db     'Iniciando kernel en Modo Real'
start_rm_len equ    $ - start_rm_msg

start_pm_msg db     'Iniciando kernel en Modo Protegido'
start_pm_len equ    $ - start_pm_msg

libretas_pm_msg db  '193/18, 281/18, 48/18'
libretas_pm_len equ    $ - libretas_pm_msg
;;
;; Seccion de código.
;; -------------------------------------------------------------------------- ;;

;; Punto de entrada del kernel.
BITS 16
start:
    ; Deshabilitar interrupciones
    cli

    ; Cambiar modo de video a 80 X 50
    mov ax, 1003h ;ax = 1003h -> para poder tener 16 colores de background
    int 10h ; set mode 03h
    xor bx, bx
    mov ax, 1112h
    int 10h ; load 8x8 font

    ; Imprimir mensaje de bienvenida
    print_text_rm start_rm_msg, start_rm_len, 0x07, 0, 0
    
    ; Habilitar A20
    call A20_enable

    ; Cargar la GDT
    lgdt [GDT_DESC]

    ; Setear el bit PE del registro CR0
    mov eax,cr0
    or eax,1
    mov cr0,eax
 
    ; Saltar a modo protegido
    jmp 0x70:modo_protegido

BITS 32
modo_protegido:
    
    ; Establecer selectores de segmentos
    mov ax, GDT_OFF_DS0_DESC
    mov ds, ax ;ds = data nivel cero
    mov gs, ax ;gs = data nivel cero
    mov fs, ax ;fs = data nivel cero
    mov ss, ax ;ss = data nivel cero

    ; Establecer la base de la pila
    mov esp, 0x2B000
  
    ; Imprimir mensaje de bienvenida
    print_text_pm start_pm_msg, start_pm_len, 0x07, 2, 0

    ; Inicializar pantalla
    mov ax, 0x90
    mov es, ax ;es = pantalla

    ; Rutina para pintar pantalla
    mov ecx, 80*50
    mov dl, C_FG_WHITE
    or dl, C_BG_CYAN
    mov eax, 0
.rutinaPantalla:
    cmp ecx, 0
    je .finRutina
    mov byte [es:eax], ' '
    mov [es:eax+1], dl
    inc eax
    inc eax
    dec ecx
    jmp .rutinaPantalla
.finRutina:

    ; Inicializar el manejador de memoria
    call mmu_init

    ; Printear libretas
    print_text_pm libretas_pm_msg, libretas_pm_len, 0x08, 20, 20

    ; Inicializar el directorio de paginas
    call mmu_initKernelDir

    ; Cargar directorio de paginas
    mov cr3, eax

    ; Habilitar paginacion
    mov eax,cr0
    or eax,1 << 31
    mov cr0,eax

    ; Inicializar tss
    call tss_init

    ; Inicializar tss de la tarea Idle

    ; Inicializar el scheduler
    call sched_init
    
    ; Inicializar la IDT
    call idt_init

    ; Cargar IDT
    lidt [IDT_DESC]

    ; Configurar controlador de interrupciones
    call pic_reset
    call pic_enable

    ; Cargar tarea inicial
    mov ax, GDT_OFF_INIT_DESC
    ltr ax
    ; ;testeando memoria
    ; push 1
    ; push 1
    ; call mmu_initTaskDir
    ; ;xchg bx,bx
    ; mov cr3, eax
    
    call mapearTareas
    call game_init

    ; Habilitar interrupciones
    sti

    ; Saltar a la primera tarea: Idle
    jmp GDT_OFF_IDLE_DESC:0

    ; Ciclar infinitamente (por si algo sale mal...)
    mov eax, 0xFFFF
    mov ebx, 0xFFFF
    mov ecx, 0xFFFF
    mov edx, 0xFFFF
    jmp $

;; -------------------------------------------------------------------------- ;;

%include "a20.asm"
