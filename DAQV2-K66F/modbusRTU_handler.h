#include "Header.h"
extern uint8_t LibM1Addr;

uint16_t s19FileParse(uint32_t startAddr);
void modbusRTU_task(void *pvParameters);
void UpdateLIBInfo(void);
void UpdateGENInfo(void);

#if (USERDEF_MONITOR_BM == ENABLED)
void UpdateBMInfo (void);
#endif
#if (USERDEF_MONITOR_VAC == ENABLED)
void UpdateVACInfo (void);
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED) //smcb
void UpdateSMCBInfo (void);
#endif
#if (USERDEF_MONITOR_FUEL == ENABLED) //fuel
void UpdateFUELInfo (void);
#endif
#if (USERDEF_MONITOR_PM == ENABLED)
void UpdatePMInfo (void);
#endif
#if (USERDEF_MONITOR_ISENSE == ENABLED) //isense
void UpdateISENSEInfo (void);
#endif
#if (USERDEF_MONITOR_PM_DC == ENABLED)
void Update_PM_DC_Info (void);
#endif
#if (USERDEF_RS485_DKD51_BDP == ENABLED)
void update_fan_dpc_info(void);
#endif
#if (USER_DEF_CHECK_GEN ==  ENABLED)
void check_gen(void);
#endif
void UpdateInfo (void);