#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"

#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))

static const uint8_t led_list[LEDS_NUMBER] = LEDS_LIST;
static const uint8_t btn_list[BUTTONS_NUMBER] = BUTTONS_LIST;

void on_led(uint32_t led_idx)
{
    nrf_gpio_pin_write(led_list[led_idx], 0);
}

void off_led(uint32_t led_idx)
{
    nrf_gpio_pin_write(led_list[led_idx], 1);
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

bool button_state(uint32_t btn_idx)
{
    bool pin_set = nrf_gpio_pin_read(btn_list[btn_idx]) ? true : false;
    return(pin_set == (BUTTONS_ACTIVE_STATE ? true : false));
}

int main(void)
{
    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    char cnt_led[4] = {6, 5, 8, 9};
    char number_led[4] = {0, 1, 2, 3};
    int i = 0;
    int j = 0;
    while (true)
    {   
        while((i<4) && (button_state(BSP_BUTTON_0)))
        {     
            while((j<cnt_led[i]) && (button_state(BSP_BUTTON_0)))
            {   

                on_led(number_led[i]);
                nrf_delay_ms(500);
                off_led(number_led[i]);
                nrf_delay_ms(500);
                j++;

            }
            if (j>=cnt_led[i])
            {
                j=0;
                i++;
            }
            if (i>=4)
            {
                i=0;
            }
        }
    }
}
