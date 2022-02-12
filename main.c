#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "function.h"

#include "nrf_nvmc.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"
#include "app_usbd_cdc_acm.h"

#define First_page_begin 0xDD000
#define First_page_end 0xDE000                                   

#define READ_SIZE 1
#define CMD_SIZE 15
#define CMD_SIZE_RED 7
#define CMD_SIZE_BLUE 8
#define CMD_SIZE_GREEN 9


uint16_t flash_value_1 = 0;
uint16_t flash_value_2 = 0;
uint16_t flash_value_3 = 0;
uint8_t count_cmd = 0;

static char m_rx_buffer[READ_SIZE];
char cmd[CMD_SIZE];

static void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event);

/* Make sure that they don't intersect with LOG_BACKEND_USB_CDC_ACM */
#define CDC_ACM_COMM_INTERFACE  2
#define CDC_ACM_COMM_EPIN       NRF_DRV_USBD_EPIN3

#define CDC_ACM_DATA_INTERFACE  3
#define CDC_ACM_DATA_EPIN       NRF_DRV_USBD_EPIN4
#define CDC_ACM_DATA_EPOUT      NRF_DRV_USBD_EPOUT4

APP_USBD_CDC_ACM_GLOBAL_DEF(usb_cdc_acm,
                            usb_ev_handler,
                            CDC_ACM_COMM_INTERFACE,
                            CDC_ACM_DATA_INTERFACE,
                            CDC_ACM_COMM_EPIN,
                            CDC_ACM_DATA_EPIN,
                            CDC_ACM_DATA_EPOUT,
                            APP_USBD_CDC_COMM_PROTOCOL_AT_V250);

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

static void prepare_cmd(void)
{
    cmd[count_cmd]=m_rx_buffer[0];
    count_cmd++;
}

static void check_cmd(void)
{
    ret_code_t ret;
    char cmd_red[CMD_SIZE_RED]    = {'R','G','B',' ','r','e','d'};
    char cmd_blue[CMD_SIZE_BLUE]  = {'R','G','B',' ','b','l','u','e'};
    char cmd_green[CMD_SIZE_GREEN]= {'R','G','B',' ','g','r','e','e','n'};

    if(memcmp (cmd, cmd_red, CMD_SIZE_RED) == 0)
    {
        NRF_LOG_INFO("Color set to red");
        bsp_board_led_on(1);
        bsp_board_led_off(2);
        bsp_board_led_off(3);
        do
        {
        ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                "Color set to red",
                                16);
        } while (ret == NRF_SUCCESS);
    }
    else if(memcmp (cmd, cmd_blue, CMD_SIZE_BLUE) == 0)
    {
        NRF_LOG_INFO("Color set to blue");
        bsp_board_led_off(1);
        bsp_board_led_off(2);
        bsp_board_led_on(3);
        do
        {
        ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                "Color set to blue",
                                17);
        } while (ret == NRF_SUCCESS);
    }
    else if(memcmp (cmd, cmd_green, CMD_SIZE_GREEN) == 0)
    {
        NRF_LOG_INFO("Color set to green");
        bsp_board_led_off(1);
        bsp_board_led_on(2);
        bsp_board_led_off(3);
        do
        {      
        ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                "Color set to green",
                                18);
        } while (ret == NRF_SUCCESS);
    }
    else 
    {
        NRF_LOG_INFO("Unknown command");
    }
    
    count_cmd = 0;

    for(int i=0;i<CMD_SIZE;i++)
        cmd[i]=0;//обнуляем команду
}

static void usb_ev_handler(app_usbd_class_inst_t const * p_inst,
                           app_usbd_cdc_acm_user_event_t event)
{
    switch (event)
    {
    case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
    {
        ret_code_t ret;
        bsp_board_led_on(0);
        ret = app_usbd_cdc_acm_read(&usb_cdc_acm, m_rx_buffer, READ_SIZE);
        UNUSED_VARIABLE(ret);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
    {
        bsp_board_led_off(0);
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
    {
        NRF_LOG_INFO("tx done");
        break;
    }
    case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
    {
        ret_code_t ret;
        do
        {
            size_t size = app_usbd_cdc_acm_rx_size(&usb_cdc_acm);
            NRF_LOG_INFO("rx size: %d", size);

            if (m_rx_buffer[0] == '\r' || m_rx_buffer[0] == '\n')
            {
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm, "\r\n", 2);
                if (m_rx_buffer[0] == '\r')
                 check_cmd();
            }
            else
            {
                prepare_cmd();
                ret = app_usbd_cdc_acm_write(&usb_cdc_acm,
                                             m_rx_buffer,
                                             READ_SIZE);
            }

            /* Fetch data until internal buffer is empty */
            ret = app_usbd_cdc_acm_read(&usb_cdc_acm,
                                        m_rx_buffer,
                                        READ_SIZE);
        } while (ret == NRF_SUCCESS);

        break;
    }
    default:
        break;
    }
}

int main(void)
{   
    logs_init();
    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    nrfx_systick_init();
    NRF_LOG_INFO("HELLOW I'am Nordic");
    interrupt_init();
    timer_init();
    //LED_RGB();
    //LED_0();

    //flash_write();
    //flash_read();//читаем значение с флэшки

    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&usb_cdc_acm);
    ret_code_t ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);

    while (true)
    {   
        
        while (app_usbd_event_queue_process())
        {
            /* Nothing to do */
        }

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_FLUSH();
    }
}