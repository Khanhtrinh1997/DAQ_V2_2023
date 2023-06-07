/**
 * @author SonLQ4
 */

function PMStatus_get()
{
	var PMStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function PMStatusComplete()
	{
		if(PMStatus.readyState == 4)
		{
			if(PMStatus.status == 200)
			{
				text = PMStatus.responseText;
				length = PMStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}	
			
				vPMConnectStatus      = PMStatus.responseText.substring(0       ,tam[0]);
				vPMInstalledPM        = PMStatus.responseText.substring(tam[0]+1,tam[1]);			
				vPMRunningPM          = PMStatus.responseText.substring(tam[1]+1,tam[2]);
				vPMModel              = PMStatus.responseText.substring(tam[2]+1,tam[3]);
				vPMVoltage            = PMStatus.responseText.substring(tam[3]+1,tam[4]);		
				vPMCurrent            = PMStatus.responseText.substring(tam[4]+1,tam[5]);	
				vPMActivePower        = PMStatus.responseText.substring(tam[5]+1,tam[6]);	
                vPMActiveEnergy       = PMStatus.responseText.substring(tam[6]+1,tam[7]);

				vPMVoltage            = parseFloat(vPMVoltage/100).toFixed(2);			
				vPMCurrent            = parseFloat(vPMCurrent/100).toFixed(2);		
				vPMActivePower        = parseFloat(vPMActivePower/100).toFixed(2);	
				vPMActiveEnergy       = parseFloat(vPMActiveEnergy/1000).toFixed(2);
							
				
				
				if (vPMConnectStatus == 170) {
					setRowText('idpmdc_Connect', "Disconnected");
				} else if (vPMConnectStatus == 255) {
					setRowText('idpmdc_Connect', "Connected");
				} else if (vPMConnectStatus == 165) {
					setRowText('idpmdc_Connect', "Warning");
				} else {	
					setRowText('idpmdc_Connect', "Unavailable");
				}				
				
	
				
				setRowText('idpmdc_InstalledPM', vPMInstalledPM);
				setRowText('idpmdc_RunningPM', vPMRunningPM);
				//setRowText('idpm_SerialNumber', vPMSerialNumber);
				setRowText('idpmdc_Voltage', vPMVoltage);
				//setRowText('idpm_Freq', vPMFreq);
				setRowText('idpmdc_Model',vPMModel);
				setRowText('idpmdc_current',vPMCurrent);
				setRowText('idpmdc_AcPo',vPMActivePower);
				setRowText('idpmdc_AcEn',vPMActiveEnergy);

			}
		}
	}

	if(window.XMLHttpRequest)
	{
		PMStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		PMStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(PMStatus)
	{
		PMStatus.open("GET","/pm_dcInfo_status?=", true);
		PMStatus.onreadystatechange = PMStatusComplete;
		PMStatus.send(null);
	}
t=setTimeout("PMStatus_get()",5000);
}

function LoadFrame() {
	PMStatus_get();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

