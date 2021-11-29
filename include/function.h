#ifndef FUNCTION_H__
#define FUNCTION_H__

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

void on_led(uint32_t led_idx);
void off_led(uint32_t led_idx);
void toggle_led(uint32_t led_idx);
void init_led(uint32_t led_idx);
bool button_state(uint32_t btn_idx);
void logs_init();

#endif // FUNCTION_H__
