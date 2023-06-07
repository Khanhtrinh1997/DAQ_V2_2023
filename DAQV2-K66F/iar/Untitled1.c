
//========================================== ISENSEInfo Function ==========================================//
error_t privateMibSetISENSEGroup(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibISENSEGroup *entry;
    
    //	Point to the liBattGroup entry
    entry = &privateMibBase.isenseGroup;
    
    if(!strcmp(object->name, "isenseInstalledISENSE"))
    {
      if((value->integer >= 0) && (value->integer <= 1))
      {
          //Get object value
          entry->isenseInstalledISENSE = value->integer; 
          if (entry->isenseInstalledISENSE != sModbusManager.u8NumberOfISENSE)
          {
              sModbusManager.u8NumberOfISENSE = entry->isenseInstalledISENSE;
              g_sParameters.u8DevNum[TOTAL_ISENSE_INDEX] = sModbusManager.u8NumberOfISENSE;
              g_sWorkingDefaultParameters.u8DevNum[TOTAL_ISENSE_INDEX] = g_sParameters.u8DevNum[TOTAL_ISENSE_INDEX];
              u8SaveConfigFlag |= 1;
          }
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    else
    {
        TRACE_ERROR("ERROR_OBJECT_NOT_FOUND!\r\n");
        //The specified object does not exist
        return ERROR_OBJECT_NOT_FOUND;
    }
    
    //Successful processing
    return NO_ERROR; 
}

error_t privateMibSetISENSEInfoEntry(const MibObject *object, const uint8_t *oid,
                                   size_t oidLen, const MibVariant *value, size_t valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    uint_t j;
    tModbusParameters sModbusDev[3];
    PrivateMibISENSEInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 2)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the libTable entry
    entry = &privateMibBase.isenseGroup.isenseTable[index - 1];
    //isenseType object?
    if(!strcmp(object->name, "isenseType"))
    {
      if((value->integer >= 0) && (value->integer <= 15))
      {
        //Set object value
        entry->isenseType= value->integer;
        j = index;
        sModbusManager.sISENSEManager[j-1].u8ISENSEType = entry->isenseType;
        
        switch(sModbusManager.sISENSEManager[j-1].u8ISENSEType)
        {
        case 1:// FORLONG
        {  
            sModbusManager.sISENSEManager[j-1].u32ISENSESlaveOffset = 26;  
            sModbusManager.sISENSEManager[j-1].u32ISENSEAbsSlaveID = 26+j;
            sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[0] = 0;
            sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[0] = 20;                                
        }
        break;
        };
        sModbusDev[j-1].u8DevVendor = sModbusManager.sISENSEManager[j-1].u8ISENSEType;
        sModbusDev[j-1].u32SlaveID = sModbusManager.sISENSEManager[j-1].u32ISENSEAbsSlaveID;  
        //
        // Did parameters change?
        //
        if  (g_sParameters.sModbusParameters[j-1+ MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor != sModbusDev[j-1].u8DevVendor)
        {
          //
          // Update the current parameters with the new settings.
          //
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor = sModbusDev[j-1].u8DevVendor;
          g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u32SlaveID = sModbusDev[j-1].u32SlaveID;
          //
          // Yes - save these settings as the defaults.
          //
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor;
          g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM + MAX_NUM_OF_VAC + MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u32SlaveID;
           
          u8SaveConfigFlag |= 1;
        }      
        
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // isenseDeltaFreqDisConnect
    else if(!strcmp(object->name, "isenseDeltaFreqDisConnect"))
    {
      if((value->integer >= 1) && (value->integer <= 10) && (value->integer > entry->isenseDeltaFreqReConnect ))
      {
        //Set object value
        entry->isenseDeltaFreqDisConnect= value->integer;
        g_sParameters.u8DeltaFreqDisConnect2[index - 1] = (uint8_t)value->integer;
        g_sWorkingDefaultParameters.u8DeltaFreqDisConnect2[index - 1] = g_sParameters.u8DeltaFreqDisConnect2[index - 1];
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // isenseDeltaFreqReConnect
    else if(!strcmp(object->name, "isenseDeltaFreqReConnect"))
    {
      if((value->integer >= 1) && (value->integer <= 10) && (value->integer < entry->isenseDeltaFreqDisConnect ))
      {
        //Set object value
        entry->isenseDeltaFreqReConnect= value->integer;
        g_sParameters.u8DeltaFreqReConnect2[index - 1] = (uint8_t)value->integer;
        g_sWorkingDefaultParameters.u8DeltaFreqReConnect2 [index - 1] = g_sParameters.u8DeltaFreqReConnect2[index - 1];
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    // isenseEnableFreqTrap
    else if(!strcmp(object->name, "isenseEnableFreqTrap"))
    {
      if((value->integer == 0) || (value->integer == 1))
      {
        //Set object value
        privateMibBase.isenseGroup.isenseTable.isenseEnableFreqTrap = value->integer;
        g_sParameters.u8EnableFreqTrap2 = (uint8_t)value->integer;
        g_sWorkingDefaultParameters.u8EnableFreqTrap = g_sParameters.u8EnableFreqTrap2;
        u8SaveConfigFlag |= 1;
      }
      else
      {
        return ERROR_PARAMETER_OUT_OF_RANGE;
      }
    }
    //Successful processing
    return NO_ERROR;

}
/**
 * @brief Get ISENSEInfoEntry object value
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier (object name and instance identifier)
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] value Object value
 * @param[in,out] valueLen Length of the object value, in bytes
 * @return Error code
 **/

error_t privateMibGetISENSEInfoEntry(const MibObject *object, const uint8_t *oid,
                                  size_t oidLen, MibVariant *value, size_t *valueLen)
{
    error_t error;
    size_t n;
    uint_t index;
    PrivateMibISENSEInfoEntry *entry;

    //Point to the instance identifier
    n = object->oidLen;

    //The ifIndex is used as instance identifier
    error = mibDecodeIndex(oid, oidLen, &n, &index);
    //Invalid instance identifier?
    if(error) return error;

    //Sanity check
    if(n != oidLen)
        return ERROR_INSTANCE_NOT_FOUND;

    //Check index range
    if(index < 1 || index > 10)
        return ERROR_INSTANCE_NOT_FOUND;

    //Point to the interface table entry
    entry = &privateMibBase.isenseGroup.isenseTable[index - 1];
    //isenseID object?
    if(!strcmp(object->name, "isenseID"))
    {
        //Get object value
        value->integer = entry->isenseID;
    }
    //isenseStatus object?
    else if(!strcmp(object->name, "isenseStatus"))
    {
        //Get object value
        value->integer = entry->isenseStatus;
    }
    //isenseImportActiveEnergy object?
    else if(!strcmp(object->name, "isenseImportActiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseImportActiveEnergy;
    }
    //isenseExportActiveEnergy object?
    else if(!strcmp(object->name, "isenseExportActiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseExportActiveEnergy;
    }
    //isenseImportReactiveEnergy object?
    else if(!strcmp(object->name, "isenseImportReactiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseImportReactiveEnergy;
    }
    //isenseExportReactiveEnergy object?
    else if(!strcmp(object->name, "isenseExportReactiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseExportReactiveEnergy;
    }
    //isenseTotalActiveEnergy object?
    else if(!strcmp(object->name, "isenseTotalActiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseTotalActiveEnergy;
    }
    //isenseTotalReactiveEnergy object?
    else if(!strcmp(object->name, "isenseTotalReactiveEnergy"))
    {
        //Get object value
        value->integer = entry->isenseTotalReactiveEnergy;
    }
    //isenseActivePower object?
    else if(!strcmp(object->name, "isenseActivePower"))
    {
        //Get object value
        value->integer = entry->isenseActivePower;
    }
    //isenseReactivePower object?
    else if(!strcmp(object->name, "isenseReactivePower"))
    {
        //Get object value
        value->integer = entry->isenseReactivePower;
    }
    //isensePowerFactor object?
    else if(!strcmp(object->name, "isensePowerFactor"))
    {
        //Get object value
        value->integer = entry->isensePowerFactor;
    }
    //isenseFrequency object?
    else if(!strcmp(object->name, "isenseFrequency"))
    {
        //Get object value
        value->integer = entry->isenseFrequency;
    }
    //isenseCurrent object?
    else if(!strcmp(object->name, "isenseCurrent"))
    {
        //Get object value
        value->integer = entry->isenseCurrent;
    }
    //isenseVoltage object?
    else if(!strcmp(object->name, "isenseVoltage"))
    {
        //Get object value
        value->integer = entry->isenseVoltage;
    }
    //isenseRealPower object?
    else if(!strcmp(object->name, "isenseRealPower"))
    {
        //Get object value
        value->integer = entry->isenseRealPower;
    }
    //isenseApparentPower object?
    else if(!strcmp(object->name, "isenseApparentPower"))
    {
        //Get object value
        value->integer = entry->isenseApparentPower;
    }
    //isenseType object?
    else if(!strcmp(object->name, "isenseType"))
    {
        //Get object value
        value->integer = entry->isenseType;
    }
    //isenseSerial object?
    else if(!strcmp(object->name, "isenseSerial"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->isenseSerialLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->isenseSerial, entry->isenseSerialLen);
            //Return object length
            *valueLen = entry->isenseSerialLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //isenseModbusID object?
    else if(!strcmp(object->name, "isenseModbusID"))
    {
        //Get object value
        value->integer = entry->isenseModbusID;
    }
    //isenseModel object?
    else if(!strcmp(object->name, "isenseModel"))
    {
        //Make sure the buffer is large enough to hold the entire object
        if(*valueLen >= entry->isenseModelLen)
        {
            //Copy object value
            memcpy(value->octetString, entry->isenseModel, entry->isenseModelLen);
            //Return object length
            *valueLen = entry->isenseModelLen;
        }
        else
        {
            //Report an error
            error = ERROR_BUFFER_OVERFLOW;
        }
    }
    //isenseOutOfRangeFreq object?
    else if(!strcmp(object->name, "isenseOutOfRangeFreq"))
    {
        //Get object value
        value->integer = entry->isenseOutOfRangeFreq;
    }
    //isenseDeltaFreqDisConnect object?
    else if(!strcmp(object->name, "isenseDeltaFreqDisConnect"))
    {
        //Get object value
        value->integer = entry->isenseDeltaFreqDisConnect;
    }
    //isenseDeltaFreqReConnect object?
    else if(!strcmp(object->name, "isenseDeltaFreqReConnect"))
    {
        //Get object value
        value->integer = entry->isenseDeltaFreqReConnect;
    }
    //isenseEnableFreqTrap object?
    else if(!strcmp(object->name, "isenseEnableFreqTrap"))
    {
        //Get object value
        value->integer = entry->isenseEnableFreqTrap;
    }

    //Return status code
    return error;
}


/**
 * @brief Get next ISENSEInfoEntry object
 * @param[in] object Pointer to the MIB object descriptor
 * @param[in] oid Object identifier
 * @param[in] oidLen Length of the OID, in bytes
 * @param[out] nextOid OID of the next object in the MIB
 * @param[out] nextOidLen Length of the next object identifier, in bytes
 * @return Error code
 **/
error_t privateMibGetNextISENSEInfoEntry(const MibObject *object, const uint8_t *oid,
                                      size_t oidLen, uint8_t *nextOid, size_t *nextOidLen)
{
    error_t error;
    size_t n;
    uint_t index;

    //Make sure the buffer is large enough to hold the OID prefix
    if(*nextOidLen < object->oidLen)
        return ERROR_BUFFER_OVERFLOW;

    //Copy OID prefix
    memcpy(nextOid, object->oid, object->oidLen);

    //Loop through network interfaces
    for(index = 1; index <= privateMibBase.isenseGroup.isenseInstalledISENSE; index++)
    {
        //Append the instance identifier to the OID prefix
        n = object->oidLen;

        //The ifIndex is used as instance identifier
        error = mibEncodeIndex(nextOid, *nextOidLen, &n, index);
        //Any error to report?
        if(error) return error;

        //Check whether the resulting object identifier lexicographically
        //follows the specified OID
        if(oidComp(nextOid, n, oid, oidLen) > 0)
        {
            //Save the length of the resulting object identifier
            *nextOidLen = n;
            //Next object found
            return NO_ERROR;
        }
    }

    //The specified OID does not lexicographically precede the name
    //of some object
    return ERROR_OBJECT_NOT_FOUND;
}
//========================================== ISENSEInfo Function ==========================================//
