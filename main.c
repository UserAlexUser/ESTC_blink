#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "function.h"

#include "nrf_nvmc.h"

#define First_page_begin 0xDD000
#define First_page_end 0xDE000                                   

uint16_t flash_value_1 = 0;
uint16_t flash_value_2 = 0;
uint16_t flash_value_3 = 0;

void flash_write(void)
{
    // uint32_t value = 0;
    // uint32_t count_page = 0;

    // for (count_page = First_page_begin; (count_page + 8) < First_page_end; count_page+=4)
    // {
    //     value = *(uint32_t*)count_page;
    //     if(value == 0xffffffff)
    //     {
    //         nrf_nvmc_write_word((count_page), MAX_BRIGHT);// m_led_rgb_seq_values.channel_1);//пишем значение
    //         nrf_nvmc_write_word((count_page + 4), MAX_BRIGHT);//m_led_rgb_seq_values.channel_2);//пишем значение
    //         nrf_nvmc_write_word((count_page + 8), MAX_BRIGHT);//m_led_rgb_seq_values.channel_3);//пишем значение
    //         break;
    //     }
    //     if((count_page + 12) >= First_page_end)
    //     {
            // nrf_nvmc_page_erase(First_page_begin);
            // nrf_nvmc_write_word((First_page_begin), MAX_BRIGHT);//m_led_rgb_seq_values.channel_1);//пишем значение
            // nrf_nvmc_write_word((First_page_begin + 4), 0);//m_led_rgb_seq_values.channel_2);//пишем значение
            // nrf_nvmc_write_word((First_page_begin + 8), MAX_BRIGHT);//m_led_rgb_seq_values.channel_3);//пишем значение
    //     }
    // }
    nrf_nvmc_page_erase(First_page_begin);
    nrf_nvmc_write_word((First_page_begin),     flash_value_1);
    nrf_nvmc_write_word((First_page_begin + 4), flash_value_2);
    nrf_nvmc_write_word((First_page_begin + 8), flash_value_3);
    nrf_nvmc_write_word((First_page_begin + 12), phase);
}

void flash_read(void)
{
    // uint32_t value = 0;
    // uint32_t count_page = 0;
    // for (count_page = First_page_begin; (count_page + 16) < First_page_end; count_page++)
    // {
    //     value = *(uint32_t*)count_page;
    //     if((value == 0xffffffff) && (count_page >= First_page_begin + 16))
    //     {
    //         flash_value_1 = *(uint32_t*)(count_page - 4);//читаем 1 значение
    //         flash_value_2 = *(uint32_t*)(count_page - 8);//читаем 2 значение
    //         flash_value_3 = *(uint32_t*)(count_page - 12);//читаем 3 значение
    //         phase         = *(uint32_t*)(count_page - 16);//читаем фазу
    //         break;
    //     }
    //     else if ((value == 0xffffffff) && (count_page <= First_page_begin + 12))
    //     {
    //         flash_value_1 = MAX_BRIGHT;
    //         flash_value_2 = MAX_BRIGHT;
    //         flash_value_3 = 0;
    //         phase         = 1;
    //         break;
    //     }
    // }
    flash_value_1 = *(uint32_t*)(First_page_begin);//читаем 1 значение
    flash_value_2 = *(uint32_t*)(First_page_begin + 4);//читаем 2 значение
    flash_value_3 = *(uint32_t*)(First_page_begin + 8);//читаем 3 значение
    phase         = *(uint32_t*)(First_page_begin + 12);//читаем 3 значение        
}

int main(void)
{   
    logs_init();
    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    nrfx_systick_init();
    NRF_LOG_INFO("HELLOW I'am Nordic");
    interrupt_init();
    timer_init();
    LED_RGB();
    LED_0();

    //flash_write();
    flash_read();//читаем значение с флэшки

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