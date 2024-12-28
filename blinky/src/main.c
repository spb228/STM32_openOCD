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