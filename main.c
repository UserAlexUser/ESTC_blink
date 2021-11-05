#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#define first_LED   0
#define second_LED  1
#define third_LED   2
#define fourth_LED  3

#define INIT_LED    0
#define ON_LED      1
#define OFF_LED     2
#define TOGGLE_LED  3

#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))

static const uint8_t led_list[LEDS_NUMBER] = LEDS_LIST;

void led_control(uint32_t led_idx,uint8_t cmd)
{
    if(cmd == INIT_LED)
    {   
        nrf_gpio_cfg_output(led_list[led_idx]);
        led_control(led_list[led_idx],OFF_LED);
    }    
    else if (cmd == ON_LED)
        nrf_gpio_pin_write(led_list[led_idx], 1);
    else if (cmd == OFF_LED)
        nrf_gpio_pin_write(led_list[led_idx], 0);
    else if (cmd == TOGGLE_LED)
        nrf_gpio_pin_toggle(led_list[led_idx]);
}
int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);
    char cnt_1 = 6*2;
    char cnt_2 = 5*2;
    char cnt_3 = 8*2;
    char cnt_4 = 9*2;
    /* Toggle LEDs. */
    while (true)
    {
        for(int i=0; i<cnt_1; i++)
            {   
                led_control(first_LED,TOGGLE_LED);
                nrf_delay_ms(250);
            }
        for(int i=0; i<cnt_2; i++)
            {   
                if(i%2)
                led_control(second_LED,ON_LED);
                else
                led_control(second_LED,OFF_LED);
                nrf_delay_ms(250);
            }
        for(int i=0; i<cnt_3; i++)
            {   
                nrf_gpio_pin_toggle(led_list[third_LED]);
                //bsp_board_led_invert(third_LED);   
                nrf_delay_ms(250);
            }
        for(int i=0; i<cnt_4; i++)
            {
                bsp_board_led_invert(fourth_LED); 
                nrf_delay_ms(250);
            }
    }
}
