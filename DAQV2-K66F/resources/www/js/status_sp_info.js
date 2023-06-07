/**
 * @author SonLQ4
 */

function SPStatus_get()
{
	var SPStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function SPStatusComplete()
	{
		if(SPStatus.readyState == 4)
		{
			if(SPStatus.status == 200)
			{
				text = SPStatus.responseText;
				length = SPStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}				
				
				vSpBaudrate = SPStatus.responseText.substring(0,tam[0]);			
				vSpDataSize = SPStatus.responseText.substring(tam[0]+1,tam[1]);
				vSpParity = SPStatus.responseText.substring(tam[1]+1,tam[2]);		
				vSpStopBits = SPStatus.responseText.substring(tam[2]+1,tam[3]);				
								
				setRowText('idsp_BaudRate', vSpBaudrate);
				setRowText('idsp_DataSize', vSpDataSize);
				
				if (vSpParity == 1) { setRowText('idsp_Parity', 'NONE'); }
				else if (vSpParity == 2) { setRowText('idsp_Parity', 'ODD'); }
				else if (vSpParity == 3) { setRowText('idsp_Parity', 'EVEN'); }
				else if (vSpParity == 4) { setRowText('idsp_Parity', 'MARK'); }
				else if (vSpParity == 5) { setRowText('idsp_Parity', 'SPACE'); }				
				
				setRowText('idsp_StopBits', vSpStopBits);
								
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		SPStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		SPStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(SPStatus)
	{
		SPStatus.open("GET","/sp_status?=", true);
		SPStatus.onreadystatechange = SPStatusComplete;
		SPStatus.send(null);
	}
t=setTimeout("SPStatus_get()",5000);
}

function LoadFrame() {
	SPStatus_get();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();
	var numWifiAddresses = getNumWifiAddresses();
	var numEthernetAddresses = getNumEthernetAddresses();
	var endIpValue = IpDot2num(endIP);
	var startIpValue = IpDot2num(startIP);
	var totalIp = endIpValue - startIpValue + 1;
	/* phuclt1 (7/8/2015) refs #2950: Add a variable for reserved IP */
	var reservedIP = calReserIP(startIpValue, endIpValue);
	/* phuclt1 (7/8/2015) refs #2950: calculate remain ip in new way */
	var remainIp = totalIp - numWifiAddresses - numEthernetAddresses - reservedIP;
	if(enblDhcpSrv == 0) {
		totalIp = 0;
		numEthernetAddresses = 0;
		numWifiAddresses = 0;
		remainIp = 0;
	}	
}

