/**
 * @author SonLQ4
 */
function DEVINFOStatus_get()
{
	var DEVINFOStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function DEVINFOStatusComplete()
	{
		if(DEVINFOStatus.readyState == 4)
		{
			if(DEVINFOStatus.status == 200)
			{
				text = DEVINFOStatus.responseText;
				length = DEVINFOStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}						
			
				vDevType = DEVINFOStatus.responseText.substring(0,tam[0]);			
				vDevDesc = DEVINFOStatus.responseText.substring(tam[0]+1,tam[1]);
				vDevSN = DEVINFOStatus.responseText.substring(tam[1]+1,tam[2]);		
				vDevHWvers = DEVINFOStatus.responseText.substring(tam[2]+1,tam[3]);
				vDevSWvers = DEVINFOStatus.responseText.substring(tam[3]+1,tam[4]);
				vDevManu = DEVINFOStatus.responseText.substring(tam[4]+1,tam[5]);			
				vDevMAC = DEVINFOStatus.responseText.substring(tam[5]+1,tam[6]);
				
				setRowText('iddev_Type', vDevType);
				setRowText('iddev_Des', vDevDesc);
				setRowText('iddev_SN', vDevSN);
				setRowText('iddev_HW', vDevHWvers);
				setRowText('iddev_SW', vDevSWvers);
				setRowText('iddev_MANU', vDevManu);
				setRowText('iddev_MAC', vDevMAC);
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		DEVINFOStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		DEVINFOStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(DEVINFOStatus)
	{
		DEVINFOStatus.open("GET","/dev_status?=", true);
		DEVINFOStatus.onreadystatechange = DEVINFOStatusComplete;
		DEVINFOStatus.send(null);
	}
t=setTimeout("DEVINFOStatus_get()",5000);
}

function LoadFrame() {

	DEVINFOStatus_get()
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
	
	
//thinhtn - Refs #4415 - Get CPU internal temperature - show CPU temperature
	setRowText('td9_2', cpu_tempa +"." + cpu_tempb + " &#8451;");
	setRowText('td10_2', cpu_usage + "%");
	setRowText('td11_2', mem_usage + "%");
}
