#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"
#define first_LED   0
#define second_LED  1
#define third_LED   2
#define fourth_LED  3

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
                bsp_board_led_invert(first_LED); 
                nrf_delay_ms(500);
            }
        for(int i=0; i<cnt_2; i++)
            {  
                bsp_board_led_invert(second_LED); 
                nrf_delay_ms(500);
            }
        for(int i=0; i<cnt_3; i++)
            {
                bsp_board_led_invert(third_LED);   
                nrf_delay_ms(500);
            }
        for(int i=0; i<cnt_4; i++)
            {
                bsp_board_led_invert(fourth_LED); 
                nrf_delay_ms(500);
            }
    }
}
