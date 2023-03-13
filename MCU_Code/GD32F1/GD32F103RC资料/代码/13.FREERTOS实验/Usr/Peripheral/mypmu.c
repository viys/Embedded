/**************************************************************************
file name:
Description:
**************************************************************************/
//---------------------- include files -----------------------
#include "mypmu.h"


/*****************************************************************************************
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*****************************************************************************************/
void PMU_init(void)
{
    /* enable PMU clock */ 
    rcu_periph_clock_enable(RCU_PMU);
}


/*****************************************************************************************
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*****************************************************************************************/
void mcu_goto_sleep(void)
{
    /* enter sleep mode */
    pmu_to_sleepmode(WFI_CMD);
}


/*****************************************************************************************
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*****************************************************************************************/
void mcu_goto_deepsleep(void)
{
    /* enter sleep mode */
    pmu_to_deepsleepmode(PMU_LDO_LOWPOWER,WFI_CMD);
}


/*****************************************************************************************
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*****************************************************************************************/
void recover_sys_clock(void)
{
	SystemInit();
}
