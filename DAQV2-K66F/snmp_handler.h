#include "Header.h"

#if (USERDEF_CLIENT_SNMP == ENABLED)
error_t snmpAgentRandCallback(uint8_t *data, size_t length);
////void trapsendTest(void);
////void Trap_Send_Type_1(SnmpAgentContext *context, const IpAddr *destIpAddr,
////   SnmpVersion version, const char_t *username, uint_t genericTrapType,
////   uint_t specificTrapCode, const SnmpTrapObject *objectList, uint_t objectListSize ,
////   const char_t *str, uint8_t *oid, size_t maxOidLen, size_t *oidLen,
////   uint32_t* pui32value, uint16_t number, uint8_t alarmVal1, uint8_t alarmVal2, uint8_t normalVal);
////void Trap_Send_Type_2(SnmpAgentContext *context, const IpAddr *destIpAddr,
////   SnmpVersion version, const char_t *username, uint_t genericTrapType,
////   uint_t specificTrapCode, const SnmpTrapObject *objectList, uint_t objectListSize ,
////   uint32_t* pui32value_new, uint32_t* pui32value_old);
void snmp_handler_init(void);

extern void snmp_TrapSend(void);
void trapSendTask(void *param);
#endif