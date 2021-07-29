/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"


f_handler_t* fun_handlers[6];
enum e_action acciones[6];
uint32_t contador;
enum e_action direccionJug[2]; //cuando los movemos reseteamos su direccion a centro
uint32_t pos_jugadores[2];
uint32_t goles[2];
uint32_t pelotas_restantes[2];
uint32_t inversa_y[6]; //si esta inversa 1
uint32_t inversa_x[6]; //si esta inversa 1
uint32_t cruces_pelotas[6]; //tiene las posiciones en x de las cajitas donde se imprime que slots estan disponibles
                            //la pos en y es 31
uint32_t modo_debug; // 0 = no estamos en modo debug, 1 = modo debug, 2 = modo debug y juego pausado 
char* text_excep[20]; //Para el modo debug

void game_init() {
	for (int i = 0; i < 6; ++i){
		fun_handlers[i] = 0;
		acciones[i] = Center;
        inversa_y[i] = 0;
        inversa_x[i] = 0;
        slots_disponibles[i] = 0;
        if (i > 2)
            inversa_x[i] = 1;
	}
    for (int i = 0; i < 2; ++i){
        direccionJug[i] = Center;
        pos_jugadores[i] = 20;
        goles[i] = 0;
        pelotas_restantes[i] = 15;
    }
    contador = 0;
    modo_debug = 0;
    cruces_pelotas[0] = 20;
    cruces_pelotas[1] = 22;
    cruces_pelotas[2] = 24;
    cruces_pelotas[3] = 59;
    cruces_pelotas[4] = 61;
    cruces_pelotas[5] = 63;
    //excepciones
    text_excep[0] = "Divide error #DE";
    text_excep[1] = "Debug #DB";
    text_excep[2] = "Non Maskable Interrupt";
    text_excep[3] = "Breakpoint #BE";
    text_excep[4] = "Overflow #OF";
    text_excep[5] = "BOUND Range Exceeded #BR";
    text_excep[6] = "Invalid Opcode #UD";
    text_excep[7] = "Device Not Available #MM";
    text_excep[8] = "Double Fault #DF";
    text_excep[9] = "Coprocessor Segment Overrun";
    text_excep[10] = "Invalid TSS #TS";
    text_excep[11] = "Non Present Segment #NP";
    text_excep[12] = "Stack Fault #SS";
    text_excep[13] = "General Protection Fault #GP";
    text_excep[14] = "Page Fault #PF";
    text_excep[15] = "q onda rrope";
    text_excep[16] = "FPU Error #MF";
    text_excep[17] = "Alignment Check #AC";
    text_excep[18] = "Machine Check #MC";
    text_excep[19] = "SIMD Floatin Point #XM";


    //printear pantalla
    screen_drawBox(0,1,40,78,' ', C_FG_LIGHT_GREY + C_BG_LIGHT_GREY);//fondo
    screen_drawBox(0,0,40,1,' ', C_FG_WHITE + C_BG_WHITE);//col jug0
    screen_drawBox(0,79,40,1,' ', C_FG_WHITE + C_BG_WHITE);//col jug1
    screen_drawBox(17,0,7,1,' ', C_FG_WHITE + C_BG_RED);//jug0
    screen_drawBox(17,79,7,1,' ', C_FG_WHITE + C_BG_BLUE);//jug1
    screen_drawBox(40,0,10,80,' ', C_FG_BLACK + C_BG_BLACK);//bordes tablero
    screen_drawBox(41,1,8,38,' ', C_FG_RED + C_BG_RED);//tablero jug0
    screen_drawBox(41,40,8,39,' ', C_FG_BLUE + C_BG_BLUE);//tablero jug1
    print("Puntos: 00",2,42,C_FG_BLACK + C_BG_WHITE);
    print("Puntos: 00",41,42,C_FG_BLACK + C_BG_WHITE);
    print("Pelotas: 15",2,43,C_FG_BLACK + C_BG_WHITE);
    print("Pelotas: 15",41,43,C_FG_BLACK + C_BG_WHITE);
    for (int i = 0; i < 6; ++i){
        print(" ",cruces_pelotas[i],42,C_FG_BLACK + C_BG_WHITE);
    }

}


void sys_talk(char* m){
    uint32_t x = 2; //si es del jug1
    uint32_t y = 45;
    if(actual > 2){ //si es del jug2
         x = 41;
         if(actual == 4){
            y = 46;
         } else if(actual == 5){
            y = 47;
         }
    } else { //si es del jug1
        if(actual == 1){
            y = 46;
         } else if(actual == 2){
            y = 47;
         }
    }
    
    print(m,x,y,C_FG_BLACK + C_BG_WHITE);   

}

uint32_t sys_where_y(){
    return pos_y_tasks[actual];
  
}

uint32_t sys_where_x(){
    if(actual > 2){
        return 78 - pos_x_tasks[actual];
    } else {
        return pos_x_tasks[actual];
    }
}

void sys_setHandler(f_handler_t* f){
    handlers[actual] = (GDT_IDX_A1_H_DESC + 2 * actual) << 3;//(uint32_t)TSS_ARRAY[actual + 1 + 6];
    TSS_ARRAY[actual + 1 + 6]->eip = (uint32_t)f;
    fun_handlers[actual] = f;
}

void sys_informAction(e_action_t action){
   /* up = 1
    center = 2
    down = 3 */
    acciones[actual] = action;
}

uint16_t selTareaActual(){
    return tss_selectores[actual];
}

void movJugadores(){
    if(contador == 30){
        contador = 0;
        uint32_t x = 0;
        uint32_t color = C_FG_RED + C_BG_RED;
        for (int i = 0; i < 2; ++i){
            if (i == 1){
                x = 79;
                color = C_BG_BLUE + C_FG_BLUE;
            }
            if(direccionJug[i] == Up){
                if(pos_jugadores[i] - 3 > 0){
                    print(" ", x, pos_jugadores[i] - 4, color);
                    print(" ", x, pos_jugadores[i] + 3, C_BG_WHITE + C_FG_WHITE);
                    pos_jugadores[i] -= 1;
                }
            } else if(direccionJug[i] == Down){
                if(pos_jugadores[i] + 3 < 39){
                    print(" ", x, pos_jugadores[i] + 4, color);
                    print(" ", x, pos_jugadores[i] - 3, C_BG_WHITE + C_FG_WHITE);
                    pos_jugadores[i] += 1;
                }
            }
            direccionJug[i] = Center;
        }
    } else {
        contador++;
    }
}

void movPelotas(){
    if(actual == 6){
        for (int i = 0; i < 6; ++i){
            uint32_t x = pos_x_tasks[i];
            uint32_t y = pos_y_tasks[i];
            if(scheduler[i]){
                if(inversa_y[i] == 1){
                    if(acciones[i] == Up){
                        if(y == 39){
                            inversa_y[i] = 0;
                            // mover para arriba
                            y = 38;
                        } else {
                            // mover para abajo
                            y++;
                        }
                    } else if(acciones[i] == Down){
                        if (y == 0){
                            inversa_y[i] = 0;
                            // mover para abajo
                            y = 1;
                        } else {
                            // mover para arriba
                            y--;
                        }
                    }
                } else {
                    if (acciones[i] == Up){
                        if (y == 0){
                            inversa_y[i] = 1;
                            // mover para abajo
                            y = 1;
                        } else{
                            // mover para arriba
                            y--;
                        }
                    } else if (acciones[i] == Down){
                        if (y == 39){
                            inversa_y[i] = 1;
                            // mover para arriba
                            y = 38;
                        } else {
                            // mover para abajo
                            y++;
                        }
                    }
                }
                if(inversa_x[i] == 1){
                    x--;
                } else {
                    x++;
                }
                uint8_t fueGol = 0;
                //chequeamos si es gol o si rebota en x
                if(x == 0){
                    if(acciones[i] == Down){ //la pelota viene de arriba
                        if((pos_jugadores[0] - 3 < y) && (y <= pos_jugadores[0] + 4)){
                            //rebota contra la barra
                            x = 2;
                            inversa_x[i] = 0;
                        } else {
                            //gol jug 1
                            fueGol = 1;
                            matarTarea(i);
                            goles[1]++;
                            printGoles(1);
                        }
                    } else if(acciones[i] == Up){ //la pelota viene de abajo
                        if((pos_jugadores[0] - 4 <= y) && (y < pos_jugadores[0] + 3)){
                            //rebota contra la barra
                            x = 2;
                            inversa_x[i] = 0;
                        } else {
                            //gol jug 1
                            fueGol = 1;
                            matarTarea(i);
                            goles[1]++;
                            printGoles(1);
                        }
                    } else { //la pelota viene del centro
                        if((pos_jugadores[0] - 3 <= y) && (y <= pos_jugadores[0] + 3)){
                            //rebota contra la barra
                            x = 2;
                            inversa_x[i] = 0;
                        } else {
                            //gol jug 1
                            fueGol = 1;
                            matarTarea(i);
                            goles[1]++;
                            printGoles(1);
                        }
                    }
                } else if(x == 79){
                    if(acciones[i] == Down){ //la pelota viene de arriba
                        if((pos_jugadores[1] - 3 < y) && (y <= pos_jugadores[1] + 4)){
                            //rebota contra la barra
                            x = 77;
                            inversa_x[i] = 1;
                        } else {
                            //gol jug 0
                            fueGol = 1;
                            matarTarea(i);
                            goles[0]++;
                            printGoles(0);
                        }
                    } else if(acciones[i] == Up){ //la pelota viene de abajo
                        if((pos_jugadores[1] - 4 <= y) && (y < pos_jugadores[1] + 3)){
                            //rebota contra la barra
                            x = 77;
                            inversa_x[i] = 1;
                        } else {
                            //gol jug 0
                            fueGol = 1;
                            matarTarea(i);
                            goles[0]++;
                            printGoles(0);
                        }
                    } else { //la pelota viene del centro
                        if((pos_jugadores[1] - 3 <= y) && (y <= pos_jugadores[1] + 3)){
                            //rebota contra la barra
                            x = 77;
                            inversa_x[i] = 1;
                        } else {
                            //gol jug 0
                            fueGol = 1;
                            matarTarea(i);
                            goles[0]++;
                            printGoles(0);
                        }
                    }
                }
                if(!fueGol){
                    //printear pelota en el nuevo lugar
                    uint32_t color = C_BG_LIGHT_GREY + C_FG_RED;
                    if(i > 2){
                        color = C_BG_LIGHT_GREY + C_FG_BLUE;
                    }
                    print("O",x,y,color);
                }
                //borrar pelota del lugar anterior
                if(x != 0 && x != 79)
                    print(" ",pos_x_tasks[i],pos_y_tasks[i],C_BG_LIGHT_GREY + C_FG_RED);
                //actualizar pos_y y pos_x
                pos_x_tasks[i] = x;
                pos_y_tasks[i] = y;
            }
        }
    }
}

int32_t esHandler(uint16_t tarea){
    int32_t res = -1;
    uint32_t indice_gdt = tarea >> 3;
    if((indice_gdt != 20) && (indice_gdt % 2 == 0)){ //para que no entre cuando la tarea es la init
        res = actual;
    }    
    return res;

}

void matarTarea(int32_t tarea){
    //slots_disponibles(printearlo), scheduler, handlers[tarea] = 0, borrar la tarea de la pantalla
    if(tarea > -1){
        scheduler[tarea] = 0;
        slots_disponibles[tarea] = 0;
        handlers[tarea] = 0;
        print(" ",cruces_pelotas[tarea],42,C_FG_WHITE + C_BG_WHITE);
        uint32_t x = 2;
        uint8_t color = C_BG_RED + C_FG_RED;
        if(tarea > 2){
            x = 2 + 39;
            color = C_BG_BLUE + C_FG_BLUE;
        }
        //borrar la pelota
        print(" ", pos_x_tasks[tarea], pos_y_tasks[tarea], C_BG_LIGHT_GREY + C_FG_LIGHT_GREY);
        screen_drawBox((tarea % 3) + 45,x,1,20,' ',color);
    }
}

void printGoles(uint16_t jug){
    print_dec(goles[jug],2,10 + jug*39,42,C_FG_BLACK + C_BG_WHITE);
}

void leerTeclado(uint8_t letra){
    if(letra == 0x11){
        direccionJug[0] = Up;
    } else if(letra == 0x1f){
        direccionJug[0] = Down;
    } else if(letra == 0x17){
        direccionJug[1] = Up;
    } else if(letra == 0x25){
        direccionJug[1] = Down;
    } else if(letra == 0x2c){
        crearPelota(0,1);
    } else if(letra == 0x2d){
        crearPelota(0,2);
    } else if(letra == 0x2e){
        crearPelota(0,3);
    } else if(letra == 0x30){
        crearPelota(1,1);
    } else if(letra == 0x31){
        crearPelota(1,2);
    } else if(letra == 0x32){
        crearPelota(1,3);
    } else if(letra == 0x15){
        modoDebug();
    }
}

void crearPelota(uint8_t jug, uint8_t tipo){
    //escribir en pantalla, modificar scheduler, slots disponibles, posiciones, inversa, pelotas restantes (numero e imprimir)
    //copiar codigo, chequear que haya pelotas restantes y slots disponibles
    uint32_t indice = -1;
    if(pelotas_restantes[jug] > 0){
        uint8_t haySlot = 0;
        for (int i = jug * 3; i < (3 + jug * 3); ++i){
            if(slots_disponibles[i] == 0){
                haySlot = 1;
                slots_disponibles[i] = tipo;
                indice = i;
                break;
            }
        }

        if(haySlot){
            tss_init_task(jug, tipo, indice);
            pelotas_restantes[jug]--;
            inversa_y[indice] = 0;
            inversa_x[indice] = jug;
            pos_x_tasks[indice] = jug * 77 + 1;
            pos_y_tasks[indice] = pos_jugadores[jug];
            scheduler[indice] = 1;
            //imprimimos los cambios
            print("X",cruces_pelotas[indice],42,C_BG_WHITE + C_FG_BLACK);
            print_dec(pelotas_restantes[jug],2,11 + jug * 39,43,C_BG_WHITE + C_FG_BLACK);
            uint16_t color;
            if(jug == 0){
                color =  C_BG_LIGHT_GREY + C_FG_RED;
            } else {
                color =  C_BG_LIGHT_GREY + C_FG_BLUE;
            }
            print("O", pos_x_tasks[indice], pos_y_tasks[indice], color);
        }
    }
}

void modoDebug(){
    if(modo_debug == 0){
        modo_debug = 1;
    } else if(modo_debug == 1){
        modo_debug = 0;
    } else {
        //sacar pantalla
        modo_debug = 1;
        screen_drawBox(0,1,40,78,' ', C_FG_LIGHT_GREY + C_BG_LIGHT_GREY);//fondo
        //imprimimos las pelotas de nuevo
        for (int i = 0; i < 6; ++i){
            if(scheduler[i]){
                uint16_t color = C_FG_RED + C_BG_LIGHT_GREY;
                if(i > 2){
                    color = C_FG_BLUE + C_BG_LIGHT_GREY;
                }
                print("O",pos_x_tasks[i],pos_y_tasks[i],color);
            }
        }
    }
}

uint32_t estaEnPausa(){
    return modo_debug;
}

void excepcion(uint32_t nro){
    if (modo_debug == 1){
        modo_debug = 2;
        tss* tss_actual = TSS_ARRAY[1 + actual];

        screen_drawBox(1,25,39,30,' ', C_FG_BLACK + C_BG_BLACK);
        screen_drawBox(2,26,1,28,' ', C_FG_LIGHT_GREEN + C_BG_LIGHT_GREEN);
        print_dec(nro, 2, 26, 2, C_FG_BLACK + C_BG_LIGHT_GREEN);
        print(text_excep[nro], 29, 2, C_FG_BLACK + C_BG_LIGHT_GREEN);
        screen_drawBox(4,26,36,28,' ', C_FG_LIGHT_GREY + C_BG_LIGHT_GREY);
        print("eax", 27, 5, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("ebx", 27, 7, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("ecx", 27,  9, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("edx",  27, 11, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("esi",  27, 13, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("edi",  27, 15, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("ebp",  27, 17, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("esp",  27, 19, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("eip",  27, 21, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("cs", 28, 23, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("ds", 28, 25, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("es", 28, 27, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("fs", 28, 29, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("gs", 28, 31, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("ss", 28, 33, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("eflags", 28, 36, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("stack", 41, 26, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("cr0",41,  6, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("cr2",41,  8, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("cr3",41,  10, C_FG_BLACK + C_BG_LIGHT_GREY);
        print("cr4",41,  12, C_FG_BLACK + C_BG_LIGHT_GREY);

        print_hex(tss_actual->eax, 8,31,  5, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->ebx, 8,31,  7, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->ecx, 8,31,  9, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->edx, 8, 31, 11, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->esi, 8, 31, 13, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->edi, 8, 31, 15, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->ebp, 8, 31, 17, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->esp, 8, 31, 19, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->eip, 8, 31, 21, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->cs, 4, 31, 23, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->ds, 4, 31, 25, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->es, 4, 31, 27, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->fs, 4, 31, 29, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->gs, 4, 31, 31, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->ss, 4, 31, 33, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->eflags, 8, 35, 36, C_FG_WHITE + C_BG_LIGHT_GREY);

        print_hex(((uint32_t*) tss_actual->esp)[0], 8, 41, 28, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(((uint32_t*) tss_actual->esp)[4], 8, 41, 29, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(((uint32_t*) tss_actual->esp)[8], 8, 41, 30, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(((uint32_t*) tss_actual->esp)[12], 8, 41, 31, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(((uint32_t*) tss_actual->esp)[16], 8, 41, 32, C_FG_WHITE + C_BG_LIGHT_GREY);

        print_hex(rcr0(), 8, 45, 6, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(rcr2(), 8, 45, 8, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(tss_actual->cr3, 8, 45, 10, C_FG_WHITE + C_BG_LIGHT_GREY);
        print_hex(rcr4(), 8, 45, 12, C_FG_WHITE + C_BG_LIGHT_GREY);        

    }
    matarTarea(actual);
}

void terminoJuego(){
    if(pelotas_restantes[0] == 0 && pelotas_restantes[1] == 0){
        uint8_t pelotasEnJuego = 0;
        for (int i = 0; i < 6; ++i){
            if(scheduler[i])
                pelotasEnJuego = 1;
        }
        if(!pelotasEnJuego){
            screen_drawBox(6,25,10,30,' ', C_FG_BLACK + C_BG_BLACK);
            uint8_t color = C_FG_WHITE + C_BG_RED;
            uint32_t x = 32;
            char* text = "Gano jugador A!!!";
            if(goles[1] > goles[0]){
                color = C_FG_WHITE + C_BG_BLUE;
                text = "Gano jugador B!!!";
            } else if(goles[1] == goles[0]){
                color = C_FG_BLACK + C_BG_LIGHT_GREEN;
                text = "Empate!!!";
                x = 34;
            }    
            screen_drawBox(7,26,8,28,' ',color);
            print(text,x,10,color);
            modo_debug = 2;
        }
    }
}