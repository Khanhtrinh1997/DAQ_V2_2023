/**
 * @author SonLQ4
 */

function ACStatus_get()
{
	var ACStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function ACStatusComplete()
	{
		if(ACStatus.readyState == 4)
		{
			if(ACStatus.status == 200)
			{
				text = ACStatus.responseText;
				length = ACStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}				
			
				vAcPhase1 = ACStatus.responseText.substring(0,tam[0]);			
				vAcPhase2 = ACStatus.responseText.substring(tam[0]+1,tam[1]);
				vAcPhase3 = ACStatus.responseText.substring(tam[1]+1,tam[2]);	

				vAcPhase1 = parseFloat(vAcPhase1/100).toFixed(2);						
				vAcPhase2 = parseFloat(vAcPhase2/100).toFixed(2);
				vAcPhase3 = parseFloat(vAcPhase3/100).toFixed(2);
				
				setRowText('idac_phase1', vAcPhase1);
				setRowText('idac_phase2', vAcPhase2);
				setRowText('idac_phase3', vAcPhase3);
								
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		ACStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		ACStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(ACStatus)
	{
		ACStatus.open("GET","/ac_status?=", true);
		ACStatus.onreadystatechange = ACStatusComplete;
		ACStatus.send(null);
	}
t=setTimeout("ACStatus_get()",5000);
}

function LoadFrame() {

	ACStatus_get();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();		
}

