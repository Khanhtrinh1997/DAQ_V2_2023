/**
 * @author SonLQ4
 */

function BTSStatus_get()
{
	var BTSStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function BTSStatusComplete()
	{
		if(BTSStatus.readyState == 4)
		{
			if(BTSStatus.status == 200)
			{
				text = BTSStatus.responseText;
				length = BTSStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}		
				
				vBtsName = BTSStatus.responseText.substring(0,tam[0]);			
				vBtsDCVendor = BTSStatus.responseText.substring(tam[0]+1,tam[1]);
				vBtsCModel = BTSStatus.responseText.substring(tam[1]+1,tam[2]);		
				vBtsRectType = BTSStatus.responseText.substring(tam[2]+1,tam[3]);		
				vBtsAmbTemp = BTSStatus.responseText.substring(tam[3]+1,tam[4]);
				vCabCtrl1Type = BTSStatus.responseText.substring(tam[4]+1,tam[5]);
				vCabCtrl2Type = BTSStatus.responseText.substring(tam[5]+1,tam[6]);					
				
				vBtsAmbTemp = parseFloat(vBtsAmbTemp/10).toFixed(1);					
								
				setRowText('idbts_Name', vBtsName);
				
				if(vCabCtrl1Type == 0) {setRowText('idbts_DCSysVendor', "Emerson");}
				else if(vCabCtrl1Type == 1) {setRowText('idbts_DCSysVendor', "ZTE");}
				else if(vCabCtrl1Type == 2) {setRowText('idbts_DCSysVendor', "Agisson");}
				else if(vCabCtrl1Type == 3) {setRowText('idbts_DCSysVendor', "Emerson");}
				else if(vCabCtrl1Type == 4) {{setRowText('idbts_DCSysVendor', "BDP");}}
				else if(vCabCtrl1Type == 5) {{setRowText('idbts_DCSysVendor', "Vertiv");}}
				
				if(vCabCtrl1Type == 0) {setRowText('idbts_ControllerModel', "M500D");}
				else if(vCabCtrl1Type == 1) {setRowText('idbts_ControllerModel', "ZXDU58");}
				else if(vCabCtrl1Type == 2) {setRowText('idbts_ControllerModel', "CPMU01");}
				else if(vCabCtrl1Type == 3) {setRowText('idbts_ControllerModel', "M221S");}
				else if(vCabCtrl1Type == 4) {setRowText('idbts_ControllerModel', "DKD51-2");}
				else if(vCabCtrl1Type == 5) {setRowText('idbts_ControllerModel', "M830B");}
				//setRowText('idbts_DCSysVendor', vBtsDCVendor);
				//setRowText('idbts_ControllerModel', vBtsCModel);
				setRowText('idbts_RectType', vBtsRectType);
				setRowText('idbts_AmbientTemp', vBtsAmbTemp);
								
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		BTSStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		BTSStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(BTSStatus)
	{
		BTSStatus.open("GET","/bts_status?=", true);
		BTSStatus.onreadystatechange = BTSStatusComplete;
		BTSStatus.send(null);
	}
t=setTimeout("BTSStatus_get()",5000);
}

function LoadFrame() {
	BTSStatus_get();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

