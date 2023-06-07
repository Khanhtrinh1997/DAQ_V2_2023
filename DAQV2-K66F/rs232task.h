#include "fsl_uart_freertos.h"
#include "fsl_uart.h"
#include "fsl_port.h"


#define EMER 0
#define ZTE 1
#define HW 2
#define EMERV21 3
#define DKD51_BDP 4

#define SET_LLVD				0x00000001
#define SET_BLVD				0x00000002
#define SET_DCLOW				0x00000004
#define SET_BATTSTDCAP			        0x00000008
#define SET_CCL					0x00000010
#define SET_WIT_EN				0x00000020
#define SET_WIT_VAL				0x00000040
#define SET_FLTVOL				0x00000080
#define SET_BSTVOL				0x00000100
#define SET_OVERTEMP			        0x00000200
#define SET_TEMPCOMP_VAL		        0x00000400
#define SET_AC_THRES 			        0x00000800
#define SET_AC_LOW_THRES 		        0x00000800
#define SET_AC_HIGH_THRES 		        0x00001000
#define SET_AC_INPUT_CURR_LIMIT                 0x00002000
#define SET_BATTSTDCAP2 		        0x00004000

#define SET_BT_PLANTEST 		        0x00008000
#define SET_BT_ENDCAP			        0x00010000
#define SET_BT_ENDTIME			        0x00020000
#define SET_BT_ENDVOLT			        0x00040000
#define SET_BT_PLEN				0x00080000
#define SET_BT_SCUTIME			        0x00100000

#define SET_BT_CELLNO                           0x00200000
#define START_BT_TEST                           0x00400000
#define STOP_BT_TEST                            0x00800000

#define SET_TEST_VOLT                           0x01000000
#define SET_AUTO_TEST_DAY                       0x02000000
#define SET_TEST_START_TIME                     0x04000000

#define SET_DCUNDER				0x08000000
#define SET_LLVD_EN				0x10000000
#define SET_BLVD_EN				0x40000000
#define SET_DCOVER				0x10000000
#define SET_BATTSTDCAP3                         0x20000000  //thanhcm add for dpc 
#define SET_BATTSTDCAP4                         0x80000000  //thanhcm3 add for dpc

extern uart_rtos_config_t uart_config;


extern uart_rtos_handle_t handle;
extern struct _uart_handle t_handle;
extern uint32_t PMUConnectCount;
extern uint32_t Recv_Count;
extern uint8_t RecvBuff[300];
extern uint8_t RS2321Buff[300];
extern uint8_t RS2322Buff[300];
extern uint8_t RS232Cfg[200] ;
extern uint32_t RX2321Count,RX2322Count,RS232CfgCount;
extern unsigned char g_setInfo[32];
extern unsigned char g_getInfo[24];
extern void Emerson_Data_Process(void *pvParameters);
extern void EMER_V21_Data_Process(void *pvParameters);
extern void ZTE_Data_Process(void *pvParameters);
extern void RS2321_Send_Data(void *pvParameters);
extern void RS2321_Recv_Data(void *pvParameters);
extern void Agisson_Data_Process(void *pvParameters);
extern void DKD51_BDP_data_process(void *pvParameters);
void Uart1_ISR(void);
extern uint32_t requestcode;
extern uint8_t TYPE;
extern uint8_t g_AlrmMan;
extern uint8_t ui8User[6];
extern unsigned char g_UARTRxBuf[300];
extern unsigned char g_UARTRxBuf2[300];
extern uint32_t RecvCntStart;
extern uint32_t RecvCntStart2;
extern uint8_t val;


