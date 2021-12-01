#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "function.h"

#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))
#define MAX_LEDS 4
#define COUNT_1KHz 1000
#define TIMER_1c 1000

char flag_condition = 0;//0-положение по умолчанию 1-выбираем цвет 2-выбираем насыщенность 3-выбираем яркость
static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
char count_button = 0;
#define USED_PWM(idx) (1UL << idx)
static uint8_t m_used = 0;
static uint16_t const              m_LED_0_top  = 10000;
static uint16_t                    m_LED_0_step = 0;
static uint8_t                     m_LED_0_phase;
static nrf_pwm_values_individual_t m_led_0_seq_values;
static nrf_pwm_sequence_t const    m_LED_0_seq =
{
    .values.p_individual = &m_led_0_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_led_0_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};
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
static void colors_init(void)
{

}

static void saturation_init(void)
{

}

static void brightness_init(void)
{

}

void button_pressed(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{   
    count_button++;
    if(count_button == 4)
    {
        // double_click = (double_click ? 0 : 1);
        count_button = 0;
        flag_condition++;//переходим в следующие состояние
    }
    timer_start();//ждём второе нажатие кнопки

    if(flag_condition == 1) // включаемплавное изменение цветов
    {
        m_LED_0_step = 300;
        colors_init();
    }
    else if(flag_condition == 2) // выбираем насыщенность
    {
        m_LED_0_step = 700;
        saturation_init();
    }
    else if(flag_condition == 3) // выбираем яркость
    {
        m_LED_0_step = 34464;
        brightness_init();
    }
    else if(flag_condition > 3) // переходим в начальное состояние
    {
        flag_condition = 0;
        m_LED_0_step = 0;
    }
}

static void interrupt_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);//приходит прерывание когда кнопка нажата
    in_config.pull = NRF_GPIO_PIN_PULLUP;
    err_code = nrf_drv_gpiote_in_init(BSP_BUTTON_0, &in_config, button_pressed);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(BSP_BUTTON_0, true);
}

static void LED_0_handler(nrf_drv_pwm_evt_type_t event_type)
{
    if (event_type == NRF_DRV_PWM_EVT_FINISHED)
    {
        uint8_t channel    = m_LED_0_phase >> 1;

        uint16_t * p_channels = (uint16_t *)&m_led_0_seq_values;
        uint16_t value = p_channels[channel];
        if (m_LED_0_step == 0)
        {
            value = m_LED_0_step;
        }
        else if (m_LED_0_step == 34464)
        {
            value = m_LED_0_step;
        }
        else
        {
            value += m_LED_0_step;
        }
        p_channels[channel] = value;
    }
}
static void LED_0(void)
{
        nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            BSP_LED_0 | NRF_DRV_PWM_PIN_INVERTED, // channel 0
            NRF_DRV_PWM_PIN_NOT_USED, // channel 1
            NRF_DRV_PWM_PIN_NOT_USED, // channel 2
            NRF_DRV_PWM_PIN_NOT_USED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_1MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = m_LED_0_top,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, LED_0_handler));
    m_used |= USED_PWM(0);

    m_led_0_seq_values.channel_0 = 0;
    m_led_0_seq_values.channel_1 = 0;
    m_led_0_seq_values.channel_2 = 0;
    m_led_0_seq_values.channel_3 = 0;
    m_LED_0_phase                = 0;

    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &m_LED_0_seq, 1, NRF_DRV_PWM_FLAG_LOOP);
}

int main(void)
{   
    logs_init();
    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    nrfx_systick_init();
    NRF_LOG_INFO("HELLOW I'am Nordic");
    interrupt_init();
    timer_init();
    LED_0();

    while (true)
    {   
        // Wait for an event.
        __WFE();

        // Clear the event register.
        __SEV();
        __WFE();

        NRF_LOG_FLUSH();
    }
}
