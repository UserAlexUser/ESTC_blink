#include "function.h"

#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))
#define MAX_LEDS 4
#define COUNT_1KHz 1000

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

void logs_init()
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}