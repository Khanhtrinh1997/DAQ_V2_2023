/**
 * @author SonLQ4
 */

function ALRMDIStatus_get()
{
	var ALRMDIStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function ALRMDIStatusComplete()
	{
		if(ALRMDIStatus.readyState == 4)
		{
			if(ALRMDIStatus.status == 200)
			{
				text = ALRMDIStatus.responseText;
				length = ALRMDIStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}	
			
				vAlarmDI[0] = ALRMDIStatus.responseText.substring(0,tam[0]);			
				vAlarmDI[1] = ALRMDIStatus.responseText.substring(tam[0]+1,tam[1]);
				vAlarmDI[2] = ALRMDIStatus.responseText.substring(tam[1]+1,tam[2]);
				vAlarmDI[3] = ALRMDIStatus.responseText.substring(tam[2]+1,tam[3]);		
				vAlarmDI[4] = ALRMDIStatus.responseText.substring(tam[3]+1,tam[4]);		
				vAlarmDI[5] = ALRMDIStatus.responseText.substring(tam[4]+1,tam[5]);
				vAlarmDI[6] = ALRMDIStatus.responseText.substring(tam[5]+1,tam[6]);	
				vAlarmDI[7] = ALRMDIStatus.responseText.substring(tam[6]+1,tam[7]);							
						
				
				if (vAlarmDI[0] == 0) {
					setRowText('idalarm_DI1', "Normal");
				} else{
					setRowText('idalarm_DI1', "Alarm");
				}				
				if (vAlarmDI[1] == 0) {
					setRowText('idalarm_DI2', "Normal");
				} else{
					setRowText('idalarm_DI2', "Alarm");
				}
				if (vAlarmDI[2] == 0) {
					setRowText('idalarm_DI3', "Normal");
				} else{
					setRowText('idalarm_DI3', "Alarm");
				}
				if (vAlarmDI[3] == 0) {
					setRowText('idalarm_DI4', "Normal");
				} else{
					setRowText('idalarm_DI4', "Alarm");
				}	
				if (vAlarmDI[4] == 0) {
					setRowText('idalarm_DI5', "Normal");
				} else{
					setRowText('idalarm_DI5', "Alarm");
				}				
				if (vAlarmDI[5] == 0) {
					setRowText('idalarm_DI6', "Normal");
				} else{
					setRowText('idalarm_DI6', "Alarm");
				}
				if (vAlarmDI[6] == 0) {
					setRowText('idalarm_DI7', "Normal");
				} else{
					setRowText('idalarm_DI7', "Alarm");
				}
				if (vAlarmDI[7] == 0) {
					setRowText('idalarm_DI8', "Normal");
				} else{
					setRowText('idalarm_DI8', "Alarm");
				}			
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		ALRMDIStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		ALRMDIStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(ALRMDIStatus)
	{
		ALRMDIStatus.open("GET","/alarmDI_status?=", true);
		ALRMDIStatus.onreadystatechange = ALRMDIStatusComplete;
		ALRMDIStatus.send(null);
	}
t=setTimeout("ALRMDIStatus_get()",5000);
}

function LoadFrame() {
	ALRMDIStatus_get();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

