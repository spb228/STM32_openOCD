#include <stdint.h>

// Forward declarations
void Reset_Handler(void);
void Default_Handler(void);
extern int main(void);

// Linker script symbols
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

#define STACK_START 0x20018000U   // End of RAM for STM32F401RE

// Vector table
uint32_t vectors[] __attribute__((section(".isr_vector"))) = {
    STACK_START,                    // Stack pointer
    (uint32_t)Reset_Handler,        // Reset
    (uint32_t)Default_Handler,      // NMI
    (uint32_t)Default_Handler,      // Hard Fault
    (uint32_t)Default_Handler,      // MemManage
    (uint32_t)Default_Handler,      // BusFault
    (uint32_t)Default_Handler,      // UsageFault
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    (uint32_t)Default_Handler,      // SVCall
    (uint32_t)Default_Handler,      // Debug Monitor
    0,                              // Reserved
    (uint32_t)Default_Handler,      // PendSV
    (uint32_t)Default_Handler       // SysTick
};

// Default handler for interrupts
void Default_Handler(void) {
    while(1) {}
}

// Reset handler
void Reset_Handler(void) {
    // Copy data from FLASH to RAM
    uint32_t *pSrc = &_etext;
    uint32_t *pDest = &_sdata;
    while (pDest < &_edata) {
        *pDest++ = *pSrc++;
    }

    // Zero fill BSS
    uint32_t *pBss = &_sbss;
    while (pBss < &_ebss) {
        *pBss++ = 0;
    }

    // Call main
    main();

    // Loop forever if main returns
    while(1) {}
}