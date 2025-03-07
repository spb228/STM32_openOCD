#include <stdint.h>
#include "pll_setup.c"

// RCC and GPIO registers
#define RCC_BASE        0x40023800U
#define RCC_AHB1ENR    (*(volatile uint32_t *)(RCC_BASE + 0x30))

#define GPIOA_BASE     0x40020000U
#define GPIOA_MODER    (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR      (*(volatile uint32_t *)(GPIOA_BASE + 0x14))

#define LED_PIN        (5)  // PA5 is the onboard LED

// Simple delay function
void delay(uint32_t delay)
{
    while (delay--)
    {
        __asm("nop");
    }
}

int main(void) 
{
    pll_setup(); 
    
    uint32_t sysclk = (RCC_CFGR >> 2) & 0x3;
    if (sysclk != 2)
    {
        while (1); // Trap here if PLL is not the system clock
    }

    // Enable GPIOA clock
    RCC_AHB1ENR |= (1 << 0); 

    // Configure PA5 as output
    GPIOA_MODER &= ~(3 << (LED_PIN * 2)); // clear out bits 10 and 11
    GPIOA_MODER |= (1 << (LED_PIN * 2)); // set bit 10 for output

    // while loop
    while (1)
    {
        GPIOA_ODR ^= (1 << LED_PIN); 
        delay(5000000);
    }

    return 0;
}