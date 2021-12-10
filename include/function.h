#ifndef FUNCTION_H__
#define FUNCTION_H__

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"

#include "nrf_drv_pwm.h"
#include "app_timer.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"

#include "nrfx_systick.h"

extern char count_button;
extern char flag_condition;

extern uint16_t begin_value_1;
extern uint16_t begin_value_2;
extern uint16_t begin_value_3;

extern uint16_t second_value_1;
extern uint16_t second_value_2;
extern uint16_t second_value_3;

extern uint16_t flash_value_1;
extern uint16_t flash_value_2;
extern uint16_t flash_value_3;

extern bool timer_end;
extern uint16_t m_led_0_step;

extern bool state_button;

extern int phase;

#define MAX_LEDS 4
#define COUNT_1KHz 1000
#define TIMER_1c 1000
#define MAX_BRIGHT 10000//34464
#define MIN_BRIGHT 0
#define BRIGHT_STEP 100//96//32
#define MAX_STEP 100//359//1077
#define OFF 0  

void on_led(uint32_t led_idx);
void off_led(uint32_t led_idx);
void toggle_led(uint32_t led_idx);
void init_led(uint32_t led_idx);
bool button_state(uint32_t btn_idx);
void logs_init();
void LED_0(void);
void LED_0_handler(nrf_drv_pwm_evt_type_t event_type);
void interrupt_init(void);
void LED_RGB(void);
void button_zero_out(void * p_context);
void timer_init(void);
void timer_start(void);
void LED_RGB_handler(nrf_drv_pwm_evt_type_t event_type);
void button_pressed(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
void flash_write(void);

#endif // FUNCTION_H__
