#include "function.h"

#define NRF_GPIO_PIN_MAP(port, pin) (((port) << 5) | ((pin) & 0x1F))
#define MAX_LEDS 4
#define COUNT_1KHz 1000

static const uint8_t led_list[LEDS_NUMBER] = LEDS_LIST;
static const uint8_t btn_list[BUTTONS_NUMBER] = BUTTONS_LIST;

bool timer_end = true;
APP_TIMER_DEF(button_timer);  

char flag_condition = 5;//0-положение по умолчанию 1-выбираем цвет 2-выбираем насыщенность 3-выбираем яркость
static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
char count_button = 0;
bool state_button = false;
int phase = 0;
uint16_t const              m_led_0_top  = 10000;
uint16_t                    m_led_0_step = 0;
static uint8_t                     m_led_0_phase;
static nrf_pwm_values_individual_t m_led_0_seq_values;

static nrf_pwm_sequence_t const    m_led_0_seq =
{
    .values.p_individual = &m_led_0_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_led_0_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

static nrf_drv_pwm_t m_pwm1 = NRF_DRV_PWM_INSTANCE(1);
static uint16_t const              m_led_rgb_top  = 10000;
static uint8_t                     m_led_rgb_phase;
static nrf_pwm_values_individual_t m_led_rgb_seq_values;
static nrf_pwm_sequence_t const    m_led_rgb_seq =
{
    .values.p_individual = &m_led_rgb_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_led_rgb_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

uint16_t save_value_1 = 0;
uint16_t save_value_2 = 0;
uint16_t save_value_3 = 0;

uint16_t begin_value_1 = 0;
uint16_t begin_value_2 = 0;
uint16_t begin_value_3 = 0;

uint16_t second_value_1 = 0;
uint16_t second_value_2 = 0;
uint16_t second_value_3 = 0;

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

void LED_0_handler(nrf_drv_pwm_evt_type_t event_type)
{
    if (event_type == NRF_DRV_PWM_EVT_FINISHED)
    {
        uint8_t channel    = m_led_0_phase;
        uint16_t * p_channels = (uint16_t *)&m_led_0_seq_values;
        uint16_t value = p_channels[channel];
        if (m_led_0_step == 0)
        {
            value = m_led_0_step;
        }
        else if (m_led_0_step == MAX_BRIGHT)
        {
            value = m_led_0_step;
        }
        else
        {
            value += m_led_0_step;
        }
        p_channels[channel] = value;
    }
}

void LED_0(void)
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
        .top_value    = m_led_0_top,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, LED_0_handler));
    // m_used |= USED_PWM(0);

    m_led_0_seq_values.channel_0 = 0;
    m_led_0_seq_values.channel_1 = 0;
    m_led_0_seq_values.channel_2 = 0;
    m_led_0_seq_values.channel_3 = 0;
    m_led_0_phase                = 0;

    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &m_led_0_seq, 1, NRF_DRV_PWM_FLAG_LOOP);
}

void interrupt_init(void)
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

void LED_RGB(void)
{
        nrf_drv_pwm_config_t const config1 =
    {
        .output_pins =
        {
            NRF_DRV_PWM_PIN_NOT_USED, // channel 0
            BSP_LED_1 | NRF_DRV_PWM_PIN_INVERTED, // channel 1
            BSP_LED_2 | NRF_DRV_PWM_PIN_INVERTED, // channel 2
            BSP_LED_3 | NRF_DRV_PWM_PIN_INVERTED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,    //< Приоритет прерываний
        .base_clock   = NRF_PWM_CLK_1MHz,           //< Базовая тактовая частота
        .count_mode   = NRF_PWM_MODE_UP,            //< Режим работы счетчика генератора импульсов.
        .top_value    = m_led_rgb_top,              //< Значение, до которого отсчитывается счетчик генератора импульсов.
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,   //< Режим загрузки данных последовательности из оперативной памяти.
        .step_mode    = NRF_PWM_STEP_AUTO          //< Режим продвижения активной последовательности.
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm1, &config1, LED_RGB_handler));

    m_led_rgb_seq_values.channel_0 = 0;
    m_led_rgb_seq_values.channel_1 = 0;
    m_led_rgb_seq_values.channel_2 = 0;
    m_led_rgb_seq_values.channel_3 = 0;
    m_led_rgb_phase                = 1;

    (void)nrf_drv_pwm_simple_playback(&m_pwm1, &m_led_rgb_seq, 1, NRF_DRV_PWM_FLAG_LOOP);
}

void button_zero_out(void * p_context)
{
    count_button = 0;
    timer_end = true;
}

void timer_init(void)
{
    ret_code_t err_code;
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    err_code = app_timer_create(&button_timer, APP_TIMER_MODE_SINGLE_SHOT, button_zero_out);
    APP_ERROR_CHECK(err_code);
}

void timer_start(void)
{
    uint32_t err_code = app_timer_start(button_timer, APP_TIMER_TICKS(1000), NULL);//таймер на 1 секунду
    APP_ERROR_CHECK(err_code);
}

void LED_RGB_handler(nrf_drv_pwm_evt_type_t event_type)
{
    if (event_type == NRF_DRV_PWM_EVT_FINISHED)
    {
        uint16_t value_1 = m_led_rgb_seq_values.channel_1;
        uint16_t value_2 = m_led_rgb_seq_values.channel_2;
        uint16_t value_3 = m_led_rgb_seq_values.channel_3;
        if(flag_condition == 5)
        {
            value_1 = flash_value_1;
            value_2 = flash_value_2;
            value_3 = flash_value_3;
        }
        if(flag_condition == 1 && state_button)
        {   
            if (phase != 0)
            {
                value_1 = save_value_1;
                value_2 = save_value_2;
                value_3 = save_value_3;
            }


            if (value_1 == OFF && value_2 == OFF && value_3 == OFF)
                value_1 = MAX_BRIGHT;
            else if (value_1 == MAX_BRIGHT && value_2 == OFF && value_3 == OFF)
                phase=1;
            else if (value_1 == MAX_BRIGHT && value_2 == MAX_BRIGHT && value_3 == OFF)
                phase=2;
            else if (value_1 == OFF && value_2 == MAX_BRIGHT && value_3 == OFF)
                phase=3;
            else if (value_1 == OFF && value_2 == MAX_BRIGHT && value_3 == MAX_BRIGHT)
                phase=4;
            else if (value_1 == OFF && value_2 == OFF && value_3 == MAX_BRIGHT)
                phase=5;
            else if (value_1 == MAX_BRIGHT && value_2 == OFF && value_3 == MAX_BRIGHT)
                phase=6;

            if(phase == 1)
            {   
                value_1 =  MAX_BRIGHT;
                if ((value_2 + BRIGHT_STEP) >= MAX_BRIGHT)
                    value_2 = MAX_BRIGHT;
                else
                    value_2 += BRIGHT_STEP;
                value_3 =  OFF;
            }                    
            else if(phase == 2)
            {
                if ((value_1 - BRIGHT_STEP) <= MIN_BRIGHT)
                    value_1 = MIN_BRIGHT;
                else
                    value_1 -= BRIGHT_STEP;
                value_2 =  MAX_BRIGHT;
                value_3 =  OFF;
            }
            else if(phase == 3)
            {
                value_1 =  OFF;
                value_2 =  MAX_BRIGHT;
                if ((value_3 + BRIGHT_STEP) >= MAX_BRIGHT)
                    value_3 = MAX_BRIGHT;
                else
                    value_3 += BRIGHT_STEP;
            }
            else if(phase == 4)
            {
                value_1 =  OFF;
                if ((value_2 - BRIGHT_STEP) <= MIN_BRIGHT)
                    value_2 = MIN_BRIGHT;
                else
                    value_2 -= BRIGHT_STEP;
                value_3 =  MAX_BRIGHT;
            }
            else if(phase == 5)
            {
                if ((value_1 + BRIGHT_STEP) >= MAX_BRIGHT)
                    value_1 = MAX_BRIGHT;
                else
                    value_1 += BRIGHT_STEP;
                value_2 =  OFF;
                value_3 =  MAX_BRIGHT;
            }
            else if(phase == 6)
            {
                value_1 =  MAX_BRIGHT;
                value_2 =  OFF;
                if ((value_3 - BRIGHT_STEP) <= MIN_BRIGHT)
                    value_3 = MIN_BRIGHT;
                else
                    value_3 -= BRIGHT_STEP;
            }

            save_value_1 = value_1;
            save_value_2 = value_2;
            save_value_3 = value_3;
        }
        else if (flag_condition == 2 && state_button)
        {   
            if ((value_1 + BRIGHT_STEP) >= MAX_BRIGHT)
                value_1 = MAX_BRIGHT;
            else
                value_1 += BRIGHT_STEP;

            if ((value_2 + BRIGHT_STEP) >= MAX_BRIGHT)
                value_2 = MAX_BRIGHT;
            else
                value_2 += BRIGHT_STEP;

            if ((value_3 + BRIGHT_STEP) >= MAX_BRIGHT)
                value_3 = MAX_BRIGHT;
            else
                value_3 += BRIGHT_STEP;

            if((value_1 == MAX_BRIGHT) && (value_2 == MAX_BRIGHT) && (value_3 == MAX_BRIGHT))
            {
                value_1 = begin_value_1;
                value_2 = begin_value_2;
                value_3 = begin_value_3;
            }

        }
        else if (flag_condition == 3 && state_button)
        {
            if ((value_1 - BRIGHT_STEP) <= MIN_BRIGHT)
                value_1 = MIN_BRIGHT;
            else
                value_1 -= BRIGHT_STEP;

            if ((value_2 - BRIGHT_STEP) <= MIN_BRIGHT)
                value_2 = MIN_BRIGHT;
            else
                value_2 -= BRIGHT_STEP;

            if ((value_3 - BRIGHT_STEP) <= MIN_BRIGHT)
                value_3 = MIN_BRIGHT;
            else
                value_3 -= BRIGHT_STEP;

            if((value_1 == MIN_BRIGHT) && (value_2 == MIN_BRIGHT) && (value_3 == MIN_BRIGHT))
            {
                value_1 = second_value_1;
                value_2 = second_value_2;
                value_3 = second_value_3;
            }
            
            flash_value_1 = value_1;
            flash_value_2 = value_2;
            flash_value_3 = value_3;

        }
        m_led_rgb_seq_values.channel_1 = value_1;
        m_led_rgb_seq_values.channel_2 = value_2;
        m_led_rgb_seq_values.channel_3 = value_3;
    }
}

void button_pressed(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{   
    count_button++;
    if(count_button == 4)
    {
        // double_click = (double_click ? 0 : 1);
        count_button = 0;
        flag_condition++;//переходим в следующие состояние

        if(flag_condition == 1) // включаемплавное изменение цветов
        {   
            m_led_0_step = 500;
        }
        else if(flag_condition == 2) // выбираем насыщенность
        {
            begin_value_1 = m_led_rgb_seq_values.channel_1;
            begin_value_2 = m_led_rgb_seq_values.channel_2;
            begin_value_3 = m_led_rgb_seq_values.channel_3;
            m_led_0_step = 1500;
        }
        else if(flag_condition == 3) // выбираем яркость
        {
            second_value_1 = m_led_rgb_seq_values.channel_1;
            second_value_2 = m_led_rgb_seq_values.channel_2;
            second_value_3 = m_led_rgb_seq_values.channel_3;
            m_led_0_step = MAX_BRIGHT;
        }
        else if(flag_condition >= 4) // переходим в начальное состояние
        {
            flag_condition = 0;
            m_led_0_step = 0;
            flash_write();
        }
    }

    if(timer_end)
    {
        timer_start();//ждём второе нажатие кнопки
        timer_end = false;
    }

    state_button  = (state_button  ? false : true);
}