/**
 * @author SonLQ4
 */

function libItem(id , voltage, current, temperature, soc) {
	this.id = id.trim();
	this.voltage = voltage.trim();
	this.current = current.trim();
	this.temperature = temperature.trim();
	this.soc = soc.trim();
}		
function libStatusGet()
{
	var i=0;
	var t= 0;
	var length=0;
	var text;
	var LIBListInfo = new Array();	
    var lib = false;
    function libComplete()
    {
        if(lib.readyState == 4)
        {
            if(lib.status == 200)
            {                
				text = lib.responseText;
				length = lib.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							LIBListInfo.push(new libItem(name[0], name[1], name[2], name[3], name[4]));
						}
					}                                         
				}
				for (i = 0; i < 16; i++)
				{
					LIBListInfo[i].voltage = parseFloat(LIBListInfo[i].voltage/100).toFixed(2);
					LIBListInfo[i].current = parseFloat(LIBListInfo[i].current/100).toFixed(2);					
					LIBListInfo[i].temperature = parseFloat(LIBListInfo[i].temperature/10).toFixed(1);			
					LIBListInfo[i].soc = parseFloat(LIBListInfo[i].soc/100).toFixed(2);
					
					setRowText(LIBParam[0][i], LIBListInfo[i].id);
					
					if((LIBListInfo[i].voltage == 0) && (LIBListInfo[i].current == 0) && (LIBListInfo[i].temperature == 0))
					{
						setRowText(LIBParam[2][i], '');
						setRowText(LIBParam[3][i], '');
						setRowText(LIBParam[4][i], '');	
						setRowText(LIBParam[1][i], '');			
					
					} else
					{
						setRowText(LIBParam[2][i], LIBListInfo[i].voltage);
						setRowText(LIBParam[3][i], LIBListInfo[i].current);
						setRowText(LIBParam[4][i], LIBListInfo[i].temperature);
						setRowText(LIBParam[1][i], LIBListInfo[i].soc);
					}
				}				
			}	
        }
    }
	
    if(window.XMLHttpRequest)
    {
        lib = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        lib = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(lib)
    {
        lib.open("GET", "/libbatt_status?=", true);
        lib.onreadystatechange = libComplete;
        lib.send(null);
    }
	t=setTimeout("libStatusGet()",5000);
}
function LoadFrame() {
	libStatusGet();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

