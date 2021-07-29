/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
  definicion de funciones del manejador de memoria
*/

#include "mmu.h"
#include "i386.h"

unsigned int proxima_pagina_libre_kernel;
unsigned int proxima_pagina_libre_task;

pd_entry* PD_TAREAS[6];
uint32_t PAG_CODIGO[6]; //guardamos las direc fisicas que usamos para copiar los codigos de las tareas

void mmu_init() {
	proxima_pagina_libre_kernel = INICIO_PAGINAS_LIBRES_KERNEL;
	proxima_pagina_libre_task = INICIO_PAGINAS_LIBRES_TASK;
}

uint32_t mmu_nextFreeKernelPage() {
	uint32_t res = proxima_pagina_libre_kernel;
	proxima_pagina_libre_kernel += 4096;
    return res;
}

uint32_t mmu_nextFreeTaskPage() {
	uint32_t res = proxima_pagina_libre_task;
	proxima_pagina_libre_task += 4096;
    return res;
}

void mmu_mapPage(uint32_t virtual, uint32_t cr3, uint32_t phy, uint8_t us, uint8_t rw) {

	uint32_t index_pd = (virtual >> 22);
	uint32_t index_pt = (virtual << 10);
	index_pt = (index_pt >> 22);
	//uint32_t offset = virtual:12;
	pd_entry* PD = (pd_entry*) cr3;
	pt_entry* PT;
	if(!PD[index_pd].p){

		PT = (pt_entry*) mmu_nextFreeKernelPage();

		PD[index_pd].p = 1;
		PD[index_pd].rw = 1;
		PD[index_pd].us = 1; //ponemos 1 para manejar el nivel desde la página directamente y no modificar el directorio según attrs
		PD[index_pd].pwt = 0;
		PD[index_pd].pcd = 0;
		PD[index_pd].a = 0;
	    PD[index_pd].ignored = 0;
		PD[index_pd].ps = 0;
		PD[index_pd].g = 0;
	    PD[index_pd].disponible = 0;
		PD[index_pd].base_31_12 = (uint32_t) PT >> 12 ;
		//inicializar PT
		for (int i = 0; i < 1024; ++i)
		{
			PT[i].p = 0;
		}
	} else {
		PT = (pt_entry*) (PD[index_pd].base_31_12 << 12);
	}

	PT[index_pt].p = 1;
	PT[index_pt].rw = rw;
	PT[index_pt].us = us;
	PT[index_pt].pwt = 0;
	PT[index_pt].pcd = 0;
	PT[index_pt].a = 0;
	PT[index_pt].d = 0;
	PT[index_pt].pat = 0;
	PT[index_pt].g = 0;
	PT[index_pt].disponible = 0;
	PT[index_pt].base_31_12 = (phy >> 12);

	tlbflush();

}

//esta funcion es buscar en el PD la PT que corresponde a la dir y poner un 0 en el bit Present
uint32_t mmu_unmapPage(uint32_t virtual, uint32_t cr3) {
    uint32_t index_pd = (virtual >> 22);
	uint32_t index_pt = (virtual << 10);
	index_pt = (index_pt >> 22);
	//uint32_t offset = virtual:12;

	pd_entry* PD = (pd_entry*) cr3;

	//si no esta mapeada no hacemos nada
	if(PD[index_pd].p){
		uint32_t indice = ( (PD[index_pd].base_31_12) << 12 );
		pt_entry* PT = (pt_entry*) indice;

    	PT[index_pt].p = 0;
	}
	
    tlbflush();
    
    return 0;
}

uint32_t mmu_initKernelDir() {
	/*una tabla = 4K
	Con una tabla direccionas 1024 páginas
	porque por página necesitas un desciptor que ocupa 4B 
	4B * 1024 = 4K
	Entonces modificamos la primer posicion del pde 
	y toda la pte correspondiente completa 
	1024 paginas spn 4MB*/	
	pd_entry* PD = (pd_entry*) 0x2B000;
	PD[0].p = 1;
	PD[0].rw = 1;
	PD[0].us = 0;
	PD[0].pwt = 0;
	PD[0].pcd = 0;
	PD[0].a = 0;
    PD[0].ignored = 0;
	PD[0].ps = 0;
	PD[0].g = 0;
    PD[0].disponible = 0;
	PD[0].base_31_12 = 0x2C000 >> 12; 
    
    pt_entry* PT = (pt_entry*) 0x2C000;
    for (uint32_t i = 0; i < 1024; ++i){
    	PT[i].p = 1;
		PT[i].rw = 1;
		PT[i].us = 0;
		PT[i].pwt = 0;
		PT[i].pcd = 0;
		PT[i].a = 0;
    	PT[i].d = 0;
		PT[i].pat = 0;
		PT[i].g = 0;
    	PT[i].disponible = 0;
		PT[i].base_31_12 = i; //no va shifteado pq estamos en la pos 0 del PD
    }
    return (uint32_t) PD;
}

void mapearTareas(){
	for (int i = 0; i < 6; ++i){
		PD_TAREAS[i] = (pd_entry*) mmu_nextFreeKernelPage();
		pt_entry* PT = (pt_entry*) mmu_nextFreeKernelPage();

		//armamos identity mapping para todas las tareas
		PD_TAREAS[i][0].p = 1;
		PD_TAREAS[i][0].rw = 1;
		PD_TAREAS[i][0].us = 0;
		PD_TAREAS[i][0].pwt = 0;
		PD_TAREAS[i][0].pcd = 0;
		PD_TAREAS[i][0].a = 0;
	    PD_TAREAS[i][0].ignored = 0;
		PD_TAREAS[i][0].ps = 0;
		PD_TAREAS[i][0].g = 0;
    	PD_TAREAS[i][0].disponible = 0;
		PD_TAREAS[i][0].base_31_12 = ((uint32_t)PT) >> 12; 


		for (uint32_t i = 0; i < 1024; ++i){
	    	PT[i].p = 1;
			PT[i].rw = 1;
			PT[i].us = 0;
			PT[i].pwt = 0;
			PT[i].pcd = 0;
			PT[i].a = 0;
	    	PT[i].d = 0;
			PT[i].pat = 0;
			PT[i].g = 0;
	    	PT[i].disponible = 0;
			PT[i].base_31_12 = i; //no va shifteado pq estamos en la pos 0 del PD

	    }

	    uint32_t p1 = mmu_nextFreeTaskPage();
	    uint32_t p2 = mmu_nextFreeTaskPage();

	    PAG_CODIGO[i] = p1;

    	mmu_mapPage(0x08000000, (uint32_t)PD_TAREAS[i], p1, 1, 1); //128 MB
    	mmu_mapPage(0x08000000 + 0x1000, (uint32_t)PD_TAREAS[i], p2, 1, 1); //128 MB + 4K
	}
}

void copiarCodigo(uint8_t jugador, uint8_t tipoTarea, uint32_t indice){

	uint32_t p1 = PAG_CODIGO[indice];
	uint32_t p2 = p1 + 0x1000;


	mmu_mapPage(p1, rcr3(), p1, 0, 0); //elegimos usar identity mapping para guardar en cr3 actual
    mmu_mapPage(p2, rcr3(), p2, 0, 0);

    uint32_t dirTarea;
    if(jugador == 0){
    	if(tipoTarea == 1){
    		dirTarea = 0x10000;
    	} else if(tipoTarea == 2){
    		dirTarea = 0x12000;
    	} else if(tipoTarea == 3){
    		dirTarea = 0x14000;
    	}
    } else {
    	if(tipoTarea == 1){
    		dirTarea = 0x16000;
    	} else if(tipoTarea == 2){
    		dirTarea = 0x18000;
    	} else if(tipoTarea == 3){
    		dirTarea = 0x1A000;
    	}
    }

    //hay que copiar 1 pagina
    for (int i = 0; i < 1024; ++i) 
    {
    	((uint32_t*)p1)[i] = ((uint32_t*)dirTarea)[i];
    }
    //copiamos la segunda pag
    for (int i = 0; i < 1024; ++i) 
    {
    	((uint32_t*)p2)[i] = ((uint32_t*)dirTarea)[i + 1024];
    }

    mmu_unmapPage(p1, rcr3()); 
    mmu_unmapPage(p2, rcr3());

}
