/**
 * @author SonLQ4
 */

function fuelItem(id , status, fuellevel) {
	this.id = id.trim();
	this.status = status.trim();
	this.fuellevel = fuellevel.trim();
}
	

function fuelStatusGet()
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
	var FUELListInfo = new Array();	
	
	var fuelTable = document.getElementById("fuelTable");
	
    var fuel = false;
    function fuelComplete()
    {
        if(fuel.readyState == 4)
        {
            if(fuel.status == 200)
            {                
				text = fuel.responseText;
				length = fuel.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							FUELListInfo.push(new fuelItem(name[0], name[1], name[2]));
						}
					}                                         
				}

				for (i = 0; i < 2; i++)
				{
					FUELListInfo[i].fuellevel = parseFloat(FUELListInfo[i].fuellevel/1000).toFixed(3);
					if (FUELListInfo[i].status == 165)
					{
						setRowText(FUELParam[1][i], 'Warning');
					}
					else if (FUELListInfo[i].status == 170)
					{
						setRowText(FUELParam[1][i], 'Disconnected');
					}
					else if (FUELListInfo[i].status == 255)
					{
						setRowText(FUELParam[1][i], 'Connected');
					}else {	
						setRowText(FUELParam[1][i], "Unavailable");
					}		
					setRowText(FUELParam[2][i], FUELListInfo[i].fuellevel);
					}				
			}	
        }
    }
	
			

    if(window.XMLHttpRequest)
    {
        fuel = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        fuel = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(fuel)
    {
        fuel.open("GET", "/fuelinfo_status?=", true);
        fuel.onreadystatechange = fuelComplete;
        fuel.send(null);
    }
	t=setTimeout("fuelStatusGet()",5000);
}


function LoadFrame() {
	fuelStatusGet();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

