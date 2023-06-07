#ifndef SOURCE_HEADER_H_
#define SOURCE_HEADER_H_

/* Dependencies. */
#include <stdlib.h>
#include "math.h"
#include "mk66f18.h"
#include "frdm_k66f.h"
#include "os_port.h"
#include "core/net.h"
#include "drivers/mk6x_eth.h"
#include "drivers/ksz8081.h"
#include "dhcp/dhcp_client.h"
#include "ipv6/slaac.h"
#include "http/http_server.h"
#include "http/mime.h"
#include "path.h"
#include "date_time.h"
#include "resource_manager.h"
#include "debug.h"
//#include "variables.h"
#include "flash_spi.h"
#include "private_mib_module.h"
#include "snmp_handler.h"
#include "http_handler.h"
#include "config.h"

#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_sdramc.h"
#include "fsl_sdram.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "fsl_uart_freertos.h"
#include "fsl_uart.h"
#include "fsl_port.h"
#include "fsl_wdog.h"
#include "fsl_rcm.h"
#include "fsl_crc.h"

//#include "Uart9bit.h"
#include "variables.h"
#include "Uart9bit.h"


#include "rs232task.h"
#include "rs485task.h"
#include "DriverFunc.h"
#include "ZTEDriver.h"
#include "AgissonDriver.h"
#include "EMERDriver.h"
#include "ZTEHistorylog.h"
#include "DKD51_BDP_driver.h"
#include "VERTIV_M830B_driver.h"

#include "rs485.h"
#include "rs232.h"
#include "watchdog.h"
//#include "ftp_handler.h"
#include "timers.h"

#include "rs485RTU.h"
#include "modbusRTU_handler.h"


//#include "fsl_uart_freertos.h"

#if (USERDEF_CLIENT_SNMP == ENABLED)
#include "snmp/snmp_agent.h"
#include "mibs/mib2_module.h"
#include "mibs/mib2_impl.h"
#include "oid.h"
#include "private_mib_module.h"
#include "private_mib_impl.h"
#endif

#if (USERDEF_CLIENT_FTP == ENABLED)
#include "ftp/ftp_client.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Application configuration. */
#define APP_MAC_ADDR "00-AB-CD-EF-00-65"

#define APP_USE_DHCP DISABLED
#define APP_IPV4_HOST_ADDR "192.168.100.103"
#define APP_IPV4_SUBNET_MASK "255.255.255.0"
#define APP_IPV4_DEFAULT_GATEWAY "192.168.100.1"
#define APP_IPV4_PRIMARY_DNS "8.8.8.8"
#define APP_IPV4_SECONDARY_DNS "8.8.4.4"

#define APP_USE_SLAAC ENABLED
#define APP_IPV6_LINK_LOCAL_ADDR "fe80::65"
#define APP_IPV6_PREFIX "2001:db8::"
#define APP_IPV6_PREFIX_LENGTH 64
#define APP_IPV6_GLOBAL_ADDR "2001:db8::65"
#define APP_IPV6_ROUTER "fe80::1"
#define APP_IPV6_PRIMARY_DNS "2001:4860:4860::8888"
#define APP_IPV6_SECONDARY_DNS "2001:4860:4860::8844"

#define APP_HTTP_MAX_CONNECTIONS 4

#if (USERDEF_CLIENT_SNMP == ENABLED)
#define APP_SNMP_ENTERPRISE_OID "1.3.6.1.4.1.45797.14.20"//"1.3.6.1.4.1.8072.9999.9998"//
#define APP_SNMP_CONTEXT_ENGINE "\x80\x00\x00\x00\x01\x02\x03\x04"
#define APP_SNMP_TRAP_DEST_IP_ADDR "192.168.100.25"//"117.6.55.97"//
#endif

#if (USERDEF_SW_TIMER == ENABLED)
/* The software timer period. */
#define SW_TIMER_PERIOD_MS (1000 / portTICK_PERIOD_MS)
#endif
   
#define RS2321_Send_Priority (configMAX_PRIORITIES - 3)
#define TOTAL_LIB_INDEX 0
#define TOTAL_GEN_INDEX 1
#define TOTAL_BM_INDEX 2
#define TOTAL_PM_INDEX 3
#define TOTAL_VAC_INDEX 4
#define TOTAL_SMCB_INDEX 5
#define TOTAL_FUEL_INDEX 6
#define TOTAL_ISENSE_INDEX 7
#define TOTAL_PM_DC_INDEX  8

#define MAX_NUM_OF_LIB 16
#define MAX_NUM_OF_GEN 2
#define MAX_NUM_OF_BM 16
#define MAX_NUM_OF_PM 2
#define MAX_NUM_OF_VAC 2
#define MAX_NUM_OF_SMCB 5
#define MAX_NUM_OF_FUEL 2
#define MAX_NUM_OF_ISENSE 1
#define MAX_NUM_OF_PM_DC  1

#endif // SOURCE_HEADER_H_

