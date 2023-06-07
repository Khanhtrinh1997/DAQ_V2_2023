#include "http_handler.h"
#include "Header.h"
#include "ftp_handler.h"

//#define MAX_NUM_OF_LIB 16
//#define MAX_NUM_OF_GEN 2
//#define MAX_NUM_OF_BM 16
//#define MAX_NUM_OF_PM 2
//#define MAX_NUM_OF_VAC 2
//
//#define TOTAL_LIB_INDEX 0
//#define TOTAL_GEN_INDEX 1
//#define TOTAL_BM_INDEX 2
//#define TOTAL_PM_INDEX 3
//#define TOTAL_VAC_INDEX 4

#if (USERDEF_SERVER_HTTP == ENABLED)
HttpServerSettings httpServerSettings;
HttpServerContext httpServerContext;
HttpConnection httpConnections[APP_HTTP_MAX_CONNECTIONS];
uint16_t u16TextBufferLen;
char g_cSampleTextBuffer[16] = {0};
tConfigParameters sIpConfig;
const char *ucdata;
uint8_t htmlBuffer[24];
char fs_pbuf[16000];
bool bConfirmed[12];
tConfigParameters sConfigParams1;
uint32_t thaocheck=0;
#endif



extern tConfigHandler configHandle;
#if (USERDEF_SERVER_HTTP == ENABLED)
/**
 * @brief CGI callback function
 * @param[in] connection Handle referencing a client connection
 * @param[in] param NULL-terminated string that contains the CGI parameter
 * @return Error code
 **/

error_t httpServerCgiCallback(HttpConnection *connection,
                              const char_t *param)
{
    static uint_t pageCounter = 0;
    uint_t length;
    MacAddr macAddr;
#if (IPV4_SUPPORT == ENABLED)
    Ipv4Addr ipv4Addr;
#endif
#if (IPV6_SUPPORT == ENABLED)
    uint_t n;
    Ipv6Addr ipv6Addr;
#endif

    //Underlying network interface
    NetInterface *interface = connection->socket->interface;

    //Check parameter name
    if(!strcasecmp(param, "PAGE_COUNTER"))
    {
        pageCounter++;
        sprintf(connection->buffer, "%u time%s", pageCounter, (pageCounter >= 2) ? "s" : "");
    }
    else if(!strcasecmp(param, "BOARD_NAME"))
    {
        strcpy(connection->buffer, "FRDM-K66F");
    }
    else if(!strcasecmp(param, "SYSTEM_TIME"))
    {
        systime_t time = osGetSystemTime();
        formatSystemTime(time, connection->buffer);
    }
    else if(!strcasecmp(param, "MAC_ADDR"))
    {
        netGetMacAddr(interface, &macAddr);
        macAddrToString(&macAddr, connection->buffer);
    }
    else if(!strcasecmp(param, "IPV4_ADDR"))
    {
        ipv4GetHostAddr(interface, &ipv4Addr);
        ipv4AddrToString(ipv4Addr, connection->buffer);
    }
    else if(!strcasecmp(param, "SUBNET_MASK"))
    {
        ipv4GetSubnetMask(interface, &ipv4Addr);
        ipv4AddrToString(ipv4Addr, connection->buffer);
    }
    else if(!strcasecmp(param, "DEFAULT_GATEWAY"))
    {
        ipv4GetDefaultGateway(interface, &ipv4Addr);
        ipv4AddrToString(ipv4Addr, connection->buffer);
    }
    else if(!strcasecmp(param, "IPV4_PRIMARY_DNS"))
    {
        ipv4GetDnsServer(interface, 0, &ipv4Addr);
        ipv4AddrToString(ipv4Addr, connection->buffer);
    }
    else if(!strcasecmp(param, "IPV4_SECONDARY_DNS"))
    {
        ipv4GetDnsServer(interface, 1, &ipv4Addr);
        ipv4AddrToString(ipv4Addr, connection->buffer);
    }
#if (IPV6_SUPPORT == ENABLED)
    else if(!strcasecmp(param, "LINK_LOCAL_ADDR"))
    {
        ipv6GetLinkLocalAddr(interface, &ipv6Addr);
        ipv6AddrToString(&ipv6Addr, connection->buffer);
    }
    else if(!strcasecmp(param, "GLOBAL_ADDR"))
    {
        ipv6GetGlobalAddr(interface, 0, &ipv6Addr);
        ipv6AddrToString(&ipv6Addr, connection->buffer);
    }
    else if(!strcasecmp(param, "IPV6_PREFIX"))
    {
        ipv6GetPrefix(interface, 0, &ipv6Addr, &n);
        ipv6AddrToString(&ipv6Addr, connection->buffer);
        length = strlen(connection->buffer);
        sprintf(connection->buffer + length, "/%u", n);
    }
    else if(!strcasecmp(param, "ROUTER"))
    {
        ipv6GetDefaultRouter(interface, 0, &ipv6Addr);
        ipv6AddrToString(&ipv6Addr, connection->buffer);
    }
    else if(!strcasecmp(param, "IPV6_PRIMARY_DNS"))
    {
        ipv6GetDnsServer(interface, 0, &ipv6Addr);
        ipv6AddrToString(&ipv6Addr, connection->buffer);
    }
    else if(!strcasecmp(param, "IPV6_SECONDARY_DNS"))
    {
        ipv6GetDnsServer(interface, 1, &ipv6Addr);
        ipv6AddrToString(&ipv6Addr, connection->buffer);
    }
#endif
    else
    {
        return ERROR_INVALID_TAG;
    }

    //Get the length of the resulting string
    length = strlen(connection->buffer);

    //Send the contents of the specified environment variable
    return httpWriteStream(connection, connection->buffer, length);
}


/**
 * @brief URI not found callback
 * @param[in] connection Handle referencing a client connection
 * @param[in] uri NULL-terminated string containing the path to the requested resource
 * @return Error code
 **/

error_t httpServerUriNotFoundCallback(HttpConnection *connection,
                                      const char_t *uri)
{
    //Not implemented
    return ERROR_NOT_FOUND;

}

char_t  *A;
error_t httpServerRequestCallback(HttpConnection *connection,
                                  const char_t *request)
{
    error_t error;
    uint_t n;
    char_t buffer[500];
    const char *data;
    


    
    if(!strcasecmp(request, "/batt_status"))
    {
        n = sprintf(buffer,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
                    privateMibBase.batteryGroup.battVolt,
                    privateMibBase.batteryGroup.battCurr,
                    privateMibBase.batteryGroup.battTemp,
                    privateMibBase.batteryGroup.battBanksNumofBanks,
                    privateMibBase.batteryGroup.battBrkStatus[0],
                    privateMibBase.batteryGroup.battBrkStatus[1],
                    privateMibBase.batteryGroup.battBrkStatus[2],
                    privateMibBase.batteryGroup.battBrkStatus[3],
                    privateMibBase.batteryGroup.battCapLeft1,
                    privateMibBase.batteryGroup.battCapLeft2,
                    privateMibBase.batteryGroup.battCapLeft3,
                    privateMibBase.batteryGroup.battCapLeft4
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/ac_status"))
    {
        n = sprintf(buffer,"%d;%d;%d",
                    privateMibBase.acPhaseGroup.acPhaseTable[0].acPhaseVolt,
                    privateMibBase.acPhaseGroup.acPhaseTable[1].acPhaseVolt,
                    privateMibBase.acPhaseGroup.acPhaseTable[2].acPhaseVolt
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/load_status"))
    {
        n = sprintf(buffer,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
                    privateMibBase.loadGroup.loadCurrent,
                    privateMibBase.loadGroup.loadStatus[0],
                    privateMibBase.loadGroup.loadStatus[1],
                    privateMibBase.loadGroup.loadStatus[2],
                    privateMibBase.loadGroup.loadStatus[3],
                    privateMibBase.loadGroup.loadStatus[4],
                    privateMibBase.loadGroup.loadStatus[5],
                    privateMibBase.loadGroup.loadStatus[6],
                    privateMibBase.loadGroup.loadStatus[7],
                    privateMibBase.loadGroup.loadStatus[8],
                    privateMibBase.loadGroup.loadStatus[9]
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/pmInfo_status"))
    {
        n = sprintf(buffer,"%d;%d;%d;%d;%s;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
                    privateMibBase.pmGroup.pmTable[0].pmStatus,
                    privateMibBase.pmGroup.pmInstalledPM,
                    privateMibBase.pmGroup.pmActivePM,
                    privateMibBase.pmGroup.pmTable[0].pmType,
                    &privateMibBase.pmGroup.pmTable[0].pmSerial[0],
                    privateMibBase.pmGroup.pmTable[0].pmTotalCurrent,
                    privateMibBase.pmGroup.pmTable[0].pmFrequency,
                    privateMibBase.pmGroup.pmTable[0].pmPowerFactor,
                    privateMibBase.pmGroup.pmTable[0].pmActivePower,
                    privateMibBase.pmGroup.pmTable[0].pmReactivePower,
                    privateMibBase.pmGroup.pmTable[0].pmTotalActiveEnergy,
                    privateMibBase.pmGroup.pmTable[0].pmTotalReactiveEnergy,
                    privateMibBase.pmGroup.pmTable[0].pmPhase1Voltage,//1
                    privateMibBase.pmGroup.pmTable[0].pmPhase1Current,
                    privateMibBase.pmGroup.pmTable[0].pmPhase1PowerFactor,
                    privateMibBase.pmGroup.pmTable[0].pmPhase1RealPower,
                    privateMibBase.pmGroup.pmTable[0].pmPhase1ReactivePower,
                    privateMibBase.pmGroup.pmTable[0].pmPhase2Voltage,//2
                    privateMibBase.pmGroup.pmTable[0].pmPhase2Current,
                    privateMibBase.pmGroup.pmTable[0].pmPhase2PowerFactor,
                    privateMibBase.pmGroup.pmTable[0].pmPhase2RealPower,
                    privateMibBase.pmGroup.pmTable[0].pmPhase2ReactivePower,
                    privateMibBase.pmGroup.pmTable[0].pmPhase3Voltage,//3
                    privateMibBase.pmGroup.pmTable[0].pmPhase3Current,
                    privateMibBase.pmGroup.pmTable[0].pmPhase3PowerFactor,
                    privateMibBase.pmGroup.pmTable[0].pmPhase3RealPower,
                    privateMibBase.pmGroup.pmTable[0].pmPhase3ReactivePower
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/i_senseInfo_status"))
    {
        n = sprintf(buffer,"%d;%d;%d;%s;%s;%d;%d;%d",
                    privateMibBase.isenseGroup.isenseTable.isenseStatus,
                    privateMibBase.isenseGroup.isenseInstalledISENSE,
                    privateMibBase.isenseGroup.isenseActiveISENSE,
                    &privateMibBase.isenseGroup.isenseTable.isenseModel[0],
                    &privateMibBase.isenseGroup.isenseTable.isenseSerial[0],
                    privateMibBase.isenseGroup.isenseTable.isenseVoltage,
                    privateMibBase.isenseGroup.isenseTable.isenseFrequency,
                    privateMibBase.isenseGroup.isenseTable.isenseCurrent
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
        connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/pm_dcInfo_status")){
        n = sprintf(buffer,"%d;%d;%d;%s;%d;%d;%d;%d",
                    privateMibBase.pm_dc_group.pm_dc_table.pm_dc_status,          //0
                    privateMibBase.pm_dc_group.pm_dc_installed_PM_DC,             //1
                    privateMibBase.pm_dc_group.pm_dc_active_PM_DC,                //2
                    &privateMibBase.pm_dc_group.pm_dc_table.pm_dc_model_[0],      //3
                    privateMibBase.pm_dc_group.pm_dc_table.pm_dc_voltage,         //4
                    privateMibBase.pm_dc_group.pm_dc_table.pm_dc_current,         //5
                    privateMibBase.pm_dc_group.pm_dc_table.pm_dc_active_power,    //6
                    privateMibBase.pm_dc_group.pm_dc_table.pm_dc_active_energy    //7
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
        connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
      
    }
    else if(!strcasecmp(request, "/vacInfo_status"))
    {
        n = sprintf(buffer,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%x;%d;%s;%d",
                    privateMibBase.vacGroup.vacTable[0].vacInstalledFan,
                    privateMibBase.vacGroup.vacTable[0].vacActiveFan,
                    privateMibBase.vacGroup.vacTable[0].vacType,
                    privateMibBase.vacGroup.vacTable[0].vacSysMode,
                    privateMibBase.vacGroup.vacTable[0].vacFanSpeedMode,
                    privateMibBase.vacGroup.vacTable[0].vacInTemp,
                    privateMibBase.vacGroup.vacTable[0].vacExTemp,
                    privateMibBase.vacGroup.vacTable[0].vacFan1Duty,
                    privateMibBase.vacGroup.vacTable[0].vacFan1RPM,
                    privateMibBase.vacGroup.vacTable[0].vacAlarmStatus.all,
                    privateMibBase.vacGroup.vacTable[0].vacStatus,  
                    &privateMibBase.vacGroup.vacTable[0].vacSerial[0],
                    privateMibBase.vacGroup.vacTable[0].vacFirmVersion
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/bts_status"))
    {
        n = sprintf(buffer,"%s;%s;%s;%s;%d;%d;%d",
                    privateMibBase.siteGroup.siteBTSCode,
                    "Emerson",
                    "Emerson Ctrler",
                    "DC 48V",
                    privateMibBase.siteGroup.siteAmbientTemp,
                    privateMibBase.siteGroup.siteControllerModel[0],
                    privateMibBase.siteGroup.siteControllerModel[1]
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/rect_status"))
    {
        uint8_t i;
        n=0;
        for (i = 0; i < privateMibBase.rectGroup.rectInstalledRect; i++)
        {
            if(n<300)
            {
                n += snprintf(buffer + n, 300 - n,
                              "%d;%s;%d;%d;%d@",
                              i + 1,
                              privateMibBase.rectGroup.rectTable[i].rectSerialNo,
                              privateMibBase.rectGroup.rectTable[i].rectOutputVoltage,
                              privateMibBase.rectGroup.rectTable[i].rectOutputCurrent,
                              privateMibBase.rectGroup.rectTable[i].rectTemp
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/libbatt_status"))//edit buffer 300 to 500
    {
        uint8_t i;
        n=0;
        for (i = 0; i < 16; i++)
        {
            if(n<500)
            {
                n += snprintf(buffer + n, 500 - n,
                              "%d;%d;%d;%d;%d@",
                              i + 1,
                              privateMibBase.liBattGroup.liBattTable[i].liBattPackVolt,
                              privateMibBase.liBattGroup.liBattTable[i].liBattPackCurr,
                              privateMibBase.liBattGroup.liBattTable[i].liBattAmbTemp,
                              privateMibBase.liBattGroup.liBattTable[i].liBattSOC
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/geninfo_status"))
    {
        uint8_t i;
        n=0;
        for (i = 0; i < 2; i++)
        {
            if(n<300)
            {
                n += snprintf(buffer + n, 300 - n,
                              "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d@",
                              i + 1,
                              privateMibBase.genGroup.genTable[i].genStatusStatus,
                              privateMibBase.genGroup.genTable[i].genStatusLNVolt1,
                              privateMibBase.genGroup.genTable[i].genStatusLNVolt2,
                              privateMibBase.genGroup.genTable[i].genStatusLNVolt3,
                              privateMibBase.genGroup.genTable[i].genStatusLNCurr1,
                              privateMibBase.genGroup.genTable[i].genStatusLNCurr2,
                              privateMibBase.genGroup.genTable[i].genStatusLNCurr3,
                              privateMibBase.genGroup.genTable[i].genStatusOilPressure,
                              privateMibBase.genGroup.genTable[i].genStatusFuelLevel,
                              privateMibBase.genGroup.genTable[i].genStatusBattVolt,
                              privateMibBase.genGroup.genTable[i].genStatusFrequency,
                              privateMibBase.genGroup.genTable[i].genStatusCoolantTemp,
                              privateMibBase.genGroup.genTable[i].genStatusWorkingHour,
                              privateMibBase.genGroup.genTable[i].genStatusEngineSpeed
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/fuelinfo_status"))
    {
        uint8_t i;
        n=0;
        for (i = 0; i < 2; i++)
        {
            if(n<300)
            {
                n += snprintf(buffer + n, 300 - n,
                              "%d;%d;%d@",
                              i + 1,
                              privateMibBase.fuelGroup.FuelTable[i].FuelStatus,
                              privateMibBase.fuelGroup.FuelTable[i].FuelLevel
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/liion_status"))//edit buffer 300 to 500
    {
        uint8_t i;
        n=0;
        for (i = 0; i < privateMibBase.liBattGroup.liBattInstalledPack; i++)
        {
            if(n<500)
            {
                n += snprintf(buffer + n, 500 - n,
                              "%d;%d@",
                              privateMibBase.liBattGroup.liBattTable[i].liBattModbusID,
                              privateMibBase.liBattGroup.liBattTable[i].liBattType
                              //privateMibBase.liBattGroup.liBattTable[i].liBattSerialNo
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/generator_status"))
    {
        uint8_t i;
        n=0;
        for (i = 0; i < privateMibBase.genGroup.genInstalledGen; i++)
        {
            if(n<300)
            {
                n += snprintf(buffer + n, 300 - n,
                              "%d;%d;%s@",
                              privateMibBase.genGroup.genTable[i].genStatusModbusID,
                              privateMibBase.genGroup.genTable[i].genStatusType,
                              "Generator"
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
#if (USERDEF_MONITOR_BM == ENABLED)
    else if(!strcasecmp(request, "/labm_status"))
    {
        uint8_t i;
        n=0;
        for (i = 0; i < privateMibBase.bmGroup.bmInstalledBM; i++)
        {
            if(n<300)
            {
                n += snprintf(buffer + n, 300 - n,
                              "%d;%d@",
                              privateMibBase.bmGroup.bmTable[i].bmModbusID,
                              privateMibBase.bmGroup.bmTable[i].bmType
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
#endif
    
#if (USERDEF_MONITOR_SMCB == ENABLED)
    else if(!strcasecmp(request, "/smcb_status"))
    {
        uint8_t i;
        n=0;
        for (i = 0; i < privateMibBase.smcbGroup.SmcbInstalledSMCB; i++)
        {
            if(n<300)
            {
                n += snprintf(buffer + n, 300 - n,
                              "%d;%d;%d;%d@",
                              privateMibBase.smcbGroup.SmcbTable[i].SmcbModbusID,
                              privateMibBase.smcbGroup.SmcbTable[i].SmcbType,
                              privateMibBase.smcbGroup.SmcbTable[i].SmcbStatus,
                              privateMibBase.smcbGroup.SmcbTable[i].SmcbState
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
#endif
#if (USERDEF_MONITOR_PM == ENABLED)  
    else if(!strcasecmp(request, "/powermeter_status"))
    {
        uint8_t i;
        n=0;
        for (i = 0; i < privateMibBase.pmGroup.pmInstalledPM; i++)
        {
            if(n<300)
            {
                n += snprintf(buffer + n, 300 - n,
                              "%d;%d;%s@",
                              privateMibBase.pmGroup.pmTable[i].pmModbusID,
                              privateMibBase.pmGroup.pmTable[i].pmType,
                              "PowerMeter"
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
#endif
    
#if (USERDEF_MONITOR_ISENSE == ENABLED)  
    else if(!strcasecmp(request, "/i_sense_status"))
    {
        n=0;
        if(privateMibBase.isenseGroup.isenseInstalledISENSE){
          if(n<300)
          {
              n += snprintf(buffer + n, 300 - n,
                            "%d;%d;%s@",
                            privateMibBase.isenseGroup.isenseTable.isenseModbusID,
                            privateMibBase.isenseGroup.isenseTable.isenseType,
                            "PowerMeter"
                           );
          }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
#endif
#if(USERDEF_MONITOR_PM_DC == ENABLED)
    else if(!strcasecmp(request, "/pm_dc_status")){
      n=0;
      if(privateMibBase.pm_dc_group.pm_dc_installed_PM_DC){
        if(n<300)
        {
            n += snprintf(buffer + n, 300 - n,
                          "%d;%d;%s@",
                          privateMibBase.pm_dc_group.pm_dc_table.pm_dc_modbus_ID,
                          privateMibBase.pm_dc_group.pm_dc_table.pm_dc_type,
                          "PowerMeter"
                         );
        }
      }

      //Format HTTP response header
      connection->response.version = connection->request.version;
      connection->response.statusCode = 200;
      connection->response.keepAlive = connection->request.keepAlive;
      connection->response.noCache = TRUE;
      connection->response.contentType = mimeGetType(".html");
      connection->response.chunkedEncoding = FALSE;
      connection->response.contentLength = n;

      //Send the header to the client
      error = httpWriteHeader(connection);
      //Any error to report?
      if(error) return error;

      //Send response body
      error = httpWriteStream(connection, buffer, n);
      //Any error to report?
      if(error) return error;

      //Properly close output stream
      error = httpCloseStream(connection);
      //Return status code
      return error;
      
    }
#endif
#if (USERDEF_MONITOR_VAC == ENABLED)  
    else if(!strcasecmp(request, "/vac_status"))
    {
        uint8_t i;
        n=0;
        for (i = 0; i < privateMibBase.vacGroup.vacInstalledVAC; i++)
        {
            if(n<300)
            {
                n += snprintf(buffer + n, 300 - n,
                              "%d;%d;%s@",
                              privateMibBase.vacGroup.vacTable[i].vacModbusID,
                              privateMibBase.vacGroup.vacTable[i].vacType,
                              "VAC"
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
#endif     
#if (USERDEF_MONITOR_FUEL == ENABLED)  
    else if(!strcasecmp(request, "/fuel_status"))
    {
        uint8_t i;
        n=0;
        for (i = 0; i < privateMibBase.fuelGroup.FuelInstalledFUEL; i++)
        {
            if(n<300)
            {
                n += snprintf(buffer + n, 300 - n,
                              "%d;%d;%s@",
                              privateMibBase.fuelGroup.FuelTable[i].FuelModbusID,
                              privateMibBase.fuelGroup.FuelTable[i].FuelType,
                              "FUEL"
                             );
            }
        }

        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
#endif 
    else if(!strcasecmp(request, "/battconfig_status"))
    {
        n = sprintf(buffer,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%s;%s;%s;%s;%d;%d;%s;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
                    privateMibBase.configGroup.cfgCurrentLimit,//0
                    privateMibBase.configGroup.cfgFloatVolt,//1
                    privateMibBase.configGroup.cfgBoostVolt,//2
                    privateMibBase.configGroup.cfgDCLowVolt,//3
                    privateMibBase.configGroup.cfgLLVDVolt,//4
                    privateMibBase.configGroup.cfgBLVDVolt,//5
                    privateMibBase.configGroup.cfgTempCompValue,//6
                    privateMibBase.configGroup.cfgBattCapacityTotal,//7
                    privateMibBase.configGroup.cfgHighMajorTempLevel,//8
                    privateMibBase.configGroup.cfgWalkInTimeEn,//9
                    privateMibBase.configGroup.cfgWalkInTimeDuration,//10
                    privateMibBase.configGroup.cfgAcLowLevel,//11
                    privateMibBase.configGroup.cfgAcUnderLevel,//12
                    privateMibBase.configGroup.cfgAcHighLevel,//13
                    privateMibBase.configGroup.cfgOverMajorTempLevel,//14
                    privateMibBase.configGroup.cfgBattTestVolt,//15
                    privateMibBase.configGroup.cfgBattCapacityTotal2,//16
                    privateMibBase.cfgBTGroup.cfgBTEndVolt,//17
                    privateMibBase.cfgBTGroup.cfgBTEndCap,//18
                    privateMibBase.cfgBTGroup.cfgBTEndTime,//19
                    privateMibBase.cfgBTGroup.cfgBTPlanTestEn,//20
                    0,//21
                    privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestString,//22
                    privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestString,//23
                    privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestString,//24
                    privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestString,//25
                    privateMibBase.configGroup.cfgDCUnderVolt,//26
                    privateMibBase.configGroup.cfgDCOverVolt,//27
                    privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUString,//28
                    privateMibBase.configGroup.cfgLowTempLevel,//29
                    privateMibBase.cfgBTGroup.cfgTestVoltCfg,//30
                    privateMibBase.cfgBTGroup.cfgAutoTestDay,//31
                    privateMibBase.cfgBTGroup.cfgTestStartTime,//32
                    privateMibBase.siteGroup.siteControllerModel[0],//33
                    privateMibBase.configGroup.cfgLLVDEn,//34
                    privateMibBase.configGroup.cfgBLVDEn,//35
                    privateMibBase.configGroup.cfgBattCapacityTotal3,//36 thanhcm3 add for dpc
                    privateMibBase.configGroup.cfgBattCapacityTotal4, //37 thanhcm3 add for dpc
                    privateMibBase.fan_dpc_info.mib.para_dc_starting_point,//38 thanhcm3 add for dpc
                    privateMibBase.fan_dpc_info.mib.para_dc_sensivive_point,//39 thanhcm3 add for dpc
                    privateMibBase.fan_dpc_info.mib.para_alarm_high_temp,//40 thanhcm3 add for dpc
                    privateMibBase.fan_dpc_info.mib.para_alarm_low_temp, //41 thanhcm3 add for dpc
                    privateMibBase.configGroup.cfgCurrentlimitA,      //42 thanhcm3 add for dpc
                    privateMibBase.fan_dpc_info.mib.para_dc_heating_start_point,//43 thanhcm3 add for dpc
                    privateMibBase.fan_dpc_info.mib.para_dc_heating_sensivive    //44 thanhcm3 add for dpc
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/alarmGen_status"))
    {
        n = sprintf(buffer,"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
                    privateMibBase.mainAlarmGroup.alarmBattHighTemp,
                    privateMibBase.mainAlarmGroup.alarmLLVD,
                    privateMibBase.mainAlarmGroup.alarmBLVD,
                    privateMibBase.mainAlarmGroup.alarmDCLow,
                    privateMibBase.mainAlarmGroup.alarmACmains,
                    privateMibBase.mainAlarmGroup.alarmBattBreakerOpen,
                    privateMibBase.mainAlarmGroup.alarmLoadBreakerOpen,
                    privateMibBase.mainAlarmGroup.alarmRectACFault,
                    privateMibBase.mainAlarmGroup.alarmRectNoResp,
                    privateMibBase.mainAlarmGroup.alarmManualMode,
                    privateMibBase.mainAlarmGroup.alarmACLow,
                    privateMibBase.mainAlarmGroup.alarmSmoke,
                    privateMibBase.mainAlarmGroup.alarmEnvDoor
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/alarmDI_status"))
    {
        n = sprintf(buffer,"%d;%d;%d;%d;%d;%d;%d;%d",
                    privateMibBase.diAlarmGroup.alarmDigitalInput[0],
                    privateMibBase.diAlarmGroup.alarmDigitalInput[1],
                    privateMibBase.diAlarmGroup.alarmDigitalInput[2],
                    privateMibBase.diAlarmGroup.alarmDigitalInput[3],
                    privateMibBase.diAlarmGroup.alarmDigitalInput[4],
                    privateMibBase.diAlarmGroup.alarmDigitalInput[5],
                    privateMibBase.diAlarmGroup.alarmDigitalInput[6],
                    privateMibBase.diAlarmGroup.alarmDigitalInput[7]
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/alarmHis_status"))
    {
    	uint16_t n = 0,i;
        for (i = 0; i < 200; i++)
        {
                if(n < 16000)
                {
                        n += snprintf(fs_pbuf + n, 16000 - n,
                                        "%d;%s;%s;%s;%s@",
                                        i,
                                        sHistoryInfo.sHistoryParam[i].sAlarmContent.ucName,
                                        sHistoryInfo.sHistoryParam[i].sAlarmContent.cRectID,
                                        sHistoryInfo.sHistoryParam[i].sTimeFrame[0].cTimeFrame,
                                        sHistoryInfo.sHistoryParam[i].sTimeFrame[1].cTimeFrame
                                        );
                }
        }
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, fs_pbuf, n);//buffer
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/alrmget"))
    {
      sHistoryInfo.ucSequenceNum = 0;
      if (TYPE == EMERV21) {MESGState_v21 = HISTORYMESG_REQ;}
      else MESGState = HISTORYMESG_REQ;
      
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/sp_status"))
    {
        n = sprintf(buffer,"%d;%d;%d;%d",
                    9600,
                    8,
                    0,
                    1
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/snmp_status"))
    {
        n = sprintf(buffer,"%d;%d;%s",
                    privateMibBase.cfgNetworkGroup.u8TrapMode,
                    privateMibBase.cfgNetworkGroup.u32TrapInterval,
                    privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSIP
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/dev_status"))
    {
        n = sprintf(buffer,"%s;%s;%s;%s;%s-V2.10.28;%s;%s",
                    privateMibBase.siteGroup.siteDevModel,
                    "Power System Monitor",
                    privateMibBase.siteGroup.siteSerialNumber,
                    "HWV1R3",
                    privateMibBase.siteGroup.siteCRCStr,
                    g_sWorkingDefaultParameters2.u8CompileInfo,
                    privateMibBase.siteGroup.siteMACInfo
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/deviceip_status"))
    {

        n = sprintf(buffer,"%s;%s;%s;%s;%d;%d;%s",
                    privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucIP,
                    privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucGW,
                    privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSN,
                    privateMibBase.siteGroup.siteBTSCode,
                    privateMibBase.siteGroup.siteControllerModel[0],
                    privateMibBase.siteGroup.siteControllerModel[1],
                    privateMibBase.cfgNetworkGroup.cFTPServer
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(strncmp(request, "/devsip1?value=", 7) == 0)//(!strcasecmp(request, "/devsip"))
    {
        uint32_t i,temp=0;
//        bool bConfirmed[12]={0};

        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("devsip1"));
        ucdata = request;
        switch (ucdata[sizeof("devsip1")-1]-0x30)
        {
        case 1:
        {
            sIpConfig.u32StaticIP = (sIpConfig.u32StaticIP &  0x00ffffff) | (temp << 24);
            bConfirmed[0] = 1;
        }
        break;
        case 2:
        {
            sIpConfig.u32StaticIP = (sIpConfig.u32StaticIP &  0xff00ffff) | (temp << 16);
            bConfirmed[1] = 1;
        }
        break;
        case 3:
        {
            sIpConfig.u32StaticIP = (sIpConfig.u32StaticIP &  0xffff00ff) | (temp << 8);
            bConfirmed[2] = 1;
        }
        break;
        case 4:
        {
            sIpConfig.u32StaticIP = (sIpConfig.u32StaticIP &  0xffffff00) | (temp << 0);
            bConfirmed[3] = 1;
        }
        break;

        case 5:
        {
            sIpConfig.u32GatewayIP = (sIpConfig.u32GatewayIP &  0x00ffffff) | (temp << 24);
            bConfirmed[4] = 1;
        }
        break;
        case 6:
        {
            sIpConfig.u32GatewayIP = (sIpConfig.u32GatewayIP &  0xff00ffff) | (temp << 16);
            bConfirmed[5] = 1;
        }
        break;
        case 7:
        {
            sIpConfig.u32GatewayIP = (sIpConfig.u32GatewayIP &  0xffff00ff) | (temp << 8);
            bConfirmed[6] = 1;
        }
        break;
        case 8:
        {
            sIpConfig.u32GatewayIP = (sIpConfig.u32GatewayIP &  0xffffff00) | (temp << 0);
            bConfirmed[7] = 1;
        }
        break;

        case 9:
        {
            sIpConfig.u32SubnetMask = (sIpConfig.u32SubnetMask &  0x00ffffff) | (temp << 24);
            bConfirmed[8] = 1;
        }
        break;
        case 17:
        {
            sIpConfig.u32SubnetMask = (sIpConfig.u32SubnetMask &  0xff00ffff) | (temp << 16);
            bConfirmed[9] = 1;
        }
        break;
        case 18:
        {
            sIpConfig.u32SubnetMask = (sIpConfig.u32SubnetMask &  0xffff00ff) | (temp << 8);
            bConfirmed[10] = 1;
        }
        break;
        case 19:
        {
            sIpConfig.u32SubnetMask = (sIpConfig.u32SubnetMask &  0xffffff00) | (temp << 0);
            bConfirmed[11] = 1;
        }
        break;
        default:
            break;
        };

        if ((bConfirmed[0] == 1)&&(bConfirmed[1] == 1)&&(bConfirmed[2] == 1)&&(bConfirmed[3] == 1)&&
                (bConfirmed[4] == 1)&&(bConfirmed[5] == 1)&&(bConfirmed[6] == 1)&&(bConfirmed[7] == 1)&&
                (bConfirmed[8] == 1)&&(bConfirmed[9] == 1)&&(bConfirmed[10] == 1)&&(bConfirmed[11] == 1))
        {
            for (i = 0; i < 12; i++)
            {
                bConfirmed[i] = 0;
            }
            //
            // Did any of the telnet parameters change?
            //
            if  (g_sParameters.u32StaticIP != sIpConfig.u32StaticIP)
            {
                //
                // Update the current parameters with the new settings.
                //
                g_sParameters.u32StaticIP = sIpConfig.u32StaticIP;
            }

            //
            // Did any of the telnet parameters change?
            //
            if  (g_sParameters.u32GatewayIP != sIpConfig.u32GatewayIP)
            {
                //
                // Update the current parameters with the new settings.
                //
                g_sParameters.u32GatewayIP = sIpConfig.u32GatewayIP;
            }
            //
            // Did any of the telnet parameters change?
            //
            if  (g_sParameters.u32SubnetMask != sIpConfig.u32SubnetMask)
            {
                //
                // Update the current parameters with the new settings.
                //
                g_sParameters.u32SubnetMask = sIpConfig.u32SubnetMask;
            }

            //
            // Yes - save these settings as the defaults.
            //
            g_sWorkingDefaultParameters.u32StaticIP = g_sParameters.u32StaticIP;
            g_sWorkingDefaultParameters.u32GatewayIP = g_sParameters.u32GatewayIP;
            g_sWorkingDefaultParameters.u32SubnetMask = g_sParameters.u32SubnetMask;
            u8SaveConfigFlag |= 1;
            u8IsRebootFlag = 1;
            //
            // Tell the main loop that a IP address update has been requested.
            //
//              g_cUpdateRequired |= UPDATE_IP_ADDR;

        };
//        //Return status code
        return error;
    }
    else if(strncmp(request, "/battDCOver?value=", 10) == 0)
    {
        //
        // Battery DC Over
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgDCOverVolt,sizeof("battDCOver"));
        setCmd_flag = 1;
        setCmd_mask = SET_DCOVER;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battDCLow?value=", 9) == 0)
    {
        //
        // Battery DC Low
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgDCLowVolt,sizeof("battDCLow"));
        setCmd_flag = 1;
        setCmd_mask = SET_DCLOW;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battDCUnder?value=", 12) == 0)
    {
        //
        // Battery DC Under
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgDCUnderVolt,sizeof("battDCUnder"));
        setCmd_flag = 1;
        setCmd_mask = SET_DCUNDER;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battEnLLVD?value=", 10) == 0)
    {
        //
        // LLVD Enable
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgLLVDEn,sizeof("battEnLLVD"));
        setCmd_flag = 1;
        setCmd_mask = SET_LLVD_EN;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battEnBLVD?value=", 10) == 0)
    {
        //
        // BLVD Enable
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgBLVDEn,sizeof("battEnBLVD"));
        setCmd_flag = 1;
        setCmd_mask = SET_BLVD_EN;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battCCL?value=", 8) == 0)
    {
        //
        // Charge Current Limit(C)
        //
      if(privateMibBase.siteGroup.siteControllerModel[0] != 4)
      {
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgCurrentLimit,sizeof("battCCL"));
        setCmd_flag = 1;
        setCmd_mask = SET_CCL;
      }

        //Return status code
        return error;
    }
    else if (strncmp(request, "/battdpcCCL?value=", 11) == 0)
    {
      //
      // charge current limit (A)
      //
      if(privateMibBase.siteGroup.siteControllerModel[0] == 4)
      {
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgCurrentlimitA,sizeof("battdpcCCL"));
        setCmd_flag = 1;
        setCmd_mask = SET_CCL;
      }
      //Return status code
      return error;
    }
    else if(strncmp(request, "/battFV?value=", 7) == 0)
    {
        //
        // Float Voltage
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgFloatVolt,sizeof("battFV"));
        setCmd_flag = 1;
        setCmd_mask = SET_FLTVOL;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battBV?value=", 7) == 0)
    {
        //
        // Boost Voltage
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgBoostVolt,sizeof("battBV"));
        setCmd_flag = 1;
        setCmd_mask = SET_BSTVOL;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battLLVD?value=", 9) == 0)
    {
        //
        // Battery LLVD
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgLLVDVolt,sizeof("battLLVD"));
        setCmd_flag = 1;
        setCmd_mask = SET_LLVD;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battBLVD?value=", 9) == 0)
    {
        //
        // Battery BLVD
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgBLVDVolt,sizeof("battBLVD"));
        setCmd_flag = 1;
        setCmd_mask = SET_BLVD;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battTempComp?value=", 9) == 0)
    {
        //
        // Battery Temp Compensation
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgTempCompValue,sizeof("battTempComp"));
        setCmd_flag = 1;
        setCmd_mask = SET_TEMPCOMP_VAL;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battCap1?value=", 9) == 0)
    {
        //
        // Battery Capacity 1
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgBattCapacityTotal,sizeof("battCap1"));
        setCmd_flag = 1;
        setCmd_mask = SET_BATTSTDCAP;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battCap2?value=", 9) == 0)
    {
        //
        // Battery Capacity 2
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgBattCapacityTotal2,sizeof("battCap2"));
        setCmd_flag = 1;
        if(privateMibBase.siteGroup.siteControllerModel[0] == 1)
        {
                setCmd_mask = SET_BATTSTDCAP2;
        }else if(privateMibBase.siteGroup.siteControllerModel[0] == 2)
        {
                setCmd_mask = SET_BATTSTDCAP;
        }else if(privateMibBase.siteGroup.siteControllerModel[0] == 4){
                setCmd_mask = SET_BATTSTDCAP2;
        }

        //Return status code
        return error;
    }
    else if(strncmp(request, "/battCap3?value=", 9) == 0)
    {
        //
        // Battery Capacity 3
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgBattCapacityTotal3,sizeof("battCap3"));
        setCmd_flag = 1;
        if(privateMibBase.siteGroup.siteControllerModel[0] == 4){
        setCmd_mask = SET_BATTSTDCAP3;
        }
        //Return status code
        return error;
    }
    else if(strncmp(request, "/battCap4?value=", 9) == 0)
    {
        //
        // Battery Capacity 4
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgBattCapacityTotal4,sizeof("battCap4"));
        setCmd_flag = 1;
        if(privateMibBase.siteGroup.siteControllerModel[0] == 4){
        setCmd_mask = SET_BATTSTDCAP4;
        }
        //Return status code
        return error;
    }
    else if(strncmp(request, "/fanstartpoint?value=", 14) == 0)
    {
      // fan dc startting point
      if(privateMibBase.siteGroup.siteControllerModel[0] == 4){
        
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.fan_dpc_info.mib.temp_set,sizeof("fanstartpoint"));
        privateMibBase.fan_dpc_info.mib.para_dc_starting_point = (uint16_t) privateMibBase.fan_dpc_info.mib.temp_set;
        fan_info_dpc_t.para_dc_starting_point                  = (uint16_t) privateMibBase.fan_dpc_info.mib.temp_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_STARTING_POINT;
      }
      return error;
    }
    else if(strncmp(request, "/fansensitpoint?value=", 15) == 0)
    {
      // fan dc sensivive point
      if(privateMibBase.siteGroup.siteControllerModel[0] == 4){
        
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.fan_dpc_info.mib.temp_set,sizeof("fansensitpoint"));
        privateMibBase.fan_dpc_info.mib.para_dc_sensivive_point = (uint16_t) privateMibBase.fan_dpc_info.mib.temp_set;
        fan_info_dpc_t.para_dc_sensivive_point                  = (uint16_t) privateMibBase.fan_dpc_info.mib.temp_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_SENSITIVE_POINT;
      }
      return error;
    }
    else if(strncmp(request, "/fanalarmHtemp?value=", 14) == 0)
    {
      // fan alarm high temp
      if(privateMibBase.siteGroup.siteControllerModel[0] == 4){
        
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.fan_dpc_info.mib.temp_set,sizeof("fanalarmHtemp"));
        privateMibBase.fan_dpc_info.mib.para_alarm_high_temp = (uint16_t) privateMibBase.fan_dpc_info.mib.temp_set;
        fan_info_dpc_t.para_alarm_high_temp                  = (uint16_t) privateMibBase.fan_dpc_info.mib.temp_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_H_TEMP_W_POINT;
      }
      return error;
    }
    else if(strncmp(request, "/fanalarmLtemp?value=", 14) == 0)
    {
      // fan alarm low temp
      if(privateMibBase.siteGroup.siteControllerModel[0] == 4){
        
        u16TextBufferLen = htmlTextExtract2(request,(int32_t*)&privateMibBase.fan_dpc_info.mib.i_tem_set,sizeof("fanalarmLtemp"));
        privateMibBase.fan_dpc_info.mib.para_alarm_low_temp = (int16_t) privateMibBase.fan_dpc_info.mib.i_tem_set;
        fan_info_dpc_t.para_alarm_low_temp                  = (uint16_t) privateMibBase.fan_dpc_info.mib.i_tem_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_L_TEMP_W_POINT;
      }
      return error;
    }
    else if (strncmp(request, "/fanHeatSpoint?value=", 14) == 0)
    {
      // fan alarm heat start temp
      if(privateMibBase.siteGroup.siteControllerModel[0] == 4){
        
        u16TextBufferLen = htmlTextExtract2(request,(int32_t*)&privateMibBase.fan_dpc_info.mib.i_tem_set,sizeof("fanHeatSpoint"));
        privateMibBase.fan_dpc_info.mib.para_dc_heating_start_point = (uint16_t) privateMibBase.fan_dpc_info.mib.i_tem_set;
        fan_info_dpc_t.para_dc_heating_start_point                 = (uint16_t) privateMibBase.fan_dpc_info.mib.i_tem_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_HEATING_START_POINT;
      }
      return error;
    }
    else if (strncmp(request, "/fanHeatSI?value=", 10) == 0)
    {
      // fan alarm heat sensivive temp
      if(privateMibBase.siteGroup.siteControllerModel[0] == 4){
        
        u16TextBufferLen = htmlTextExtract2(request,(int32_t*)&privateMibBase.fan_dpc_info.mib.i_tem_set,sizeof("fanHeatSI"));
        privateMibBase.fan_dpc_info.mib.para_dc_heating_sensivive = (uint16_t) privateMibBase.fan_dpc_info.mib.i_tem_set;
        fan_info_dpc_t.para_dc_heating_sensivive                  = (uint16_t) privateMibBase.fan_dpc_info.mib.i_tem_set;
        sModbusManager.SettingCommand = SET_FAN_DPC_HEATING_SENSITIVE_POINT;
      }
      return error;
    }
    else if(strncmp(request, "/battHiTemp?value=", 9) == 0)
    {
        //
        // Battery High Temp
        //
        u16TextBufferLen = htmlTextExtract2(request,(int32_t*)&privateMibBase.configGroup.cfgHighMajorTempLevel,sizeof("battHiTemp"));
        setCmd_flag = 1;
        setCmd_mask = SET_OVERTEMP;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/rectWITE?value=", 9) == 0)
    {
        //
        // Rectifier walk-in Time Enable
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgWalkInTimeEn,sizeof("rectWITE"));
        setCmd_flag = 1;
        setCmd_mask = SET_WIT_EN;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/rectWITI?value=", 9) == 0)
    {
        //
        // Rectifier walk-in Time Interval
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgWalkInTimeDuration,sizeof("rectWITI"));
        setCmd_flag = 1;
        setCmd_mask = SET_WIT_VAL;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/acHiThres?value=", 10) == 0)
    {
        //
        // AC High Threshold
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgAcHighLevel,sizeof("acHiThres"));
        setCmd_flag = 1;
        setCmd_mask = SET_AC_HIGH_THRES;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/acLowThres?value=", 11) == 0)
    {
        //
        // AC Low Threshold
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.configGroup.cfgAcLowLevel,sizeof("acLowThres"));
        setCmd_flag = 1;
        setCmd_mask = SET_AC_THRES;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/btEndVolt?value=", 10) == 0)
    {
        //
        // Battery End Voltage
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.cfgBTGroup.cfgBTEndVolt,sizeof("btEndVolt"));
        setCmd_flag = 1;
        setCmd_mask = SET_BT_ENDVOLT;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/btEndCap?value=", 9) == 0)
    {
        //
        // Battery End Capacity
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.cfgBTGroup.cfgBTEndCap,sizeof("btEndCap"));
        setCmd_flag = 1;
        setCmd_mask = SET_BT_ENDCAP;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/btEndTime?value=", 10) == 0)
    {
        //
        // Battery End Time
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.cfgBTGroup.cfgBTEndTime,sizeof("btEndTime"));
        setCmd_flag = 1;
        setCmd_mask = SET_BT_ENDTIME;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/btPlanEn?value=", 9) == 0)
    {
        //
        // Battery Test Plan Enable
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.cfgBTGroup.cfgBTPlanTestEn,sizeof("btPlanEn"));
        setCmd_flag = 1;
        setCmd_mask = SET_BT_PLEN;

        //Return status code
        return error;
    }
    //==================================== Agisson Battery Test ======================================================//
    else if(strncmp(request, "/btplstart?id=", 9) == 0)
    {
        //
        // Battery Test Plan Start
        //
        setCmd_flag = 1;
        setCmd_mask = START_BT_TEST;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/btplstop?id=", 9) == 0)
    {
        //
        // Battery Test Plan Stop
        //
        setCmd_flag = 1;
        setCmd_mask = STOP_BT_TEST;

        //Return status code
        return error;
    }
    //==================================== Agisson Battery Test ======================================================//
    //==================================== ZTE Battery Test ======================================================//    
    else if(strncmp(request, "/btZTETestVolt?value=", 13) == 0)
    {
        //
        // Battery Test Volt
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.cfgBTGroup.cfgTestVoltCfg,sizeof("btZTETestVolt"));
        setCmd_flag = 1;
        setCmd_mask = SET_TEST_VOLT;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/btZTETestDay?value=", 13) == 0)
    {
        //
        // Battery Test Day
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.cfgBTGroup.cfgAutoTestDay,sizeof("btZTETestDay"));
        setCmd_flag = 1;
        setCmd_mask = SET_AUTO_TEST_DAY;

        //Return status code
        return error;
    }
    else if(strncmp(request, "/btZTETestTime?value=", 13) == 0)
    {
        //
        // Battery Test Time
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.cfgBTGroup.cfgTestStartTime,sizeof("btZTETestTime"));
        setCmd_flag = 1;
        setCmd_mask = SET_TEST_START_TIME;

        //Return status code
        return error;
    }
    
    //==================================== ZTE Battery Test ======================================================//
    else if(strncmp(request, "/plantest1m?value=", 9) == 0)
    {
        uint32_t i,temp=0;
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("plantest1m"));
        data = request;
        switch (data[sizeof("plantest1m")-2]-0x30)
        {
                case 1:
                {
                        if(strncmp(&data[sizeof("plantest1m")-1],"m",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestMonth = temp;
                        }
                        else if(strncmp(&data[sizeof("plantest1m")-1],"d",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestDate = temp;
                        }
                        else if(strncmp(&data[sizeof("plantest1m")-1],"h",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[0].cfgBTPlanTestHour = temp;
                        }
                        sBattTestInfo.u8SetPlan = 1;
                        setCmd_flag = 1;
                        setCmd_mask = SET_BT_PLANTEST;
                }
                break;
                case 2:
                {
                        if(strncmp(&data[sizeof("plantest1m")-1],"m",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestMonth = temp;
                        }
                        else if(strncmp(&data[sizeof("plantest1m")-1],"d",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestDate = temp;
                        }
                        else if(strncmp(&data[sizeof("plantest1m")-1],"h",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[1].cfgBTPlanTestHour = temp;
                        }
                        sBattTestInfo.u8SetPlan = 2;
                        setCmd_flag = 1;
                        setCmd_mask = SET_BT_PLANTEST;
                }
                break;
                case 3:
                {
                        if(strncmp(&data[sizeof("plantest1m")-1],"m",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestMonth = temp;
                        }
                        else if(strncmp(&data[sizeof("plantest1m")-1],"d",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestDate = temp;
                        }
                        else if(strncmp(&data[sizeof("plantest1m")-1],"h",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[2].cfgBTPlanTestHour = temp;
                        }
                        sBattTestInfo.u8SetPlan = 3;
                        setCmd_flag = 1;
                        setCmd_mask = SET_BT_PLANTEST;
                }
                break;
                case 4:
                {
                        if(strncmp(&data[sizeof("plantest1m")-1],"m",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestMonth = temp;
                        }
                        else if(strncmp(&data[sizeof("plantest1m")-1],"d",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestDate = temp;
                        }
                        else if(strncmp(&data[sizeof("plantest1m")-1],"h",1) == 0)
                        {
                          privateMibBase.cfgBTGroup.cfgBTPlanTestTable[3].cfgBTPlanTestHour = temp;
                        }
                        sBattTestInfo.u8SetPlan = 4;
                        setCmd_flag = 1;
                        setCmd_mask = SET_BT_PLANTEST;
                }
                break;
                default:
                        break;
        };
        sBattTestInfo.u8CheckSentAll = 1;    
        //Return status code
        return error;
    }
    else if(strncmp(request, "/scutimem?value=", 8) == 0)
    {
        uint32_t i,temp=0;
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("scutimem"));
        data = request;
        if(strncmp(&data[sizeof("scutimem")-1],"y",1) == 0)
        {
          privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUYear = temp;
        }
        else if(strncmp(&data[sizeof("scutimem")-1],"m",1) == 0)
        {
          privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUMonth = temp;
        }
        else if(strncmp(&data[sizeof("scutimem")-1],"d",1) == 0)
        {
          privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUDate = temp;
        }
        else if(strncmp(&data[sizeof("scutimem")-1],"h",1) == 0)
        {
          privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUHour = temp;
        }
        else if(strncmp(&data[sizeof("scutimem")-1],"p",1) == 0)
        {
          privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUMinute = temp;
        }
        else if(strncmp(&data[sizeof("scutimem")-1],"s",1) == 0)
        {
          privateMibBase.cfgBTGroup.cfgBTSCUTable[0].cfgBTSCUSecond = temp;
        }

        sBattTestInfo.u8SetPlan = 5;
        setCmd_flag = 1;
        setCmd_mask = SET_BT_SCUTIME;

        sBattTestInfo.u8CheckSentAll = 1;

        //Return status code
        return error;
    }
    //=============================================== Lithium Battery Setting ===================================================//
    else if(strncmp(request, "/liionpack1i?value=", 10) == 0)
    {
        uint32_t i,j,temp=0;
        tModbusParameters sModbusDev[20];
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("liionpack1i"));
        data = request;
        j = data[sizeof("liionpack1i")-2]-0x30;
        switch (j)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        {
            if(strncmp(&data[sizeof("liionpack1i")-1],"v",1) == 0)
            {
                sModbusManager.sLIBManager[j-1].u8LIBType = temp;
                switch(sModbusManager.sLIBManager[j-1].u8LIBType)
                {
                case 13:// COSLIGHT_CF4850T  
                case 1:// COSLIGHT
                {
                    sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;      // FC 04
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 58;      
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 120;//128;    // FC 02
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 38;//30; 
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 48;    // FC 03
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 4;
                }
                break;
                case 2:// COSLIGHT_OLD_V1.1
                {
                    sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 30;      // FC 04
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 24;      
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 128;    // FC 02
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 28; 
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 48;    // FC 03
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 2;
                }
                break;
                case 12:// SHOTO_SDA10_48100 
                case 3:// SHOTO_2019
                {
                    sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0x1000;      // FC 04
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 10;      
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 0x2000;   
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 25; 
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 0x3040;   
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 56; 
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[3] = 0x800f;   
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[3] = 16; 
                }
                break;
                case 4:// HUAWEI
                {
                    if(j == 9)
                      sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 215;
                    else
                      sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 213;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset + j;
//                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = 213 + j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 50; 
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 67;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 8;  
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 257;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 7;  
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[3] = 529;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[3] = 10; 
                }
                break;
                case 5:// M1Viettel50
                {
                    sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0; // FC:0x04
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 100;   
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 130;// FC:0x04
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 22;  //20
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 0;// FC:0x03
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 80; 
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[3] = 162;// FC:0x03
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[3] = 3;//2                 
                }
                break;
                case 14:// HUAFU_HF48100C
                case 6:// ZTT_2020
                {
                    sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 42;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 150;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 10;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 170;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 10;
                }
                break;
                case 9:// SAFT
                {
                    sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 23764;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 16;
                }
                break;
                case 10:// Narada75
                {
                    sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 38;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = 38 + j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 4095;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 9;
                    
                }
                break;
                case 7:// ZTT50
                {
                    sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 38;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = 38 + j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 4096;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 9;
                }
                break;
                case 8:// HUAWEI_A1
                {
                    if(j == 9)
                      sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 215;
                    else
                      sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 213;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset + j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 50; 
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 67;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 8;  
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 257;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 7;                  
                }
                break;
                case 11:// EVE
                {
                    sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 42;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 150;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 10;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 170;
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 10;
                }
                break;
                };
                sModbusDev[j-1].u8DevVendor = sModbusManager.sLIBManager[j-1].u8LIBType;
                sModbusDev[j-1].u32SlaveID = sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID;  
                //
                // Did parameters change?
                //
                if  (g_sParameters.sModbusParameters[j-1].u8DevVendor != sModbusDev[j-1].u8DevVendor)
                {
                  //
                  // Update the current parameters with the new settings.
                  //
                  g_sParameters.sModbusParameters[j-1].u8DevVendor = sModbusDev[j-1].u8DevVendor;
                  g_sParameters.sModbusParameters[j-1].u32SlaveID = sModbusDev[j-1].u32SlaveID;
                  //
                  // Yes - save these settings as the defaults.
                  //
                  g_sWorkingDefaultParameters.sModbusParameters[j-1].u8DevVendor = g_sParameters.sModbusParameters[j-1].u8DevVendor;
                  g_sWorkingDefaultParameters.sModbusParameters[j-1].u32SlaveID = g_sParameters.sModbusParameters[j-1].u32SlaveID;
                   
                  u8SaveConfigFlag |= 1;
                } 
                
            }
        }
        break;
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        {
          j = j - 7;
          if(strncmp(&data[sizeof("liionpack1i")-1],"v",1) == 0)
          {
              sModbusManager.sLIBManager[j-1].u8LIBType = temp;
              switch(sModbusManager.sLIBManager[j-1].u8LIBType)
              {
              case 13:// COSLIGHT_CF4850T
              case 1:// COSLIGHT
              {
                  sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                  sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;      // FC 04
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 58;      
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 120;//128;    // FC 02
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 38;//30; 
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 48;    // FC 03
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 4;
              }
              break;
              case 2:// COSLIGHT_OLD_V1.1
              {
                  sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                  sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 30;      // FC 04
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 24;      
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 128;    // FC 02
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 28; 
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 48;    // FC 03
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 2;
              }
              break;
              case 12:// SHOTO_SDA10_48100
              case 3:// SHOTO_2019
              {
                  sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                  sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0x1000;      // FC 04
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 10;      
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 0x2000;   
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 25; 
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 0x3040;   
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 56; 
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[3] = 0x800f;   
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[3] = 16; 
              }
              break;
              case 4:// HUAWEI
              {
                  sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 215;
                  sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = 215 + j;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 50; 
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 67;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 8;  
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 257;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 7;  
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[3] = 529;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[3] = 10; 
              }
              break;
              case 5:// M1Viettel50
              {
                    sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                    sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0; // FC:0x04
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 100;   
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 130;// FC:0x04
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 22;  //20;
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 0;// FC:0x03
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 80; 
                    sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[3] = 162;// FC:0x03
                    sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[3] = 3;//2                
              }
              break;
              case 14:// HUAFU_HF48100C
              case 6:// ZTT_2020
              {
                  sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                  sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 42;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 150;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 10;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 170;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 10;
              }
              break;
              case 9:// SAFT
              {
                  sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                  sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 23764;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 16;
              }
              break;
              case 10:// Narada75
              {
                  sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 38;
                  sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = 38 + j;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 4095;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 9;
                  
              }
              break;
              case 7:// ZTT50
              {
                  sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 38;
                  sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = 38 + j;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 4096;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 9;
              }
              break;
              case 8:// HUAWEI_A1
              {
                  sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 215;
                  sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = 215 + j;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 50; 
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 67;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 8;  
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 257;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 7;                  
              }
              break;
              case 11:// EVE
              {
                  sModbusManager.sLIBManager[j-1].u32LIBSlaveOffset = 0;
                  sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID = j;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[0] = 0;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[0] = 42;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[1] = 150;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[1] = 10;
                  sModbusManager.sLIBManager[j-1].u32LIBBaseAddr[2] = 170;
                  sModbusManager.sLIBManager[j-1].u8LIBNumOfReg[2] = 10;
              }
              break;
              };
              sModbusDev[j-1].u8DevVendor = sModbusManager.sLIBManager[j-1].u8LIBType;
              sModbusDev[j-1].u32SlaveID = sModbusManager.sLIBManager[j-1].u32LIBAbsSlaveID;  
              //
              // Did parameters change?
              //
              if  (g_sParameters.sModbusParameters[j-1].u8DevVendor != sModbusDev[j-1].u8DevVendor)
              {
                //
                // Update the current parameters with the new settings.
                //
                g_sParameters.sModbusParameters[j-1].u8DevVendor = sModbusDev[j-1].u8DevVendor;
                g_sParameters.sModbusParameters[j-1].u32SlaveID = sModbusDev[j-1].u32SlaveID;
                //
                // Yes - save these settings as the defaults.
                //
                g_sWorkingDefaultParameters.sModbusParameters[j-1].u8DevVendor = g_sParameters.sModbusParameters[j-1].u8DevVendor;
                g_sWorkingDefaultParameters.sModbusParameters[j-1].u32SlaveID = g_sParameters.sModbusParameters[j-1].u32SlaveID;
                 
                u8SaveConfigFlag |= 1;
              } 
              
          }
        }
        break;
        
        default:
            break;
        };

        //Return status code
        return error;
    }
    else if(strncmp(request, "/libTotal?value=", 8) == 0)
    {
      uint32_t temp=0;
        //
        // Rectifier walk-in Time Interval
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("libTotal"));
        if (temp != sModbusManager.u8NumberOfLIB)
        {
          sModbusManager.u8NumberOfLIB = temp;
          g_sParameters.u8DevNum[TOTAL_LIB_INDEX] = sModbusManager.u8NumberOfLIB;
          g_sWorkingDefaultParameters.u8DevNum[TOTAL_LIB_INDEX] = g_sParameters.u8DevNum[TOTAL_LIB_INDEX];
          u8SaveConfigFlag |= 1;
        }
//        setCmd_flag = 1;
//        setCmd_mask = SET_WIT_VAL;

        //Return status code
        return error;
    }
    //=============================================== Lithium Battery Setting ===================================================//
    //=============================================== Generator Setting ===================================================//
    else if(strncmp(request, "/gennum1i?value=", 7) == 0)
    {
        uint32_t i,j,temp=0;
        tModbusParameters sModbusDev[20];
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("gennum1i"));
        data = request;
        j = data[sizeof("gennum1i")-2]-0x30;
        switch (j)
        {
        case 1:
        case 2:
        {
            if(strncmp(&data[sizeof("gennum1i")-1],"v",1) == 0)
            {
                sModbusManager.sGenManager[j-1].u8GenType = temp;
                switch(sModbusManager.sGenManager[j-1].u8GenType)
                {
                case 1:// KUBOTA
                {
                    sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
                    sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 0;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 235;
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 85;    
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 1; 
                }
                break;
                case 2:// BE142
                {
                    sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
                    sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 30010;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 30135;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[2] = 30079;
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 38;    
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 4;     
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[2] = 27;
                }
                break;
                case 3:// DEEPSEA
                {
                    sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
                    sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 0x400;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 0x700;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[2] = 0x800;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[3] = 0x304;
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 34;    
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 8; 
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[2] = 8; 
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[3] = 1; 
                }
                break;
                case 4:// LR2057
                {
                    sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
                    sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 6;
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 19;  
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 42;
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 13; 
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[2] = 12;
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[2] = 66; 

                }
                break;
                case 5:// HIMOINSA
                {
                    sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 86;
                    sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 86+j;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 0;      // FC 0x02
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 23;  
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 7;      // FC 0x04
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 36; 

                }
                break;
                case 6:// QC315
                {
                    sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
                    sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 20;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 708;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[2] = 800;
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 61;  
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 6; 
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[2] = 7; 
                }
                break;
                case 7:// CUMMIN
                {
                    sModbusManager.sGenManager[j-1].u32GenSlaveOffset = 60;
                    sModbusManager.sGenManager[j-1].u32GenAbsSlaveID = 60+j;
                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[0] = 12;
//                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[1] = 708;
//                    sModbusManager.sGenManager[j-1].u32GenBaseAddr[2] = 800;
                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[0] = 60;  
//                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[1] = 6; 
//                    sModbusManager.sGenManager[j-1].u8GenNumOfReg[2] = 7; 
                }
                break;
                };
                sModbusDev[j-1].u8DevVendor = sModbusManager.sGenManager[j-1].u8GenType;
                sModbusDev[j-1].u32SlaveID = sModbusManager.sGenManager[j-1].u32GenAbsSlaveID;  
                //
                // Did parameters change?
                //
                if  (g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u8DevVendor != sModbusDev[j-1].u8DevVendor)
                {
                  //
                  // Update the current parameters with the new settings.
                  //
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u8DevVendor = sModbusDev[j-1].u8DevVendor;
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u32SlaveID = sModbusDev[j-1].u32SlaveID;
                  //
                  // Yes - save these settings as the defaults.
                  //
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u8DevVendor;
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB].u32SlaveID;
                   
                  u8SaveConfigFlag |= 1;
                }                 
            }
        }
        break;
        default:
            break;
        };

        //Return status code
        return error;
    }
    else if(strncmp(request, "/genTotal?value=", 8) == 0)
    {
      uint32_t temp=0;
        //
        // Rectifier walk-in Time Interval
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("genTotal"));
        if (temp != sModbusManager.u8NumberOfGen)
        {
          sModbusManager.u8NumberOfGen = temp;
          g_sParameters.u8DevNum[TOTAL_GEN_INDEX] = sModbusManager.u8NumberOfGen;
          g_sWorkingDefaultParameters.u8DevNum[TOTAL_GEN_INDEX] = g_sParameters.u8DevNum[TOTAL_GEN_INDEX];
          u8SaveConfigFlag |= 1;
        }
//        setCmd_flag = 1;
//        setCmd_mask = SET_WIT_VAL;

        //Return status code
        return error;
    }
    //=============================================== Generator Setting ===================================================//
#if (USERDEF_MONITOR_BM == ENABLED)
    //=============================================== BM Setting ===================================================//
    else if(strncmp(request, "/labmpack1i?value=", 9) == 0)
    {
        uint32_t i,j,temp=0;
        tModbusParameters sModbusDev[20];
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("labmpack1i"));
        data = request;
        j = data[sizeof("labmpack1i")-2]-0x30;
        switch (j)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        {
            if(strncmp(&data[sizeof("labmpack1i")-1],"v",1) == 0)
            {
                sModbusManager.sBMManager[j-1].u8BMType = temp;
                switch(sModbusManager.sBMManager[j-1].u8BMType)
                {
                case 1:// VIETTEL
                {
                    sModbusManager.sBMManager[j-1].u32BMSlaveOffset = 69;
                    sModbusManager.sBMManager[j-1].u32BMAbsSlaveID = 69+j;
                    sModbusManager.sBMManager[j-1].u32BMBaseAddr[0] = 0;
                    sModbusManager.sBMManager[j-1].u8BMNumOfReg[0] = 56;
                }
                break;
                
                case 2:// 
                {                                     
                }
                break;
                };
                sModbusDev[j-1].u8DevVendor = sModbusManager.sBMManager[j-1].u8BMType;
                sModbusDev[j-1].u32SlaveID = sModbusManager.sBMManager[j-1].u32BMAbsSlaveID;  
                //
                // Did parameters change?
                //
                if  (g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN].u8DevVendor != sModbusDev[j-1].u8DevVendor)
                {
                  //
                  // Update the current parameters with the new settings.
                  //
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN].u8DevVendor = sModbusDev[j-1].u8DevVendor;
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN].u32SlaveID = sModbusDev[j-1].u32SlaveID;
                  //
                  // Yes - save these settings as the defaults.
                  //
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN].u8DevVendor;
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN].u32SlaveID;
                   
                  u8SaveConfigFlag |= 1;
                }                 
            }
        }
        break;
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        {
          j = j -7;
          if(strncmp(&data[sizeof("labmpack1i")-1],"v",1) == 0)
          {
              sModbusManager.sBMManager[j-1].u8BMType = temp;
              switch(sModbusManager.sBMManager[j-1].u8BMType)
              {
              case 1:// VIETTEL
              {
                  sModbusManager.sBMManager[j-1].u32BMSlaveOffset = 69;
                  sModbusManager.sBMManager[j-1].u32BMAbsSlaveID = 69+j;
                  sModbusManager.sBMManager[j-1].u32BMBaseAddr[0] = 0;
                  sModbusManager.sBMManager[j-1].u8BMNumOfReg[0] = 56;
              }
              break;
              
              case 2:// 
              {                                     
              }
              break;
              };
              sModbusDev[j-1].u8DevVendor = sModbusManager.sBMManager[j-1].u8BMType;
              sModbusDev[j-1].u32SlaveID = sModbusManager.sBMManager[j-1].u32BMAbsSlaveID;  
              //
              // Did parameters change?
              //
              if  (g_sParameters.sModbusParameters[j-1+18].u8DevVendor != sModbusDev[j-1].u8DevVendor)
              {
                //
                // Update the current parameters with the new settings.
                //
                g_sParameters.sModbusParameters[j-1+18].u8DevVendor = sModbusDev[j-1].u8DevVendor;
                g_sParameters.sModbusParameters[j-1+18].u32SlaveID = sModbusDev[j-1].u32SlaveID;
                //
                // Yes - save these settings as the defaults.
                //
                g_sWorkingDefaultParameters.sModbusParameters[j-1+18].u8DevVendor = g_sParameters.sModbusParameters[j-1+18].u8DevVendor;
                g_sWorkingDefaultParameters.sModbusParameters[j-1+18].u32SlaveID = g_sParameters.sModbusParameters[j-1+18].u32SlaveID;
                 
                u8SaveConfigFlag |= 1;
              }                 
          }          
        }
        break;
        default:
            break;
        };

        //Return status code
        return error;
    }
    else if(strncmp(request, "/labmTotal?value=", 8) == 0)
    {
      uint32_t temp=0;
        //
        // Total number of LA BM
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("labmTotal"));
        if (temp != sModbusManager.u8NumberOfBM)
        {
          sModbusManager.u8NumberOfBM = temp;
          g_sParameters.u8DevNum[TOTAL_BM_INDEX] = sModbusManager.u8NumberOfBM;
          g_sWorkingDefaultParameters.u8DevNum[TOTAL_BM_INDEX] = g_sParameters.u8DevNum[TOTAL_BM_INDEX];
          u8SaveConfigFlag |= 1;
        }

        //Return status code
        return error;
    }
    //=============================================== BM Setting ===================================================//
#endif
#if (USERDEF_MONITOR_PM == ENABLED)
    //=============================================== PM Setting ===================================================//
    else if(strncmp(request, "/pmnum1i?value=", 6) == 0)
    {
        uint32_t i,j,temp=0;
        tModbusParameters sModbusDev[20];
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("pmnum1i"));
        data = request;
        j = data[sizeof("pmnum1i")-2]-0x30;     
        
        switch (j)
        {
        case 1:
        case 2:
        {
            if(strncmp(&data[sizeof("pmnum1i")-1],"v",1) == 0)
            {
                sModbusManager.sPMManager[j-1].u8PMType = temp;
                switch(sModbusManager.sPMManager[j-1].u8PMType)
                {
                case 1:// FINECO
                {
                    sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 16;
                    sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 16+j;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 16;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 50;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 0x700;    
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 2; 
                }
                break;
                
                case 2:// ASCENT
                {    
                    sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 16;
                    sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 16+j;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 0;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 100;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[2] = 106;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[3] = 118;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[4] = 142;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[5] = 144;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 2;    
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 2;    
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[2] = 2;   
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[3] = 2;   
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[4] = 2;   
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[5] = 2;                                 
                }
                break;
                
                case 3:// EASTRON
                {  

                    sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 16;
                    sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 16+j;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 0;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 32;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 70;//0x71
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 10;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[2] = 342;//0x156
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[2] = 4;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[3] = 64512;//0x156
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[3] = 2;
                }
                break;
                
                case 4:// CET1
                {  
                    sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 52;
                    sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 52+j;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 0;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 40;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[2] = 9800;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 14;    
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 18;    
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[2] = 27;                                  
                }
                break;
                
                case 5:// PILOT
                {  
                    sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 16;  
                    sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 16+j;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 0;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 17;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 19;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 6;                                  
                }
                break;
                case 15:// PILOT_3PHASE
                {  
                    sModbusManager.sPMManager[j-1].u32PMSlaveOffset = 16;  
                    sModbusManager.sPMManager[j-1].u32PMAbsSlaveID = 16+j;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[0] = 100;//0x03
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[0] = 43;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[1] = 1000;//0x03
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[1] = 12;  
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[2] = 9000;//0x03
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[2] = 2; 
                }
                break;
                case 16://YADA_3PHASE_DPC
                {
                    sModbusManager.sPMManager[j-1].u32PMSlaveOffset  = 16;
                    sModbusManager.sPMManager[j-1].u32PMAbsSlaveID   = 16+j;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[0]  = 4240;//0x03 U
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[0]   = 6;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[1]  = 4288;//0x03 I
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[1]   = 6;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[2]  = 4496;//0x03 cosphi
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[2]   = 4;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[3]  = 4528;//0x03 HZ
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[3]   = 1;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[4]  = 4192;//0x03 kvarh
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[4]   = 2;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[5]  = 4144;//0x03 kwh
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[5]   = 2;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[6]  = 4352;//0x03 P
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[6]   = 8;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[7]  = 4416;//0x03 Q
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[7]   = 8;
                }
                break;
                case 17://Schneider 2022
                {
                    sModbusManager.sPMManager[j-1].u32PMSlaveOffset  = 16;
                    sModbusManager.sPMManager[j-1].u32PMAbsSlaveID   = 16+j;
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[0]  = 29;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[0]   = 20; 
            
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[1]  = 128;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[1]   = 3;  
            
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[2]  = 2699;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[2]   = 24;
            
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[3]  = 2999;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[3]   = 6; 
            
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[4]  = 3027;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[4]   = 6;   
            
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[5]  = 3053;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[5]   = 24;   
         
            
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[6]  = 3077;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[6]   = 8;
            
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[7]  = 3109; // F
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[7]   = 2;                                    
                }
                break;     
                case 18://EASTRON SMD72D 2022
                {
                    sModbusManager.sPMManager[j-1].u32PMSlaveOffset  = 16;
                    sModbusManager.sPMManager[j-1].u32PMAbsSlaveID   = 16+j;
                    
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[0]  = 0;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[0]   = 30;
                    
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[1]  = 30;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[1]   = 30;                    
                    
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[2]  = 60;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[2]   = 30; 
                    
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[3]  = 342;
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[3]   = 30; 

                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[4]  = 10;      //Fn03
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[4]   = 30;               
                    
                    sModbusManager.sPMManager[j-1].u32PMBaseAddr[5]  = 64512;   //Fn03
                    sModbusManager.sPMManager[j-1].u8PMNumOfReg[5]   = 4;                         
                        
                }
                break;                
                };
                sModbusDev[j-1].u8DevVendor = sModbusManager.sPMManager[j-1].u8PMType;
                sModbusDev[j-1].u32SlaveID = sModbusManager.sPMManager[j-1].u32PMAbsSlaveID;  
                //
                // Did parameters change?
                //
                if  (g_sParameters.sModbusParameters[j-1+ MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u8DevVendor != sModbusDev[j-1].u8DevVendor)
                {
                  //
                  // Update the current parameters with the new settings.
                  //
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u8DevVendor = sModbusDev[j-1].u8DevVendor;
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u32SlaveID = sModbusDev[j-1].u32SlaveID;
                  //
                  // Yes - save these settings as the defaults.
                  //
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u8DevVendor;
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM].u32SlaveID;
                   
                  u8SaveConfigFlag |= 1;
                }                 
            }
        }
        break;
        default:
            break;
        };

        //Return status code
        return error;
    }
    else if(strncmp(request, "/pmTotal?value=", 7) == 0)
    {
      uint32_t temp=0;
        //
        // Rectifier walk-in Time Interval
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("pmTotal"));
        if (temp != sModbusManager.u8NumberOfPM)
        {
          sModbusManager.u8NumberOfPM = temp;
          g_sParameters.u8DevNum[TOTAL_PM_INDEX] = sModbusManager.u8NumberOfPM;
          g_sWorkingDefaultParameters.u8DevNum[TOTAL_PM_INDEX] = g_sParameters.u8DevNum[TOTAL_PM_INDEX];
          u8SaveConfigFlag |= 1;
        }
        //Return status code
        return error;
    }
    
    //=============================================== PM Setting ===================================================//
#endif
   
#if (USERDEF_MONITOR_VAC == ENABLED)
    //=============================================== VAC Setting ===================================================//
    else if(strncmp(request, "/vacnum1i?value=", 8) == 0)
    {
        uint32_t i,j,temp=0;
        tModbusParameters sModbusDev[20];
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("vacnum1i"));
        data = request;
        j = data[sizeof("vacnum1i")-2]-0x30;
        switch (j)
        {
        case 1:
        case 2:
        {
            if(strncmp(&data[sizeof("vacnum1i")-1],"v",1) == 0)
            {
                sModbusManager.sVACManager[j-1].u8VACType = temp;
                switch(sModbusManager.sVACManager[j-1].u8VACType)
                {
                case 1:// VIETTEL
                {
                    sModbusManager.sVACManager[j-1].u32VACSlaveOffset = 34;
                    sModbusManager.sVACManager[j-1].u32VACAbsSlaveID = 34+j;
                    sModbusManager.sVACManager[j-1].u32VACBaseAddr[0] = 0;
                    sModbusManager.sVACManager[j-1].u32VACBaseAddr[1] = 0;
                    sModbusManager.sVACManager[j-1].u32VACBaseAddr[2] = 80;
                    sModbusManager.sVACManager[j-1].u8VACNumOfReg[0] = 14;    
                    sModbusManager.sVACManager[j-1].u8VACNumOfReg[1] = 39;   
                    sModbusManager.sVACManager[j-1].u8VACNumOfReg[2] = 3;
                    
                }
                break;
                
                case 2:// 
                {                                     
                }
                break;
                };
                sModbusDev[j-1].u8DevVendor = sModbusManager.sVACManager[j-1].u8VACType;
                sModbusDev[j-1].u32SlaveID = sModbusManager.sVACManager[j-1].u32VACAbsSlaveID;  
                //
                // Did parameters change?
                //
                if  (g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u8DevVendor != sModbusDev[j-1].u8DevVendor)
                {
                  //
                  // Update the current parameters with the new settings.
                  //
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u8DevVendor = sModbusDev[j-1].u8DevVendor;
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u32SlaveID = sModbusDev[j-1].u32SlaveID;
                  //
                  // Yes - save these settings as the defaults.
                  //
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u8DevVendor;
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM].u32SlaveID;
                   
                  u8SaveConfigFlag |= 1;
                }                 
            }
        }
        break;
        default:
            break;
        };

        //Return status code
        return error;
    }
    else if(strncmp(request, "/vacTotal?value=", 8) == 0)
    {
      uint32_t temp=0;
        //
        // Rectifier walk-in Time Interval
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("vacTotal"));
        if (temp != sModbusManager.u8NumberOfVAC)
        {
          sModbusManager.u8NumberOfVAC = temp;
          g_sParameters.u8DevNum[TOTAL_VAC_INDEX] = sModbusManager.u8NumberOfVAC;
          g_sWorkingDefaultParameters.u8DevNum[TOTAL_VAC_INDEX] = g_sParameters.u8DevNum[TOTAL_VAC_INDEX];
          u8SaveConfigFlag |= 1;
        }
//        setCmd_flag = 1;
//        setCmd_mask = SET_WIT_VAL;

        //Return status code
        return error;
    }
    
    //=============================================== VAC Setting ===================================================//
#endif
#if (USERDEF_MONITOR_SMCB == ENABLED)
    //=============================================== SMCB Setting ===================================================//
    else if(strncmp(request, "/smcbnum1v?value=", 8) == 0)  // neu nhan vao set smcb thi se init tat ca gia tri cho tung loai smcb neu loai day dc chon
    {
        uint32_t i,j,temp=0;
        tModbusParameters sModbusDev[20];
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("smcbnum1v"));
        data = request;
        j = data[sizeof("smcbnum1v")-2]-0x30;
        switch (j)
        {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        {
            if(strncmp(&data[sizeof("smcbnum1i")-1],"v",1) == 0)
            {
                sModbusManager.sSMCBManager[j-1].u8SMCBType = temp;
                switch(sModbusManager.sSMCBManager[j-1].u8SMCBType)
                {
                case 1:// OPEN
                {
                    sModbusManager.sSMCBManager[j-1].u32SMCBSlaveOffset = 18;
                    sModbusManager.sSMCBManager[j-1].u32SMCBAbsSlaveID = 18+j;
                    sModbusManager.sSMCBManager[j-1].u32SMCBBaseAddr[0] = 0;
                    sModbusManager.sSMCBManager[j-1].u8SMCBNumOfReg[0] = 1;    
                }
                break;
                
                case 2:// MATIS
                {
                    sModbusManager.sSMCBManager[j-1].u32SMCBSlaveOffset = 18;
                    sModbusManager.sSMCBManager[j-1].u32SMCBAbsSlaveID = 18+j;
                    sModbusManager.sSMCBManager[j-1].u32SMCBBaseAddr[0] = 16;
                    sModbusManager.sSMCBManager[j-1].u8SMCBNumOfReg[0] = 1;    
                }
                break;
                case 3:// GOL
                {
                    sModbusManager.sSMCBManager[j-1].u32SMCBSlaveOffset = 18;
                    sModbusManager.sSMCBManager[j-1].u32SMCBAbsSlaveID = 18+j;
                    sModbusManager.sSMCBManager[j-1].u32SMCBBaseAddr[0] = 32768;
                    sModbusManager.sSMCBManager[j-1].u8SMCBNumOfReg[0] = 1;    
                }
                break;
                };
                sModbusDev[j-1].u8DevVendor = sModbusManager.sSMCBManager[j-1].u8SMCBType;
                sModbusDev[j-1].u32SlaveID = sModbusManager.sSMCBManager[j-1].u32SMCBAbsSlaveID;  
                //
                // Did parameters change?
                // luu vendor vao flash 
                if  (g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u8DevVendor != sModbusDev[j-1].u8DevVendor)
                {
                  //
                  // Update the current parameters with the new settings.
                  //
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u8DevVendor = sModbusDev[j-1].u8DevVendor;
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u32SlaveID = sModbusDev[j-1].u32SlaveID;
                  //
                  // Yes - save these settings as the defaults.
                  //
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u8DevVendor;
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC].u32SlaveID;
                   
                  u8SaveConfigFlag |= 1;
                }                 
            }
        }
        break;
        default:
            break;
        };

        //Return status code
        return error;
    }
    else if(strncmp(request, "/smcbTotal?value=", 9) == 0)
    {
      uint32_t temp=0;
        //
        // smcbTotal
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("smcbTotal"));
        if (temp != sModbusManager.u8NumberOfSMCB)
        {
          sModbusManager.u8NumberOfSMCB = temp;
          g_sParameters.u8DevNum[TOTAL_SMCB_INDEX] = sModbusManager.u8NumberOfSMCB;
          g_sWorkingDefaultParameters.u8DevNum[TOTAL_SMCB_INDEX] = g_sParameters.u8DevNum[TOTAL_SMCB_INDEX];
          u8SaveConfigFlag |= 1;
        }

        //Return status code
        return error;
    }
     else if(strncmp(request, "/smcbState1v?value=", 9) == 0)
    {
        //
        // SMCB State
        //
        uint32_t i,j,temp=0;
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("smcbState1v"));
        data = request;
        j = data[sizeof("smcbState1v")-2]-0x30;
        privateMibBase.smcbGroup.SmcbTable[j-1].SmcbState = temp;
        privateMibBase.smcbGroup.SmcbTable[j-1].SmcbStateWrite = temp;
        sSMCBInfo[j-1].u32State = privateMibBase.smcbGroup.SmcbTable[j-1].SmcbState;
        privateMibBase.smcbGroup.flag = j-1;
        sModbusManager.SettingCommand = SET_SMCB_STATE;

        //Return status code
        return error;
    }
    //=============================================== SMCB Setting ===================================================//
#endif        
#if (USERDEF_MONITOR_FUEL == ENABLED)
    //=============================================== FUEL Setting ===================================================//
    else if(strncmp(request, "/fuelnum1v?value=", 8) == 0)
    {
        uint32_t i,j,temp=0;
        tModbusParameters sModbusDev[20];
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("fuelnum1v"));
        data = request;
        j = data[sizeof("fuelnum1v")-2]-0x30;
        switch (j)
        {
        case 1:
        case 2:
        {
            if(strncmp(&data[sizeof("fuelnum1i")-1],"v",1) == 0)
            {
                sModbusManager.sFUELManager[j-1].u8FUELType = temp;
                switch(sModbusManager.sFUELManager[j-1].u8FUELType)
                {
                case 1:// HPT621
                {
                    sModbusManager.sFUELManager[j-1].u32FUELSlaveOffset = 23;
                    sModbusManager.sFUELManager[j-1].u32FUELAbsSlaveID = 23+j;
                    sModbusManager.sFUELManager[j-1].u32FUELBaseAddr[0] = 0;
                    sModbusManager.sFUELManager[j-1].u8FUELNumOfReg[0] = 1;    
                }
                break;
                };
                sModbusDev[j-1].u8DevVendor = sModbusManager.sFUELManager[j-1].u8FUELType;
                sModbusDev[j-1].u32SlaveID = sModbusManager.sFUELManager[j-1].u32FUELAbsSlaveID;  
                //
                // Did parameters change?
                // luu vendor vao flash 
                if  (g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u8DevVendor != sModbusDev[j-1].u8DevVendor)
                {
                  //
                  // Update the current parameters with the new settings.
                  //
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u8DevVendor = sModbusDev[j-1].u8DevVendor;
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u32SlaveID = sModbusDev[j-1].u32SlaveID;
                  //
                  // Yes - save these settings as the defaults.
                  //
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u8DevVendor;
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB+MAX_NUM_OF_GEN+MAX_NUM_OF_BM+MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB].u32SlaveID;
                   
                  u8SaveConfigFlag |= 1;
                }                 
            }
        }
        break;
        default:
            break;
        };

        //Return status code
        return error;
    }
    else if(strncmp(request, "/fuelTotal?value=", 9) == 0)
    {
      uint32_t temp=0;
        //
        // smcbTotal
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("fuelTotal"));
        if (temp != sModbusManager.u8NumberOfFUEL)
        {
          sModbusManager.u8NumberOfFUEL = temp;
          g_sParameters.u8DevNum[TOTAL_FUEL_INDEX] = sModbusManager.u8NumberOfFUEL;
          g_sWorkingDefaultParameters.u8DevNum[TOTAL_FUEL_INDEX] = g_sParameters.u8DevNum[TOTAL_FUEL_INDEX];
          u8SaveConfigFlag |= 1;
        }

        //Return status code
        return error;
    }
    //=============================================== SMCB Setting ===================================================//
#endif
     
#if (USERDEF_MONITOR_ISENSE == ENABLED)
    //=============================================== ISENSE Setting ===================================================//
    else if(strncmp(request, "/i_sensenum1i?value=", 11) == 0)
    {
        uint32_t i,j,temp=0;
        tModbusParameters sModbusDev[2];
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("i_sensenum1i"));
        data = request;
        j = data[sizeof("i_sensenum1i")-2]-0x30;     
        
        switch (j)
        {
        case 1:
        {
            if(strncmp(&data[sizeof("i_sensenum1i")-1],"v",1) == 0)
            {
                sModbusManager.sISENSEManager[j-1].u8ISENSEType = temp;
                switch(sModbusManager.sISENSEManager[j-1].u8ISENSEType)
                {
                case 1:// FORLONG
                {  
                    sModbusManager.sISENSEManager[j-1].u32ISENSESlaveOffset = 26;  
                    sModbusManager.sISENSEManager[j-1].u32ISENSEAbsSlaveID = 26+j;
                    sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[0] = 0;//0x04
                    sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[0] = 22;
                }
                break;
                //An 2022
                case 2:// IVY_DDS353H_2 
                {  
                    //thanhcm3 fix----------------------------------------------
                    sModbusManager.sISENSEManager[j-1].u32ISENSESlaveOffset  = 26;
                    sModbusManager.sISENSEManager[j-1].u32ISENSEAbsSlaveID   = 26+j;
                    
                    sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[0]  = 256;     
                    sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[0]   = 16;  
                    
                    sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[1]  = 290;    
                    sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[1]   = 36; 
                    
                    sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[2]  = 4096;     
                    sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[2]   = 6; 
//                    
//                    sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[3]  = 328;     // Q
//                    sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[3]   = 2;                
//                   
//                    sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[4]  = 336;     // S
//                    sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[4]   = 2;  
//                    
//                    sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[5]  = 344;     // Factor
//                    sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[5]   = 1; 
//
//                    sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[6]  = 40960;   // PE
//                    sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[6]   = 2;  
//                    
//                    sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[7]  = 40990;   // QE
//                    sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[7]   = 2;
//                    
//                    sModbusManager.sISENSEManager[j-1].u32ISENSEBaseAddr[8]  = 277;   //SN
//                    sModbusManager.sISENSEManager[j-1].u8ISENSENumOfReg[8]   = 3;
                    
                    
                    //thanhcm3 fix----------------------------------------------
                }
                break;
                };
                sModbusDev[j-1].u8DevVendor = sModbusManager.sISENSEManager[j-1].u8ISENSEType;
                sModbusDev[j-1].u32SlaveID = sModbusManager.sISENSEManager[j-1].u32ISENSEAbsSlaveID;  
                //
                // Did parameters change?
                //
                if  (g_sParameters.sModbusParameters[j-1+ MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor != sModbusDev[j-1].u8DevVendor)
                {
                  //
                  // Update the current parameters with the new settings.
                  //
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor = sModbusDev[j-1].u8DevVendor;
                  g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u32SlaveID = sModbusDev[j-1].u32SlaveID;
                  //
                  // Yes - save these settings as the defaults.
                  //
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u8DevVendor;
                  g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL].u32SlaveID;
                   
                  u8SaveConfigFlag |= 1;
                }                 
            }
        }
        break;
        default:
            break;
        };

        //Return status code
        return error;
    }
    else if(strncmp(request, "/i_senseTotal?value=", 11) == 0)
    {
      uint32_t temp=0;
        //
        // Rectifier walk-in Time Interval
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("i_senseTotal"));
        if (temp != sModbusManager.u8NumberOfISENSE)
        {
          sModbusManager.u8NumberOfISENSE = temp;
          g_sParameters.u8DevNum[TOTAL_ISENSE_INDEX] = sModbusManager.u8NumberOfISENSE;
          g_sWorkingDefaultParameters.u8DevNum[TOTAL_ISENSE_INDEX] = g_sParameters.u8DevNum[TOTAL_ISENSE_INDEX];
          u8SaveConfigFlag |= 1;
        }
        //Return status code
        return error;
    }
    
    //=============================================== ISENSE Setting ===================================================//
#endif
#if (USERDEF_MONITOR_PM_DC == ENABLED)
    //PM DC SETTING====================================================================================================
     else if(strncmp(request, "/pm_dcnum1i?value=", 9) == 0){
       uint32_t i,j,temp=0;
       tModbusParameters sModbusDev[2];
       u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("pm_dcnum1i"));
       data = request;
       j = data[sizeof("pm_dcnum1i")-2]-0x30;
       
       switch(j){
         case 1:
           if(strncmp(&data[sizeof("pm_dcnum1i")-1],"v",1) == 0){
             sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_type = temp;
             switch(sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_type){
             case 1:// YADA_DC
               {
                 sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_slave_offset    = 30;
                 sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_abs_slaveID     = 30 +j;
                 sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_base_addr[0]    = 0;       //0x03
                 sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_num_of_reg[0]    = 2;
                 sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_base_addr[1]    = 12;       //0x03
                 sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_num_of_reg[1]    = 2;
                 sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_base_addr[2]    = 26;       //0x03
                 sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_num_of_reg[2]    = 2;
                 sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_base_addr[3]    = 40;       //0x03
                 sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_num_of_reg[3]    = 2;
                 
               }
               break;
             default:
               break; 
             }
             sModbusDev[j-1].u8DevVendor = sModbusManager.s_pm_dc_manager[j-1].u8_pm_dc_type;
             sModbusDev[j-1].u32SlaveID = sModbusManager.s_pm_dc_manager[j-1].u32_pm_dc_abs_slaveID;  
             //
             // Did parameters change?
             //
             if  (g_sParameters.sModbusParameters[j-1+ MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u8DevVendor != sModbusDev[j-1].u8DevVendor)
             {
               //
               // Update the current parameters with the new settings.
               //
               g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u8DevVendor = sModbusDev[j-1].u8DevVendor;
               g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u32SlaveID = sModbusDev[j-1].u32SlaveID;
               //
               // Yes - save these settings as the defaults.
               //
               g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u8DevVendor = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u8DevVendor;
               g_sWorkingDefaultParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u32SlaveID = g_sParameters.sModbusParameters[j-1+MAX_NUM_OF_LIB + MAX_NUM_OF_GEN + MAX_NUM_OF_BM + MAX_NUM_OF_PM+MAX_NUM_OF_VAC+MAX_NUM_OF_SMCB + MAX_NUM_OF_FUEL+MAX_NUM_OF_ISENSE].u32SlaveID;
                   
               u8SaveConfigFlag |= 1;
             }
           }
           break;
         default:
           break;
       }
       //Return status code
       return error; 
    }
     else if(strncmp(request, "/pm_dcTotal?value=", 10) == 0){
       uint32_t temp=0;
        //
        // Rectifier walk-in Time Interval
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("pm_dcTotal"));
        if (temp != sModbusManager.u8_number_of_pm_dc)
        {
          sModbusManager.u8_number_of_pm_dc = temp;
          g_sParameters.u8DevNum[TOTAL_PM_DC_INDEX] = sModbusManager.u8_number_of_pm_dc;
          g_sWorkingDefaultParameters.u8DevNum[TOTAL_PM_DC_INDEX] = g_sParameters.u8DevNum[TOTAL_PM_DC_INDEX];
          u8SaveConfigFlag |= 1;
        }
        //Return status code
        return error;
     }
    
    //PM_DC_SETTING====================================================================================================
#endif
     else if(strncmp(request, "/alrmget", 7) == 0)
    {
        MESGState = HISTORYMESG_REQ;//SYSINFO_REQ;
        memset(&sHistoryInfo,0,sizeof(sHistoryInfo));
        getHistoryAlrm = 1;
        u16_mesgNo = 0;
        //Return status code
        return error;
    }
    else if(strncmp(request, "/fwUpdateDev?value=", 12) == 0)
    {
        //
        // LLVD Enable
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&configHandle.devType,sizeof("fwUpdateDev"));

        //Return status code
        return error;
    }
    else if(!strcasecmp(request, "/fwupdate_status"))
    {
        n = sprintf(buffer,"%d",
                    configHandle.devType
                   );
        //Format HTTP response header
        connection->response.version = connection->request.version;
        connection->response.statusCode = 200;
        connection->response.keepAlive = connection->request.keepAlive;
        connection->response.noCache = TRUE;
    connection->response.contentType = mimeGetType(".html");
        connection->response.chunkedEncoding = FALSE;
        connection->response.contentLength = n;

        //Send the header to the client
        error = httpWriteHeader(connection);
        //Any error to report?
        if(error) return error;

        //Send response body
        error = httpWriteStream(connection, buffer, n);
        //Any error to report?
        if(error) return error;

        //Properly close output stream
        error = httpCloseStream(connection);
        //Return status code
        return error;
    }
    else if(strncmp(request, "/update", 7) == 0)
    {
      if(u8FwUpdateCompleteFlag == 0)
      {
        //save Vac firm version--------------------------------------------------------------------
        if((configHandle.devType==_VAC_FW_)||(configHandle.devType==_VACV2_FW_))
        {
          configHandle.Check_vac_firm_version = privateMibBase.vacGroup.vacTable[0].vacFirmVersion;
        }else
        {
          configHandle.Check_vac_firm_version=0;
        }
        //flag update-------------------------------------------------------------------------------
        if(configHandle.devType==_NO_FW_UPDATE){
        }
        else{
          u8FwUpdateFlag=1;
        }
        //------------------------------------------------------------------------------------------
        
      }
        //Return status code
        return error;
    }
    else if(strncmp(request, "/reboot", 7) == 0)
    {
      u8IsRebootFlag = 1;
        //Return status code
        return error;
    }
    else if(strncmp(request, "/restore", 7) == 0)
    {
      ConfigLoadFactory();
      u8SaveConfigFlag |= 1;
      u8IsRebootFlag = 1;
      u8IsRewriteSN = 1;
        //Return status code
        return error;
    }
    else if(strncmp(request, "/cabCtrl1Type?value=", 8) == 0)
    {
        //
        // Battery End Time
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&privateMibBase.siteGroup.siteControllerModel[0],sizeof("cabCtrl1Type"));
        if (g_sParameters.sPort[0].u8ControllerType != privateMibBase.siteGroup.siteControllerModel[0])
        {
          //0: Emerson
          //1: ZTE
          //2: Agisson
          //3: Emerson_v21
          //4: DKD51_BDP
          //5: VERTIV_M830B
          g_sParameters.sPort[0].u8ControllerType = privateMibBase.siteGroup.siteControllerModel[0];
          g_sWorkingDefaultParameters.sPort[0].u8ControllerType = g_sParameters.sPort[0].u8ControllerType;  
          u8SaveConfigFlag |= 1;  
          u8IsRebootFlag = 1;          
        }

        //Return status code
        return error;
    }
    else if(strncmp(request, "/btsid?value=", 6) == 0)
    {
      uint16_t i=0;
        const char *pucBTSName;
        pucBTSName = request;
        pucBTSName += (sizeof("btsid") + 7);
        //
        // Charge BTS Name
        //
        privateMibBase.siteGroup.siteBTSCodeLen = htmlDecodeFormString(pucBTSName,(char *)&privateMibBase.siteGroup.siteBTSCode[0],40);

        strncpy((char*)&g_sWorkingDefaultParameters.siteName[0],(char*)&privateMibBase.siteGroup.siteBTSCode[0],privateMibBase.siteGroup.siteBTSCodeLen);
        strncpy((char*)&g_sParameters.siteName[0],(char*)&g_sWorkingDefaultParameters.siteName[0],privateMibBase.siteGroup.siteBTSCodeLen);
        privateMibBase.siteGroup.siteBTSCode[privateMibBase.siteGroup.siteBTSCodeLen] ='\0';
        g_sParameters.siteName[privateMibBase.siteGroup.siteBTSCodeLen] ='\0';
        g_sWorkingDefaultParameters.siteName[privateMibBase.siteGroup.siteBTSCodeLen] ='\0';
        
        u8SaveConfigFlag |= 1;
        //Return status code
        return error;
    }
    else if(strncmp(request, "/ftpname?value=", 6) == 0)
    {
      uint16_t i=0;
        const char *pucFTPServer;
        pucFTPServer = request;
        pucFTPServer += (sizeof("ftpname") + 7);
        //
        // Charge BTS Name
        //
        privateMibBase.cfgNetworkGroup.cFTPServerLen = htmlDecodeFormString(pucFTPServer,(char *)&privateMibBase.cfgNetworkGroup.cFTPServer[0],40);

        strncpy((char*)&g_sWorkingDefaultParameters.ucFTPServer[0],(char*)&privateMibBase.cfgNetworkGroup.cFTPServer[0],privateMibBase.cfgNetworkGroup.cFTPServerLen);
        strncpy((char*)&g_sParameters.ucFTPServer[0],(char*)&g_sWorkingDefaultParameters.ucFTPServer[0],privateMibBase.cfgNetworkGroup.cFTPServerLen);
        privateMibBase.cfgNetworkGroup.cFTPServer[privateMibBase.cfgNetworkGroup.cFTPServerLen] ='\0';
        g_sParameters.ucFTPServer[privateMibBase.cfgNetworkGroup.cFTPServerLen] ='\0';
        g_sWorkingDefaultParameters.ucFTPServer[privateMibBase.cfgNetworkGroup.cFTPServerLen] ='\0';
        
        u8SaveConfigFlag |= 1;
        //Return status code
        return error;
    }

    else if(strncmp(request, "/traptime?value=", 9) == 0)
    {
      tConfigParameters sConfigParams1;
        //
        // Trap Interval Value
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&sConfigParams1.u32TrapPeriod,sizeof("traptime"));
        //
        // Did parameters change?
        //
        if  (g_sParameters.u32TrapPeriod != sConfigParams1.u32TrapPeriod)
        {
          //
          // Update the current parameters with the new settings.
          //
          g_sParameters.u32TrapPeriod = sConfigParams1.u32TrapPeriod;
          privateMibBase.cfgNetworkGroup.u32TrapInterval = g_sParameters.u32TrapPeriod;
          //
          // Yes - save these settings as the defaults.
          //
          g_sWorkingDefaultParameters.u32TrapPeriod = g_sParameters.u32TrapPeriod;
           
          u8SaveConfigFlag |= 1;
        }        
        
        //Return status code
        return error;
    }
    else if(strncmp(request, "/trapmode?value=", 9) == 0)
    {
      tConfigParameters sConfigParams1;
        //
        // Trap Interval Value
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&sConfigParams1.u8TrapMode,sizeof("trapmode"));
        //
        // Did parameters change?
        //
        if  (g_sParameters.u8TrapMode != sConfigParams1.u8TrapMode)
        {
          //
          // Update the current parameters with the new settings.
          //
          g_sParameters.u8TrapMode = sConfigParams1.u8TrapMode;
          privateMibBase.cfgNetworkGroup.u8TrapMode = g_sParameters.u8TrapMode;
		  privateMibBase.siteGroup.siteTrapEnable = sConfigParams1.u8TrapMode;
          //
          // Yes - save these settings as the defaults.
          //
          g_sWorkingDefaultParameters.u8TrapMode = g_sParameters.u8TrapMode;
           
          u8SaveConfigFlag |= 1;
        }
        
        //Return status code
        return error;
    }
    else if(strncmp(request, "/telnetip1?value=", 9) == 0)
    {
      uint32_t temp=0;
        //
        // Trap Interval Value
        //
        u16TextBufferLen = htmlTextExtract(request,(uint32_t*)&temp,sizeof("telnetip1"));
        data = request;
        switch (data[sizeof("telnetip1")-1]-0x30)
        {
                case 1:	sConfigParams1.u32SnmpIP = (sConfigParams1.u32SnmpIP &  0x00ffffff) | (temp << 24);
                        break;
                case 2:	sConfigParams1.u32SnmpIP = (sConfigParams1.u32SnmpIP &  0xff00ffff) | (temp << 16);
                        break;
                case 3:	sConfigParams1.u32SnmpIP = (sConfigParams1.u32SnmpIP &  0xffff00ff) | (temp << 8);
                        break;
                case 4:	sConfigParams1.u32SnmpIP = (sConfigParams1.u32SnmpIP &  0xffffff00) | (temp << 0);
                        break;
                default:
                        break;
        }
        //
        // Did parameters change?
        //
        if  (g_sParameters.u32SnmpIP != sConfigParams1.u32SnmpIP)
        {
          //
          // Update the current parameters with the new settings.
          //
          g_sParameters.u32SnmpIP = sConfigParams1.u32SnmpIP;
          privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32SIP = revertIP(g_sParameters.u32SnmpIP);
          strcpy((char*)privateMibBase.cfgNetworkGroup.siteNetworkInfo.ucSIP,ipv4AddrToString((Ipv4Addr)privateMibBase.cfgNetworkGroup.siteNetworkInfo.u32SIP,0));//
          // Yes - save these settings as the defaults.
          //
          g_sWorkingDefaultParameters.u32SnmpIP = g_sParameters.u32SnmpIP;
           
          u8SaveConfigFlag |= 1;  
        }
              
        //Return status code
        return error;
    }
    else
    {
        //The requested resource cannot be found
        return ERROR_NOT_FOUND;
    }
}
#endif

#if (USERDEF_SERVER_HTTP == ENABLED)
void http_handler_init(void)
{
    error_t error;
    //Get default settings
    httpServerGetDefaultSettings(&httpServerSettings);
    //Bind HTTP server to the desired interface
    httpServerSettings.interface = &netInterface[0];
    //Listen to port 80
    httpServerSettings.port = HTTP_PORT;
    //Client connections
    httpServerSettings.maxConnections = APP_HTTP_MAX_CONNECTIONS;
    httpServerSettings.connections = httpConnections;
    //Specify the server's root directory
    strcpy(httpServerSettings.rootDirectory, "/www/");
    //Set default home page
    strcpy(httpServerSettings.defaultDocument, "login.html");
    //Callback functions
    httpServerSettings.cgiCallback = httpServerCgiCallback;
    httpServerSettings.uriNotFoundCallback = httpServerUriNotFoundCallback;
    httpServerSettings.requestCallback = httpServerRequestCallback;

    //HTTP server initialization
    error = httpServerInit(&httpServerContext, &httpServerSettings);
    //Failed to initialize HTTP server?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to initialize HTTP server!\r\n");
    }

    //Start HTTP server
    error = httpServerStart(&httpServerContext);
    //Failed to start HTTP server?
    if(error)
    {
        //Debug message
        TRACE_ERROR("Failed to start HTTP server!\r\n");
    }
}
#endif