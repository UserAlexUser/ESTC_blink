#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"

#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))

static const uint8_t led_list[LEDS_NUMBER] = LEDS_LIST;

void on_led(uint32_t led_idx)
{
    nrf_gpio_pin_write(led_list[led_idx], 1);
}

void off_led(uint32_t led_idx)
{
    nrf_gpio_pin_write(led_list[led_idx], 0);
}

void toggle_led(uint32_t led_idx)
{
    nrf_gpio_pin_toggle(led_list[led_idx]);
}

void init_led(uint32_t led_idx)
{
    nrf_gpio_cfg_output(led_list[led_idx]);
    off_led(led_list[led_idx]);  
}

int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);
    char cnt_led[4] = {6*2, 5*2, 8*2, 9*2};
    char number_led[4] = {0, 1, 2, 3};
    while (true)
    {
        for(int i=0; i<4; i++)
        {
            for(int j=0; j<cnt_led[i]; j++)
            {
                toggle_led(number_led[i]);
                nrf_delay_ms(250);
            }
        }
    }
}
