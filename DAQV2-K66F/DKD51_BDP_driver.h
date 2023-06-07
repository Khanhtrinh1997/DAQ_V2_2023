/** Thanhcm3
*** 19/08/2021
*** Driver DKD51-2 Beijing Dynamic Power
**/
//Driver DKD51-2 ===============================================================

#ifndef DKD51_BDP_DRIVER_H_
#define DKD51_BDP_DRIVER_H_

//Include ======================================================================
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//Define========================================================================
#define dir_send_rs485_dc_cab                            GPIO_SetPinsOutput(GPIOD,1u << 1u); GPIO_SetPinsOutput(GPIOD,1u << 0u)
#define dir_receive_rs485_dc_cab                         GPIO_ClearPinsOutput(GPIOD,1u << 1u);GPIO_ClearPinsOutput(GPIOD,1u << 0u)
#define ERR                                              1
#define NO_ERR                                           0
//define len array---------------------------------------------------------------
//for DKD51--------------------------------------------------
#define  DKD51_REQ_time_info_len                        18  
#define  DKD51_REQ_manufacturer_info_len                18
#define  DKD51_REQ_common_para_info_len                 30        // no use  
#define  DKD51_REQ_alarm_level_len                      30        // no use
#define  DKD51_REQ_ac_input_info_len                    20
#define  DKD51_REQ_ac_alarm_len                         20
#define  DKD51_REQ_ac_parameter_len                     18
#define  DKD51_REQ_rect_sts_len                         18
#define  DKD51_REQ_rect_info_len                        18
#define  DKD51_REQ_rect_alarm_len                       18
#define  DKD51_REQ_dc_alarm_len                         20
#define  DKD51_REQ_dc_parameter_len                     18
#define  DKD51_REQ_dc_output_len                        20
#define  DKD51_REQ_env_alarm_len                        18
#define  DKD51_REQ_env_data_len                         18
#define  DKD51_header_ac_setcode_len                    13       
#define  DKD51_header_dc_setcode_len                    13
//for fan---------------------------------------------------
#define  DKD51_FAN_REQ_sys_analog_data_len              18
#define  DKD51_FAN_REQ_switch_input_state_len           18
#define  DKD51_FAN_REQ_warning_status_len               18
#define  DKD51_FAN_REQ_remote_control_len               18
#define  DKD51_FAN_REQ_read_parameter_len               18
#define  DKD51_FAN_REQ_read_equipment_info_len          18
#define  DKD51_header_fan_dpc_setcode_len               13
#define  DKD51_FAN_SET_STARTING_POINT                   0x80
#define  DKD51_FAN_SET_SENSITIVE_POINT                  0x81
#define  DKD51_FAN_SET_HEATING_START_POINT              0x82
#define  DKD51_FAN_SET_HEATING_SENSITIVE_POINT          0x83
#define  DKD51_FAN_SET_H_TEMP_W_POINT                   0x84
#define  DKD51_FAN_SET_L_TEMP_W_POINT                   0x85
// define -------------------------------------------------------------------------
#define  DKD51_AC                                        1
#define  DKD51_DC                                        2
#define  DKD51_FAN                                       3
//---------------------------------------------------------------------------------
#define  DKD51_AC_VOLT_UPPER_LIMIT                      0x80
#define  DKD51_AC_VOLT_LOWER_LIMIT                      0x81    
//---------------------------------------------------------------------------------
#define  DKD51_AC_INPUT_CUR_UPPER_LIMIT 
#define  DKD51_DC_VOLT_UPPER_LIMIT                      0x80
#define  DKD51_DC_VOLT_LOWER_LIMIT                      0x81
#define  DKD51_FLOAT_VOLT                               0x84
#define  DKD51_BOOST_VOLT                               0x85
#define  DKD51_DC_LOW
#define  DKD51_LLVD
#define  DKD51_BLVD
#define  DKD51_CHARGING_FACTOR                          0x82
#define  DKD51_TEMP_COMPENSATION                        0x89
#define  DKD51_BATT_TEMP_OVER
#define  DKD51_BATT_CAP1                                0x8B
#define  DKD51_BATT_CAP2                                0x8C
#define  DKD51_BATT_CAP3                                0x8D
#define  DKD51_BATT_CAP4                                0x8E                           


//Config==============================================================================
//Ban tin TIMEINFO_REQ yeu cau thong tin ve thoi gian---------------------------------
static const unsigned char DKD51_REQ_time_info[DKD51_REQ_time_info_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','0',   '4','D',   '0','0','0','0',                       'F','D','A','1',    '\r'
//0x   7E    32,30     30,31     34,30      34,44       30,30,30,30                            46,44,41,31        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin MANUFACTURERINFOR_REQ yeu cau thong tin nha san xuat------------------------
static const unsigned char DKD51_REQ_manufacturer_info[DKD51_REQ_manufacturer_info_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','0',   '5','1',   '0','0','0','0',                       'F','D','B','3',    '\r'
//0x   7E    32,30     30,31	 34,30      35,31       30,30,30,30                            46,44,42,33        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+	
};
//Ban tin COMMONPARA_REQ yeu cau thong so chung(ko dung)---------------------------------------
static const unsigned char DKD51_REQ_comon_para_info[DKD51_REQ_common_para_info_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','0',   '4','D',   '0','0','0','0',                       '0','0','0','0',    '\r'
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin ALARMLEVEL_REQ yeu cau muc thong tin canh bao (khong dung)-------------------------------
static const unsigned char DKD51_REQ_alarm_level[DKD51_REQ_alarm_level_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','0',   '4','D',   '0','0','0','0',                       '0','0','0','0',    '\r'
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin ACINPUTINFO_REQ yeu cau thong tin dien ap ac dau vao------------------------
static const unsigned char DKD51_REQ_ac_input_info[DKD51_REQ_ac_input_info_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','0',   '4','2',   'E','0','0','2',        'F','F',       'F','D','1','0',    '\r'
//0x   7E     32,30     30,31     34,30      34,32       45,30,30,32            46,46          46,44,31,30        0D	  
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+

};
//Ban tin ACALARMLEVEL_REQ yêu cau thong tin muc canh bao ac dau vao------------------
static const unsigned char DKD51_REQ_ac_alarm[DKD51_REQ_ac_alarm_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','0',   '4','4',   'E','0','0','2',        'F','F',       'F','D','0','E',    '\r'
//0x  7E     32,30     30,31     34,30      34,34      	45,30,30,32            46,46           46,44,30,45        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
	
};
//Ban tin ACPARAMETER_REQ yeu cau thong so ac-----------------------------------------
static const unsigned char DKD51_REQ_ac_parameter[DKD51_REQ_ac_parameter_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','0',   '4','7',   '0','0','0','0',                       'F','D','A','E',    '\r'
//0x  7E     32,30     30,31     34,30      34,37       30,30,30,30                            46,44,41,45        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin RECTSTS_REQ yeu cau thong tin trang thai rect-------------------------------
static const unsigned char DKD51_REQ_rect_sts[DKD51_REQ_rect_sts_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','1',   '4','3',   '0','0','0','0',                       'F','D','B','1',    '\r'
//0x  7E     32,30     30,31     34,31      34,33       30,30,30,30                             46,44,42,31       0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin RECTINFO_REQ yeu cau thong tin dong ap rect---------------------------------
static const unsigned char DKD51_REQ_rect_info[DKD51_REQ_rect_info_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','1',   '4','2',   '0','0','0','0',                       'F','D','B','2',    '\r'
//0x   7E    32,30     30,31     34,31      34,32       30,30,30,30                            46,44,42,32        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin RECTALARM_REQ yeu cau thong tin canh bao rect -------------------------------
static const unsigned char DKD51_REQ_rect_alarm[DKD51_REQ_rect_alarm_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','1',   '4','4',   '0','0','0','0',                       'F','D','B','0',    '\r'
//0x   7E    32,30     30,31     34,31      34,34       30,30,30,30                            46,44,42,30       0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin DCOUTPUT_REQ yeu cau thong tin dc dau ra------------------------------------
static const unsigned char DKD51_REQ_dc_output[DKD51_REQ_dc_output_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','2',   '4','2',   'E','0','0','2',        'F','F',       'F','D','0','E',    '\r'
//0x   7E    32,30     30,31     34,32      34,32       45,30,30,32            46,46           46,44,30,45        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin DCALARM_REQ yeu cau thong tin canh bao dc-----------------------------------
static const unsigned char DKD51_REQ_dc_alarm[DKD51_REQ_dc_alarm_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','2',   '4','4',   'E','0','0','2',        'F','F',       'F','D','0','C',    '\r'
//0x  7E     32,30     30,31     34,32      34,34       45,30,30,32            46,46           46,44,30,43        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin DCPARAMETER yeu cau thong so dc---------------------------------------------
static const unsigned char DKD51_REQ_dc_parameter[DKD51_REQ_dc_parameter_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','2',   '4','7',   '0','0','0','0',                       'F','D','A','C',    '\r'
//0x  7E     32,30     30,31     34,32      34,37       30,30,30,30                            46,44,41,43        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin ENVDATA_REQ yeu cau cac thong so moi truong--------------------------------
static const unsigned char DKD51_REQ_env_data[DKD51_REQ_env_data_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '8','0',   '4','2',   '0','0','0','0',                       'F','D','A','F',    '\r'
//0x  7E     32,30     30,31     38,30      34,32       30,30,30,30                            46,44,41,46        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Ban tin ENVALARM_REQ yeu cau thong tin canh bao moi truong--------------------------
static const unsigned char DKD51_REQ_env_alarm[DKD51_REQ_env_alarm_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '8','0',   '4','4',   '0','0','0','0',                       'F','D','A','D',    '\r'
//0x   7E    32,30     30,31     38,30      34,34       30,30,30,30	                       46,44,41,44        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Header code cho ban tin cai dat thong so AC(de sau)-----------------------------------------
static unsigned char DKD51_header_ac_setcode[DKD51_header_ac_setcode_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','0',   '4','9',   'A','0','0','6'
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Header code cho ban tin cai dat thong so DC(de sau)-----------------------------------------
static unsigned char DKD51_header_dc_setcode[DKD51_header_dc_setcode_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '4','2',   '4','9',   'A','0','0','6'
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};

//for FAN BLACKSHILELD======================================================================================================================
//ban tin yeu cau thong so dieu hoa---------------------------------------------------------------------------------------
static const unsigned char DKD51_FAN_REQ_sys_analog_data[DKD51_FAN_REQ_sys_analog_data_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '6','0',   '4','2',   '0','0','0','0',                       'F','D','B','1',    '\r'
//0x   7E    32,30     30,31     36,30      34,32       30,30,30,30	                       46,44,42,31        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//ban tin yeu cau state dieu hoa------------------------------------------------------------------------------------------
static const unsigned char DKD51_FAN_REQ_switch_input_state[DKD51_FAN_REQ_switch_input_state_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '6','0',   '4','3',   '0','0','0','0',                       'F','D','B','0',    '\r'
//0x   7E    32,30     30,31     36,30      34,33       30,30,30,30	                       46,44,42,30        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//ban tin warning status dieu hoa-----------------------------------------------------------------------------------------
static const unsigned char DKD51_FAN_REQ_warning_status[DKD51_FAN_REQ_warning_status_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '6','0',   '4','4',   '0','0','0','0',                       'F','D','A','F',    '\r'
//0x   7E    32,30     30,31     36,30      34,34       30,30,30,30	                       46,44,41,46        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//ban tin remote control dieu hoa-----------------------------------------------------------------------------------------
static const unsigned char DKD51_FAN_REQ_remote_control[DKD51_FAN_REQ_remote_control_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '6','0',   '4','5',   '0','0','0','0',                       'F','D','A','E',    '\r'
//0x   7E    32,30     30,31     36,30      34,35       30,30,30,30	                       46,44,41,45        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//ban tin read parameter dieu hoa----------------------------------------------------------------------------------------
static const unsigned char DKD51_FAN_REQ_read_parameter[DKD51_FAN_REQ_read_parameter_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '6','0',   '4','7',   '0','0','0','0',                       'F','D','A','C',    '\r'
//0x   7E    32,30     30,31     36,30      34,37       30,30,30,30	                       46,44,41,43        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//ban tin read equipment info--------------------------------------------------------------------------------------------
static const unsigned char DKD51_FAN_REQ_read_equipment_info[DKD51_FAN_REQ_read_equipment_info_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '6','0',   '5','1',   '0','0','0','0',                       'F','D','B','1',    '\r'
//0x   7E    32,30     30,31     36,30      35,31       30,30,30,30	                       46,44,42,31        0D
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//ban tin cai dat thong so FAN DPC----------------------------------------------------------------------------------------
static unsigned char DKD51_header_fan_dpc_setcode[DKD51_header_fan_dpc_setcode_len]={
//--+-SOI-+---VER---+---ADR---+---CID1---+---CID2---+------LENGTH------+------LEID/2------+------CHKSUM-------+--EOI--+
//--|	  |         |         |          |          |                  |                  |                   |       |
      '~',  '2','0',  '0','1',  '6','0',   '4','9',   'A','0','0','6'
//--+-----+---------+---------+----------+----------+------------------+------------------+-------------------+-------+
};
//Header code cho ban tin cai dat thong so rect
//FUNCTION=============================================================================
extern uart_config_t DKD51_config;
void DKD51_init_rs485_uart(void);
void DKD51_set_command(unsigned char *arrCmd,uint16_t templateCmd,uint16_t setNum, unsigned char type);
//void DKD51_set_command(unsigned char *arr_cmd,uint16_t template_cmd,uint16_t set_num,unsigned char type);
void DKD51_set_command_1(unsigned char *arrCmd,uint16_t templateCmd,int16_t setNum, unsigned char type);

void DKD51_set_ac_low();
void DKD51_set_ac_high();
void DKD51_set_dc_over();
void DKD51_set_dc_low();
void DKD51_set_float_volt();
void DKD51_set_boost_volt();
void DKD51_set_charge_current_limit();
void DKD51_set_temp_compensation();
void DKD51_set_batt_capacity_1();
void DKD51_set_batt_capacity_2();
void DKD51_set_batt_capacity_3();
void DKD51_set_batt_capacity_4();
void DKD51_set_ac_input_curr_limit();
void DKD51_send_message_resquest(uint8_t * buff_request,uint8_t len_buff_request);
void DKD51_send_message_resquest_fan(uint8_t * buff_request,uint8_t len_buff_request);
uint8_t DKD51_check_sum(DPC_T* p);
//uint8_t DKD51_check_sum_fan(FAN_DPC_SWITCH_UART_T* p);
uint8_t DKD51_check_RTN(DPC_T* p);
void DKD51_clearn_buff(DPC_T* p);
void DKD51_delay(void);
void DKD51_set_fan_starting_point(uint16_t temp);
void DKD51_set_fan_sensitive_point(uint16_t temp);
void DKD51_set_fan_H_temp_W_point(uint16_t temp);
void DKD51_set_fan_L_temp_W_point(int16_t temp);
void DKD51_set_fan_heating_start_point(int16_t temp);
void DKD51_set_fan_heating_sensitive_point(int16_t temp);
#endif


