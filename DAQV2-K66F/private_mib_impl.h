/**
 * @file private_mib_impl.h
 * @brief Private MIB module implementation
 *
 * @section License
 *
 * ^^(^____^)^^
 **/

#ifndef _PRIVATE_MIB_IMPL_H
#define _PRIVATE_MIB_IMPL_H

//Dependencies
#include "mibs/mib_common.h"
#include "snmp/snmp_agent.h"

//Private MIB related functions
error_t privateMibInit(void);
void privateMibLock(void);
void privateMibUnlock(void);
//void UpdateInfo (void);
void Alarm_Control(void);
void Relay_Output(void);


error_t privateMibGetCurrentTime(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibSetStringEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibSetLedEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetLedEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextLedEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);

//=============================================== Main Function ============================================//

error_t privateMibSetSiteInfoGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetSiteInfoGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetRectInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextRectInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);

error_t privateMibGetAcPhaseEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextAcPhaseEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);

error_t privateMibGetBatteryGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetLoadGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetCntGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibSetCntGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

#if (USERDEF_MONITOR_BM == ENABLED)
//=============================================BM Info Group =============================================//
error_t privateMibSetBMInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetBMInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextBMInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);
//=============================================BM Info Group =============================================//
#endif
//=============================================LIB Info Group =============================================//
error_t privateMibSetLIBattGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibSetLIBattInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetLIBattInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextLIBattInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);

//=============================================GEN Info Group =============================================//
error_t privateMibSetGenGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibSetGenInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetGenInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextGenInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);

//=============================================PM Info Group =============================================//
error_t privateMibSetPMGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibSetPMInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetPMInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextPMInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);

//=============================================SMCB Info Group =============================================//
error_t privateMibSetSmcbInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibSetSMCBGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetSmcbInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextSmcbInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);
//=============================================FUEL Info Group =============================================//
error_t privateMibSetFuelInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibSetFUELGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetFuelInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextFuelInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);
//=============================================VAC Info Group =============================================//
error_t privateMibSetVACGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibSetVACInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetVACInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextVACInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);

error_t privateMibSetCfgBTGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetCfgBTGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibSetCfgBTPlanTestEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetCfgBTPlanTestEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextCfgBTPlanTestEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);

error_t privateMibSetCfgBTSCUEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetCfgBTSCUEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextCfgBTSCUEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);

error_t privateMibSetConfigGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetConfigGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetMainAlarmGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetDIAlarmGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetConnAlarmGroup(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);
//=============================================ISENSE Info Group =============================================//
error_t privateMibSetISENSEGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibSetISENSEInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

error_t privateMibGetISENSEInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, MibVariant *value, size_t *valueLen);

error_t privateMibGetNextISENSEInfoEntry(const MibObject *object, const uint8_t *oid,
   size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);
//pm dc info group ==============================================================================================
error_t privateMibSet_pm_dc_Group(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);
error_t privateMibSet_pm_dc_InfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);
error_t privateMibGet_pm_dc_InfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen);
error_t privateMibGetNext_pm_dc_InfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen);
//fan dpc info group ===========================================================================================
error_t privateMibGet_fan_dpc_Group(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, MibVariant *value, size_t *valueLen);
error_t privateMibSet_fan_dpc_Group(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen);

#endif
