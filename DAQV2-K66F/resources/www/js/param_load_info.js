/**
 * @author SonLQ4
 */

function LOADStatus_get()
{
	var LOADStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;
	
	var vLoadCurr = 0;
	var vLoadCB1 = 0;
	var vLoadCB2 = 0;
	var vLoadCB3 = 0;
	var vLoadCB4 = 0;
	var vLoadCB5 = 0;
	var vLoadCB6 = 0;
	var vLoadCB7 = 0;
	var vLoadCB8 = 0;
	var vLoadCB9 = 0;
	var vLoadCB10 = 0;

	function LOADStatusComplete()
	{
		if(LOADStatus.readyState == 4)
		{
			if(LOADStatus.status == 200)
			{
				text = LOADStatus.responseText;
				length = LOADStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}		
				
				vLoadCurr = LOADStatus.responseText.substring(0,tam[0]);
				vLoadCB1 = LOADStatus.responseText.substring(tam[0]+1,tam[1]);
				vLoadCB2 = LOADStatus.responseText.substring(tam[1]+1,tam[2]);	
				vLoadCB3 = LOADStatus.responseText.substring(tam[2]+1,tam[3]);
				vLoadCB4 = LOADStatus.responseText.substring(tam[3]+1,tam[4]);	
				vLoadCB5 = LOADStatus.responseText.substring(tam[4]+1,tam[5]);
				vLoadCB6 = LOADStatus.responseText.substring(tam[5]+1,tam[6]);	
				vLoadCB7 = LOADStatus.responseText.substring(tam[6]+1,tam[7]);
				vLoadCB8 = LOADStatus.responseText.substring(tam[7]+1,tam[8]);					
				vLoadCB9 = LOADStatus.responseText.substring(tam[8]+1,tam[9]);
				vLoadCB10 = LOADStatus.responseText.substring(tam[9]+1,tam[10]);	

				vLoadCurr = parseFloat(vLoadCurr/100).toFixed(2);				
				
				setRowText('idload_Curr', vLoadCurr);
				
				if (vLoadCB1 == 0) {
					setRowText('idload_CB1', "Normal");
				} else{
					setRowText('idload_CB1', "Alarm");
				}
				
				if (vLoadCB2 == 0) {
					setRowText('idload_CB2', "Normal");
				} else{
					setRowText('idload_CB2', "Alarm");
				}
				
				if (vLoadCB3 == 0) {
					setRowText('idload_CB3', "Normal");
				} else{
					setRowText('idload_CB3', "Alarm");
				}
				
				if (vLoadCB4 == 0) {
					setRowText('idload_CB4', "Normal");
				} else{
					setRowText('idload_CB4', "Alarm");
				}
				
				if (vLoadCB5 == 0) {
					setRowText('idload_CB5', "Normal");
				} else{
					setRowText('idload_CB5', "Alarm");
				}
				
				if (vLoadCB6 == 0) {
					setRowText('idload_CB6', "Normal");
				} else{
					setRowText('idload_CB6', "Alarm");
				}
				
				if (vLoadCB7 == 0) {
					setRowText('idload_CB7', "Normal");
				} else{
					setRowText('idload_CB7', "Alarm");
				}
				
				if (vLoadCB8 == 0) {
					setRowText('idload_CB8', "Normal");
				} else{
					setRowText('idload_CB8', "Alarm");
				}
				
				if (vLoadCB9 == 0) {
					setRowText('idload_CB9', "Normal");
				} else{
					setRowText('idload_CB9', "Alarm");
				}
				
				if (vLoadCB10 == 0) {
					setRowText('idload_CB10', "Normal");
				} else{
					setRowText('idload_CB10', "Alarm");
				}					
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		LOADStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		LOADStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(LOADStatus)
	{
		LOADStatus.open("GET","/load_status?=", true);
		LOADStatus.onreadystatechange = LOADStatusComplete;
		LOADStatus.send(null);
	}
t=setTimeout("LOADStatus_get()",5000);
}

function LoadFrame() {
	LOADStatus_get();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

