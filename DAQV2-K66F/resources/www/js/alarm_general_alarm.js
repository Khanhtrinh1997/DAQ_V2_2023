/**
 * @author SonLQ4
 */

function ALRMGENStatus_get()
{
	var ALRMGENStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function ALRMGENStatusComplete()
	{
		if(ALRMGENStatus.readyState == 4)
		{
			if(ALRMGENStatus.status == 200)
			{
				text = ALRMGENStatus.responseText;
				length = ALRMGENStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}	
			
				vAlarmBattHiTemp = ALRMGENStatus.responseText.substring(0,tam[0]);			
				vAlarmLLVD = ALRMGENStatus.responseText.substring(tam[0]+1,tam[1]);
				vAlarmBLVD = ALRMGENStatus.responseText.substring(tam[1]+1,tam[2]);
				vAlarmDCLow = ALRMGENStatus.responseText.substring(tam[2]+1,tam[3]);		
				vAlarmACFail = ALRMGENStatus.responseText.substring(tam[3]+1,tam[4]);		
				vAlarmBattCB = ALRMGENStatus.responseText.substring(tam[4]+1,tam[5]);
				vAlarmLoadCB = ALRMGENStatus.responseText.substring(tam[5]+1,tam[6]);	
				vAlarmRectACFault = ALRMGENStatus.responseText.substring(tam[6]+1,tam[7]);	
				vAlarmRectNoResp = ALRMGENStatus.responseText.substring(tam[7]+1,tam[8]);	
				vAlarmManMode = ALRMGENStatus.responseText.substring(tam[8]+1,tam[9]);
				vAlarmAcThres = ALRMGENStatus.responseText.substring(tam[9]+1,tam[10]);	
				vAlarmSmoke = ALRMGENStatus.responseText.substring(tam[10]+1,tam[11]);
				vAlarmDoorOpen = ALRMGENStatus.responseText.substring(tam[11]+1,tam[12]);	
				
				if (vAlarmBattHiTemp == 0) {
					setRowText('idalarm_BattHiTemp', "Normal");
				} else if(vAlarmBattHiTemp == 2){
					setRowText('idalarm_BattHiTemp', "Alarm (High Temp)");
				} else if(vAlarmBattHiTemp == 32){
					setRowText('idalarm_BattHiTemp', "Alarm (No Config)");
				} else if(vAlarmBattHiTemp == 5){
					setRowText('idalarm_BattHiTemp', "Alarm (No Sensor)");
				}else if(vAlarmBattHiTemp == 7){
				setRowText('idalarm_BattHiTemp', "Alarm (Sensor Fault)");
				}								
				if (vAlarmLLVD == 0) {
					setRowText('idalarm_LLVD', "Normal");
				} else{
					setRowText('idalarm_LLVD', "Alarm");
				}
				if (vAlarmBLVD == 0) {
					setRowText('idalarm_BLVD', "Normal");
				} else{
					setRowText('idalarm_BLVD', "Alarm");
				}
				if (vAlarmDCLow == 0) {
					setRowText('idalarm_DCLow', "Normal");
				} else{
					setRowText('idalarm_DCLow', "Alarm");
				}	
				if (vAlarmACFail == 0) {
					setRowText('idalarm_ACFail', "Normal");
				} else{
					setRowText('idalarm_ACFail', "Alarm");
				}				
				if (vAlarmBattCB == 0) {
					setRowText('idalarm_BattCB', "Normal");
				} else{
					setRowText('idalarm_BattCB', "Alarm");
				}
				if (vAlarmLoadCB == 0) {
					setRowText('idalarm_LoadCB', "Normal");
				} else{
					setRowText('idalarm_LoadCB', "Alarm");
				}
				if (vAlarmRectACFault == 0) {
					setRowText('idalarm_RectACFault', "Normal");
				} else{
					setRowText('idalarm_RectACFault', "Alarm");
				}	
				if (vAlarmRectNoResp == 0) {
					setRowText('idalarm_RectNoResp', "Normal");
				} else{
					setRowText('idalarm_RectNoResp', "Alarm");
				}
				if (vAlarmManMode == 225) {
					setRowText('idalarm_ManMode', "Alarm");
				} else{
					setRowText('idalarm_ManMode', "Normal");
				}	
				if (vAlarmAcThres == 0) {
					setRowText('idalarm_ACThres', "Normal");
				} else{
					setRowText('idalarm_ACThres', "Alarm");
				}	
				if (vAlarmSmoke == 0) {
					setRowText('idalarm_Smoke', "Normal");
				} else{
					setRowText('idalarm_Smoke', "Alarm");
				}	
				if (vAlarmDoorOpen == 0) {
					setRowText('idalarm_DoorOpen', "Normal");
				} else{
					setRowText('idalarm_DoorOpen', "Alarm");
				}				
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		ALRMGENStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		ALRMGENStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(ALRMGENStatus)
	{
		ALRMGENStatus.open("GET","/alarmGen_status?=", true);
		ALRMGENStatus.onreadystatechange = ALRMGENStatusComplete;
		ALRMGENStatus.send(null);
	}
t=setTimeout("ALRMGENStatus_get()",5000);
}

function LoadFrame() {
	ALRMGENStatus_get();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

