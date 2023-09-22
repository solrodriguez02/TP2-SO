#include <stdint.h>
#include <idtLoader.h>
#include <defs.h>
#include <interrupts.h>

#pragma pack(push)  // Push de la alineacion actual
#pragma pack (1)    // Alinea las siguientes estructuras a 1 byte

// IDT (Descriptor Table)
typedef struct {
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop)		// Reestablece la alineacion

// Puntero a la Interrupt Descriptor Table
DESCR_INT * idt = (DESCR_INT *) 0;	// IDT de 255 entradas

/**
 * @brief Agrega a la tabla de descriptores de interrupciones en la posición dada el puntero a la
 * rutina de atención de la interrupción.
 * 
 * @param index Posición en la tabla de descriptores de interrupciones.
 * @param offset Puntero a la rutina de atención de interrupción.
 */
static void setup_IDT_entry (int index, uint64_t offset) {
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t) 0;
}
/**
 * @brief Carga la tabla de descriptores de interrupciones del sistema.
 */
void load_idt() {
  // Clear Interrupts
  _cli();

  // La interrupción IRQ0 del Timer Tick corresponde a la 0x20. 
  // La rutina de atencion de interrupción es la función _irq00Handler.
  setup_IDT_entry(0x20, (uint64_t)&_irq00Handler);
  // La interrupción IRQ1 del teclado corresponde a la 0x21. 
  // La rutina de atención de interrupción es la función _irq01Handler.
  setup_IDT_entry(0x21, (uint64_t)&_irq01Handler);
  // La interrupción IRQ08 del RTC corresponde a la 0x28.
  // La rutina de atención de interrupción es la función _irq08Handler.
  // Además, activo las interrupciones periódicas para actualizar la fecha actual del sistema sobre la pantalla.
  _irq08init();
  setup_IDT_entry(0x28, (uint64_t)&_irq08Handler);
  // La interrupción 0x80 corresponde a los llamados del UserSpace al Kernel, las System Calls.
  // La rutina de atencion de interrupción es la función syscallsHandler.
  setup_IDT_entry(0x80, (uint64_t)&syscallsHandler);

	// Habilito sobre el Programmable Interrupt Controller Master la interrupcion de teclado y el TimerTick.
	picMasterMask(0xF8);
  // Habilite sobre el Programmable Interrupt Controller Slave la interrupción del módulo de Real Time Clock.
	picSlaveMask(0xFE);

  // La posición 0x0 de la tabla de descriptores corresponde a la excepción de división por cero.
  // La rutina de atención para la excepción es la función exception0Handler.
  setup_IDT_entry(0x0, (uint64_t)&_exception0Handler);
  // La posición 0x0 de la tabla de descriptores corresponde a la excepción de operación invalida.
  // La rutina de atención para la excepción es la función exception6Handler.
  setup_IDT_entry(0x6, (uint64_t)&_exception6Handler);
        
  // Start Interrupts
	_sti();
}