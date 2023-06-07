/**
 * @author SonLQ4
 */

function VACStatus_get()
{
	var VACStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function VACStatusComplete()
	{
		if(VACStatus.readyState == 4)
		{
			if(VACStatus.status == 200)
			{
				text = VACStatus.responseText;
				length = VACStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}	
			
				vVACInstalledFan = VACStatus.responseText.substring(0,tam[0]);			
				vVACRunningFan = VACStatus.responseText.substring(tam[0]+1,tam[1]);
				vVACModel = VACStatus.responseText.substring(tam[1]+1,tam[2]);
				vVACSysMode = VACStatus.responseText.substring(tam[2]+1,tam[3]);		
				vVACFanMode = VACStatus.responseText.substring(tam[3]+1,tam[4]);		
				vVACInTemp = VACStatus.responseText.substring(tam[4]+1,tam[5]);
				vVACExTemp = VACStatus.responseText.substring(tam[5]+1,tam[6]);	
				vVACFan1Speed = VACStatus.responseText.substring(tam[6]+1,tam[7]);	
				vVACFan1Speed2 = VACStatus.responseText.substring(tam[7]+1,tam[8]);	
				vVACAlarmCode = VACStatus.responseText.substring(tam[8]+1,tam[9]);	
				vVACConnectStatus = VACStatus.responseText.substring(tam[9]+1,tam[10]);	
				vVACSerialNum = VACStatus.responseText.substring(tam[10]+1,tam[11]);
				vVACSWVersion = VACStatus.responseText.substring(tam[11]+1,tam[12]);		
		
				vVACInTemp = parseFloat(vVACInTemp/10).toFixed(1);	
				vVACExTemp = parseFloat(vVACExTemp/10).toFixed(1);	
				vVACSWVersion = parseFloat(vVACSWVersion/100).toFixed(2);	
				
				if (vVACConnectStatus == 170) {
					setRowText('idvac_Connect', "Disconnected");
				} else if (vVACConnectStatus == 255) {
					setRowText('idvac_Connect', "Connected");
				} else if (vVACConnectStatus == 165) {
					setRowText('idvac_Connect', "Warning");
				} else {	
					setRowText('idvac_Connect', "Unavailable");
				}
				
				setRowText('idvac_InstalledFan', vVACInstalledFan);
				setRowText('idvac_RunningFan', vVACRunningFan);
				setRowText('idvac_InTemp', vVACInTemp);
				setRowText('idvac_ExTemp', vVACExTemp);
				setRowText('idvac_Fan1Speed', vVACFan1Speed);
				setRowText('idvac_Fan1Speed2', vVACFan1Speed2);
				setRowText('idvac_AlarmStatus', vVACAlarmCode);
				setRowText('idvac_Serial', vVACSerialNum);
				setRowText('idvac_SWVersion', vVACSWVersion);
				
				if (vVACSysMode == 0) {
					setRowText('idvac_Sysmode', "Free Air Cooling");
				} else if (vVACSysMode == 1) {
					setRowText('idvac_Sysmode', "Hybrid Mode");
				} else if (vVACSysMode == 2) {
					setRowText('idvac_Sysmode', "A/C Only");
				}
				
				if (vVACModel == 0) {
					setRowText('idvac_Model', "Unavailable");
				} else if (vVACModel == 1) {
					setRowText('idvac_Model', "VAC03");
				}
				
				if (vVACFanMode == 0) {
					setRowText('idvac_Fanmode', "Variable Speed");
				} else{
					setRowText('idvac_Fanmode', "Max Speed");
				}				
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		VACStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		VACStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(VACStatus)
	{
		VACStatus.open("GET","/vacInfo_status?=", true);
		VACStatus.onreadystatechange = VACStatusComplete;
		VACStatus.send(null);
	}
t=setTimeout("VACStatus_get()",5000);
}

function LoadFrame() {
	VACStatus_get();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

