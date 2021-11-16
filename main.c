#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"

#include "nrfx_systick.h"

#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))
#define MAX_LEDS 4
#define COUNT_1KHz 1000

static const uint8_t led_list[LEDS_NUMBER] = LEDS_LIST;
static const uint8_t btn_list[BUTTONS_NUMBER] = BUTTONS_LIST;
char count_button = 0;
char double_click = 0;

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

void logs_init()
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void button_pressed(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{   
    count_button++;
    if(count_button == 2)
    {
        double_click = (double_click ? 0 : 1);
        count_button = 0;
    }
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
    int i = 0;
    int j = 0;
    int jj = 0;
    logs_init();
    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    nrfx_systick_init();

    NRF_LOG_INFO("HELLOW I'am Nordic");

    interrupt_init();

    while (true)
    {   

        while((i<MAX_LEDS) && ( double_click != 0))
        {     
            while((j<cnt_led[i]) && (double_click != 0))
            {   
                NRF_LOG_INFO("BLINK");
                for(jj=0 ; (jj<(COUNT_1KHz*2)) && (double_click != 0); jj++)
                {   
                    do
                    {
                        on_led(number_led[i]);
                        nrfx_systick_delay_us(MIN(jj, (COUNT_1KHz*2) - jj));
                        off_led(number_led[i]);
                        nrfx_systick_delay_us(abs(COUNT_1KHz-jj));
                    } while ( !double_click );
                }
                j++;
                NRF_LOG_PROCESS();

            }
            if (j>=cnt_led[i])
            {
                j=0;
                i++;
            }
            if (i>=MAX_LEDS)
            {
                i=0;
            }
        }
        LOG_BACKEND_USB_PROCESS();
    }
}
