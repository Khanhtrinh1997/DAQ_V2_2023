/**
 * @author SonLQ4
 */

function rectItem(id, serialNo , voltage, current, temperature) {
	this.id = id.trim();
	this.serialNo = serialNo.trim();
	this.voltage = voltage.trim();
	this.current = current.trim();
	this.temperature = temperature.trim();
}

function rectStatusGet()
{
	var i=0;
	var j=0;
	var y=0;
	var x = 0;
	var t= 0;
	var m = 0;
	var length=0;
	var text;
	var temp1=Array(300);
	var temp2=Array(300);
	var RECTListInfo = new Array();		
		
	
    var rect = false;
    function rectComplete()
    {
        if(rect.readyState == 4)
        {
            if(rect.status == 200)
            {                
				text = rect.responseText;
				length = rect.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							RECTListInfo.push(new rectItem(name[0], name[1], name[2], name[3], name[4]));
						}
					}                                         
				}
				
				for (i = 0; i < 11; i++)
				{
					RECTListInfo[i].voltage = parseFloat(RECTListInfo[i].voltage/100).toFixed(2);
					RECTListInfo[i].current = parseFloat(RECTListInfo[i].current/100).toFixed(2);					
					RECTListInfo[i].temperature = parseFloat(RECTListInfo[i].temperature/100).toFixed(2);
					
					setRowText(RectParam[0][i], RECTListInfo[i].id);
					//setRowText(RectParam[1][i], RECTListInfo[i].serialNo);
					if((RECTListInfo[i].voltage == 0) && (RECTListInfo[i].current == 0) && (RECTListInfo[i].temperature == 0))
					{
						setRowText(RectParam[2][i], '');
						setRowText(RectParam[3][i], '');
						setRowText(RectParam[4][i], '');			
					
					} else
					{
						setRowText(RectParam[2][i], RECTListInfo[i].voltage);
						setRowText(RectParam[3][i], RECTListInfo[i].current);
						setRowText(RectParam[4][i], RECTListInfo[i].temperature);
					}
				//	setRowText(RectParam[0][i], RECTListInfo[i].id);
				//	setRowText(RectParam[1][i], RECTListInfo[i].serialNo);
				//	setRowText(RectParam[2][i], RECTListInfo[i].voltage);
				//	setRowText(RectParam[3][i], RECTListInfo[i].current);
				//	setRowText(RectParam[4][i], RECTListInfo[i].temperature);
				}

				//	setRowText('idrect_id1', RECTListInfo[0].id);
				//	setRowText('idrect_SN1', RECTListInfo[0].serialNo);
				//	setRowText('idrect_voltage1', RECTListInfo[0].voltage);
				//	setRowText('idrect_current1', RECTListInfo[0].current);
				//	setRowText('idrect_temp1', RECTListInfo[0].temperature);				
			}
			
        }
    }

    if(window.XMLHttpRequest)
    {
        rect = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        rect = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(rect)
    {
        rect.open("GET", "/rect_status?=", true);
        rect.onreadystatechange = rectComplete;
        rect.send(null);
    }
	t=setTimeout("rectStatusGet()",5000);
}

function LoadFrame() {
	rectStatusGet();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

