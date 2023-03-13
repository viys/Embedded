/***************************************************************************//**
  文件: main.c
  作者: Zhengyu https://gzwelink.taobao.com
  版本: V1.0.0
  时间: 20210401
	平台:MINI-GD32F103RCT6

*******************************************************************************/
#include "gd32f10x.h"
#include "gd32f10x_libopt.h"
#include "systick.h"
#include "core_cm3.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"
#include "croutine.h"
void delay(int time)
{
    while(time--);
    
    return;
}
//配置systick 1ms
void systick_config(void)
{
    /* setup systick timer for 1ms interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}
//LED熄灭
void vTaskLED(void *pvParameters)
{
    while (1)
    {
        /* LED OFF */
        gpio_bit_set(GPIOB, GPIO_PIN_4);
        vTaskDelay(150/ portTICK_RATE_MS);
    }
}
//LED点亮
void vTaskLEDFlash(void *pvParameters)
{
    while (1)
    {
         /* LED ON */
        gpio_bit_reset(GPIOB, GPIO_PIN_4);
        vTaskDelay(100/ portTICK_RATE_MS);
    }
}
/*********************************************************************************************
    \brief      toggle the led every 500ms
    \param[in]  none
    \param[out] none
    \retval     none
*********************************************************************************************/
int main(void)
{
   rcu_ahb_clock_config(RCU_AHB_CKSYS_DIV1);
	systick_config();//配置系统主频108M，外部8M晶振
	rcu_periph_clock_enable(RCU_AF); 

	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);//PB4管脚重映射
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);//初始化PB4口
	  xTaskCreate(vTaskLED, "Task LED", 10, NULL, 1, NULL); //创建任务   
    xTaskCreate(vTaskLEDFlash, "Task LED Flash", 10, NULL, 2, NULL);//创建任务
    vTaskStartScheduler();//启动任务
	while(1)
	{
		
	}
	
  
}

/*********************************************************************************************
    \brief      retarget the C library printf function to the USART
    \param[in]  none
    \param[out] none
    \retval     none
*********************************************************************************************/
//int fputc(int ch, FILE *f)
//{
//    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
//    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));

//    return ch;
//}
