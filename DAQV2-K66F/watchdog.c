#include "watchdog.h"


/*!
 * @brief Application entry point.
 */

static WDOG_Type *wdog_base = WDOG;
static RCM_Type *rcm_base = RCM;
uint16_t wdog_reset_count = 0;
  
static void WaitWctClose(WDOG_Type *base)
{
    /* Accessing register by bus clock */
    for (uint32_t i = 0; i < 256; i++)
    {
        (void)base->RSTCNT;
    }
}

void watchdog_Init(void)
{
  uint16_t wdog_reset_count = 0;
  wdog_config_t wdogconfig;
  
  if(!(RCM_GetPreviousResetSources(rcm_base) & kRCM_SourceWdog))
  {
      WDOG_ClearResetCount(wdog_base);
  }
  wdog_reset_count = WDOG_GetResetCount(wdog_base);
  
  WDOG_GetDefaultConfig(&wdogconfig);
  wdogconfig.timeoutValue = 0x2710U;//0x4E20U;
  WDOG_Init(wdog_base, &wdogconfig);
  WaitWctClose(wdog_base);
}

void watchdog_Refresh(void)
{
  WDOG_Refresh(wdog_base);
};