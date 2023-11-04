#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <videodriver.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <MemoryManager.h>
#include <scheduler.h>
#include <manageProcess.h>
#include <interrupts.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();

void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary() {
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	return getStackBase();
}

int main() {	

	// la cre o antes asi no lo puede interrumpir 
	createMemoryManager();
	
	initializeScheduler();
	
	//Elige escribir directo en pantalla.
	setScreenBuffer(1);	
	drawTopLine();

	//!creo proceso 
	execve( &haltProcess, 0, 0, NULL);

	execve((void*)0x400000, 1, 1, NULL);

	// Carga de descriptores del IDT.
	load_idt(); 	
	
	
	//! para forzar int 0x20 para syscalls
	forceTimerInt();
	
	// LAS SYS ESTAN DESACTIVADAASS
	// sin PushState
	// mem 

	char * space = allocMemory(20);

	// Escribe en la heapppp
	// cheq ultima direccion kernel
	void * ans = getStackBase();
	//ans = ;
	numToStr((uint64_t)&ans ,16,space);
	drawString( space, 0XFF00FF, 0X000000 );
	
	numToStr((uint64_t)&bss ,16,space);
	drawString( space, 0X0000FF, 0X000000 );
	space[10]='a';
	space[11]='b';
	space[12]=0;

	drawString( space+10, 0XFF00FF, 0X000000 );
	// Libera	
	freeMemory(space);
	space = allocMemory(20);
	drawString( space+10, 0XFF00FF, 0X000000 );


	space = allocMemory(2000);
	drawString( space+10, 0XFF00FF, 0X000000 );

	
	//!deberia deshabili desp
	//*sino puedo poner int 20h
	return 0;
	
	// Llamado a la Shell.
//	((EntryPoint)sampleCodeModuleAddress)();
	
}
