#include <stdint.h>

#define RCC_BASE        0x40023800U
#define RCC_CR          (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR     (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR        (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30))

#define FLASH_BASE      0x40023C00U
#define FLASH_ACR       (*(volatile uint32_t *)(FLASH_BASE + 0x00))


static void pll_setup(void)
{
    RCC_CR |= (1 << 16); // enable HSE
    while (!(RCC_CR & (1 << 17))); // wait for HSERDY

    // configure PLL (M = 4, N = 168, P = 2, Q = 7)
    RCC_PLLCFGR = (4 << 0)   |  // PLLM = 4
                  (84 << 6) |  // PLLN = 84
                  (0 << 16)  |  // PLLP = 2 (00)
                  (7 << 24)  |  // PLLQ = 7
                  (1 << 22);    // PLL source = HSE

    RCC_CR |= (1 << 24); // PLLON 
    while (!(RCC_CR & (1 << 25))); // PLLRDY

    FLASH_ACR &= ~(7 << 0);  // Clear previous settings
    FLASH_ACR |= (5 << 0);   // Set latency to 5 WS

    // Set APB1 = /4 (42 MHz), APB2 = /2 (84 MHz)
    RCC_CFGR &= ~((7 << 10) | (7 << 13)); // Clear PPRE1 and PPRE2
    RCC_CFGR |= (4 << 10) | (2 << 13);    // APB1 = /4, APB2 = /2

    // Switch system clock to PLL
    RCC_CFGR &= ~(3 << 0); // Clear SW bits
    RCC_CFGR |= (2 << 0);  // Set SW = 10 (PLL selected)

    // Wait until PLL is used as the system clock
    while ((RCC_CFGR & (3 << 2)) != (2 << 2));
}