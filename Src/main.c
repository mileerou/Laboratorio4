#include "stm32f4xx.h"


#define LED_PIN   (1U << 5)

static void delay(volatile uint32_t n) { while(n--) {} }

static void gpio_led_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER &= ~(3U << (5U*2U));
    GPIOA->MODER |=  (1U << (5U*2U));
}

static void led_toggle(void) { GPIOA->ODR ^= LED_PIN; }

static void send_frame(const char* payload)
{
    const uint8_t STX = 0x02, ETX = 0x03;
    uint8_t len = 0;

    while (payload[len] && len < 50) len++;

    uart2_write_char((char)STX);
    uart2_write_char((char)len);

    uint8_t chk = len;
    for (uint8_t i = 0; i < len; i++) {
        uart2_write_char(payload[i]);
        chk ^= (uint8_t)payload[i];
    }
    uart2_write_char((char)chk);
    uart2_write_char((char)ETX);
    uart2_write_str("\r\n");
}

int main(void)
{
    gpio_led_init();

    const uint32_t baud = 115200;
    uart2_init(baud, 0);

    uint32_t cnt = 0;

    while (1)
    {
        uart2_write_str("0123456789ABCDEF\r\n");

        char msg[16] = "CNT=000000";
        uint32_t v = cnt;
        for (int i = 9; i >= 4; i--) { msg[i] = (char)('0' + (v % 10U)); v /= 10U; }
        send_frame(msg);

        led_toggle();
        cnt++;
        delay(2000000);
    }
}


