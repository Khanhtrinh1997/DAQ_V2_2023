/**
 * @author SonLQ4
 */

function BATTStatus_get()
{
	var BATTStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function BATTStatusComplete()
	{
		if(BATTStatus.readyState == 4)
		{
			if(BATTStatus.status == 200)
			{
				text = BATTStatus.responseText;
				length = BATTStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}	
			
				vBattVolt = BATTStatus.responseText.substring(0,tam[0]);			
				vBattCurr = BATTStatus.responseText.substring(tam[0]+1,tam[1]);
				vBattTemp = BATTStatus.responseText.substring(tam[1]+1,tam[2]);
				vBattNum = BATTStatus.responseText.substring(tam[2]+1,tam[3]);		
				vBattCB1 = BATTStatus.responseText.substring(tam[3]+1,tam[4]);		
				vBattCB2 = BATTStatus.responseText.substring(tam[4]+1,tam[5]);
				vBattCB3 = BATTStatus.responseText.substring(tam[5]+1,tam[6]);	
				vBattCB4 = BATTStatus.responseText.substring(tam[6]+1,tam[7]);	
				vBatt1RmnCap = BATTStatus.responseText.substring(tam[7]+1,tam[8]);	
				vBatt2RmnCap = BATTStatus.responseText.substring(tam[8]+1,tam[9]);
				vBatt3RmnCap = BATTStatus.responseText.substring(tam[9]+1,tam[10]);
				vBatt4RmnCap = BATTStatus.responseText.substring(tam[10]+1,tam[11]);	

				vBattVolt = parseFloat(vBattVolt/100).toFixed(2);		
				vBattCurr = parseFloat(vBattCurr/100).toFixed(2);		
				vBattTemp = parseFloat(vBattTemp/10).toFixed(1);	
				vBatt1RmnCap = parseFloat(vBatt1RmnCap/100).toFixed(2);		
				vBatt2RmnCap = parseFloat(vBatt2RmnCap/100).toFixed(2);				
				vBatt3RmnCap = parseFloat(vBatt3RmnCap/100).toFixed(2);
				vBatt4RmnCap = parseFloat(vBatt4RmnCap/100).toFixed(2);

				setRowText('idbatt_Volt', vBattVolt);
				setRowText('idbatt_Curr', vBattCurr);
				setRowText('idbatt_Temp', vBattTemp);
				setRowText('idbatt_Num', vBattNum);
				setRowText('idbatt_1rmnCap', vBatt1RmnCap);
				setRowText('idbatt_2rmnCap', vBatt2RmnCap);
				setRowText('idbatt_3rmnCap', vBatt3RmnCap);
				setRowText('idbatt_4rmnCap', vBatt4RmnCap);
				
				if (vBattCB1 == 0) {
					setRowText('idbatt_CB1', "Normal");
				} else{
					setRowText('idbatt_CB1', "Alarm");
				}
				
				if (vBattCB2 == 0) {
					setRowText('idbatt_CB2', "Normal");
				} else{
					setRowText('idbatt_CB2', "Alarm");
				}
				if (vBattCB3 == 0) {
					setRowText('idbatt_CB3', "Normal");
				} else{
					setRowText('idbatt_CB3', "Alarm");
				}
				if (vBattCB4 == 0) {
					setRowText('idbatt_CB4', "Normal");
				} else{
					setRowText('idbatt_CB4', "Alarm");
				}				
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		BATTStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		BATTStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(BATTStatus)
	{
		BATTStatus.open("GET","/batt_status?=", true);
		BATTStatus.onreadystatechange = BATTStatusComplete;
		BATTStatus.send(null);
	}
t=setTimeout("BATTStatus_get()",5000);
}

function LoadFrame() {
	BATTStatus_get();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

