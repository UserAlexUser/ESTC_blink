#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "app_timer.h"
#include "function.h"

#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))
#define MAX_LEDS 4
#define COUNT_1KHz 1000
#define TIMER_1c 1000

char count_button = 0;
char double_click = 0;

APP_TIMER_DEF(button_timer);                                           

static void button_zero_out(void * p_context)
{
    count_button = 0;
}

static void timer_init(void)
{
    ret_code_t err_code;
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_create(&button_timer, APP_TIMER_MODE_SINGLE_SHOT, button_zero_out);
    APP_ERROR_CHECK(err_code);
}

static void timer_start(void)
{
    uint32_t err_code = app_timer_start(button_timer, APP_TIMER_TICKS(TIMER_1c), NULL);//таймер на 1 секунду
    APP_ERROR_CHECK(err_code);
}

void button_pressed(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{   
    count_button++;
    if(count_button == 2)
    {
        double_click = (double_click ? 0 : 1);
        count_button = 0;
    }
    timer_start();//ждём второе нажатие кнопки
}

static void interrupt_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);//приходит прерывание когда кнопка отпущена
    in_config.pull = NRF_GPIO_PIN_PULLUP;
    err_code = nrf_drv_gpiote_in_init(BSP_BUTTON_0, &in_config, button_pressed);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(BSP_BUTTON_0, true);
}

int main(void)
{   
    char cnt_led[4] = {6, 5, 8, 9};
    char number_led[4] = {0, 1, 2, 3};
    int count_leds = 0;
    int count_blink = 0;
    int count_smooth_blinking = 0;
    logs_init();
    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    nrfx_systick_init();

    NRF_LOG_INFO("HELLOW I'am Nordic");

    interrupt_init();
    timer_init();

    while (true)
    {   

        while((count_leds<MAX_LEDS) && ( double_click != 0))
        {     
            while((count_blink<cnt_led[count_leds]) && (double_click != 0))
            {   
                NRF_LOG_INFO("BLINK");
                for(count_smooth_blinking=0 ; (count_smooth_blinking<(COUNT_1KHz*2)) && (double_click != 0); count_smooth_blinking++)
                {   
                    do
                    {
                        on_led(number_led[count_leds]);
                        nrfx_systick_delay_us(MIN(count_smooth_blinking, (COUNT_1KHz*2) - count_smooth_blinking));
                        off_led(number_led[count_leds]);
                        nrfx_systick_delay_us(abs(COUNT_1KHz - count_smooth_blinking));
                    } while ( !double_click );
                }
                count_blink++;
                NRF_LOG_PROCESS();

            }
            if (count_blink>=cnt_led[count_leds])
            {
                count_blink=0;
                count_leds++;
            }
            if (count_leds>=MAX_LEDS)
            {
                count_leds=0;
            }
        }
        LOG_BACKEND_USB_PROCESS();
    }
}
