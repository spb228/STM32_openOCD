# STM32F401RE Bare Metal Blinky Project

## Project Structure
```
blinky/
├── src/
│   ├── main.c
│   └── startup.c
├── .vscode/
│   ├── launch.json
│   ├── c_cpp_properties.json
│   └── tasks.json
├── Makefile
└── stm32f401re.ld
```

## 1. main.c
```c
#include <stdint.h>

// RCC and GPIO registers
#define RCC_BASE        0x40023800U
#define RCC_AHB1ENR    (*(volatile uint32_t *)(RCC_BASE + 0x30))

#define GPIOA_BASE     0x40020000U
#define GPIOA_MODER    (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR      (*(volatile uint32_t *)(GPIOA_BASE + 0x14))

// Bit positions
#define RCC_AHB1ENR_GPIOAEN  (1U << 0)
#define LED_PIN              5  // PA5 is the onboard LED

// Simple delay function
static void delay(volatile uint32_t count) {
    while (count--) {
        __asm("nop");
    }
}

int main(void) {
    // Enable GPIOA clock
    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Configure PA5 as output
    GPIOA_MODER &= ~(3U << (LED_PIN * 2));  // Clear bits
    GPIOA_MODER |= (1U << (LED_PIN * 2));   // Set as output

    while (1) {
        // Toggle LED
        GPIOA_ODR ^= (1U << LED_PIN);
        delay(500000);
    }

    return 0;
}
```

## 2. startup.c
```c
#include <stdint.h>

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

// Prototypes
void Reset_Handler(void);
void Default_Handler(void);
extern int main(void);

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

// These are defined by the linker
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
```

## 3. stm32f401re.ld (Linker Script)
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH

    .data :
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM AT > FLASH

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM
}
```

## 4. Makefile
```makefile
# Project Name
TARGET = blinky

# Directories
BUILD_DIR = build
SRC_DIR = src

# Toolchain
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
SIZE = $(PREFIX)size

# MCU Config
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# Source Files
C_SOURCES = \
$(SRC_DIR)/main.c \
$(SRC_DIR)/startup.c

# Includes
C_INCLUDES = \
-I$(SRC_DIR)

# Compiler Flags
C_DEFS = \
-DSTM32F401xE

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) \
-Wall -Wextra \
-Os \
-ffunction-sections \
-fdata-sections \
-ffreestanding \
-nostdlib

# Linker
LDSCRIPT = stm32f401re.ld
LDFLAGS = $(MCU) -nostartfiles -T$(LDSCRIPT) \
-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# Build Rules
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/$(TARGET).elf: $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
	$(CC) $^ $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
```

## 5. .vscode/launch.json
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug (OpenOCD)",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "openocd",
            "cwd": "${workspaceRoot}",
            "executable": "${workspaceRoot}/build/blinky.elf",
            "configFiles": [
                "interface/stlink.cfg",
                "target/stm32f4x.cfg"
            ],
            "runToEntryPoint": "main",
            "device": "STM32F401RE"
        }
    ]
}
```

## Build and Flash Instructions

1. Create the project structure as shown above
2. Create all files with the content provided
3. Build the project:
```bash
make clean
make
```
4. Flash using OpenOCD:
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program build/blinky.elf verify reset exit"
```

Or use VS Code's debug functionality to flash and debug.

The LED on PA5 (the built-in LED on the NUCLEO board) should start blinking.

Important Notes:
1. The onboard LED is on PA5 for the STM32F401RE NUCLEO board
2. The delay timing might need adjustment based on your needs
3. Make sure all paths in the configurations match your system setup

Would you like me to explain any specific part in more detail?
