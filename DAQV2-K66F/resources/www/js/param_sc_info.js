/**
 * @author SonLQ4
 */


function chargerItem(id, serialNo , ivoltage, ovoltage, ocurrent, opower, temperature) {
	this.id = id.trim();
	this.serialNo = serialNo.trim();
	this.ivoltage = ivoltage.trim();
	this.ovoltage = ovoltage.trim();
	this.ocurrent = ocurrent.trim();
	this.opower = opower.trim();
	this.temperature = temperature.trim();
}

function chargerStatusGet()
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
	var CHARGERListInfo = new Array();		
	
    var charger = false;
    function chargerComplete()
    {
        if(charger.readyState == 4)
        {
            if(charger.status == 200)
            {                
				text = charger.responseText;
				length = charger.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							CHARGERListInfo.push(new chargerItem(name[0], name[1], name[2], name[3], name[4], name[5], name[6]));
						}
					}
				}
				
				for (i = 0; i < 5; i++)
				{					
					setRowText(ChargerParam[0][i], CHARGERListInfo[i].id);
					setRowText(ChargerParam[1][i], CHARGERListInfo[i].serialNo);
					setRowText(ChargerParam[2][i], CHARGERListInfo[i].ovoltage);
					setRowText(ChargerParam[3][i], CHARGERListInfo[i].ivoltage);
					setRowText(ChargerParam[4][i], CHARGERListInfo[i].ocurrent);
					setRowText(ChargerParam[5][i], CHARGERListInfo[i].opower);
					setRowText(ChargerParam[6][i], CHARGERListInfo[i].temperature);
				}				
			}
			
        }
    }

    if(window.XMLHttpRequest)
    {
        charger = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        charger = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(charger)
    {
        charger.open("GET", "/charger_status?=", true);
        charger.onreadystatechange = chargerComplete;
        charger.send(null);
    }
	t=setTimeout("chargerStatusGet()",5000);
}

function LoadFrame() {
	chargerStatusGet();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

