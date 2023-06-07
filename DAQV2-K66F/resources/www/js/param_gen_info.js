/**
 * @author SonLQ4
 */

function genItem(id , status, voltage1, voltage2, voltage3, current1, current2, current3, oilpressure, fuellevel, battvolt, frequency, oiltemp, workhour, speed) {
	this.id = id.trim();
	this.status = status.trim();
	this.voltage1 = voltage1.trim();
	this.voltage2 = voltage2.trim();
	this.voltage3 = voltage3.trim();
	this.current1 = current1.trim();
	this.current2 = current2.trim();
	this.current3 = current3.trim();
	this.oilpressure = oilpressure.trim();
	this.fuellevel = fuellevel.trim();
	this.battvolt = battvolt.trim();
	this.frequency = frequency.trim();
	this.oiltemp = oiltemp.trim();
	this.workhour = workhour.trim();
	this.speed = speed.trim();
}

	
var cell = new Array();	
var row = new Array();		

function genStatusGet()
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
	var GENListInfo = new Array();	
	
	var genTable = document.getElementById("genTable");
	
    var gen = false;
    function genComplete()
    {
        if(gen.readyState == 4)
        {
            if(gen.status == 200)
            {                
				text = gen.responseText;
				length = gen.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							GENListInfo.push(new genItem(name[0], name[1], name[2], name[3], name[4], name[5], name[6], name[7], name[8], name[9], name[10], name[11], name[12], name[13], name[14]));
						}
					}                                         
				}

				
				for (i = 0; i < 2; i++)
				{
					GENListInfo[i].voltage1 = parseFloat(GENListInfo[i].voltage1/100).toFixed(2);
					GENListInfo[i].voltage2 = parseFloat(GENListInfo[i].voltage2/100).toFixed(2);
					GENListInfo[i].voltage3 = parseFloat(GENListInfo[i].voltage3/100).toFixed(2);
					
					GENListInfo[i].current1 = parseFloat(GENListInfo[i].current1/100).toFixed(2);
					GENListInfo[i].current2 = parseFloat(GENListInfo[i].current2/100).toFixed(2);	
					GENListInfo[i].current3 = parseFloat(GENListInfo[i].current3/100).toFixed(2);		
																
					GENListInfo[i].oilpressure = parseFloat(GENListInfo[i].oilpressure/10).toFixed(1);
					GENListInfo[i].battvolt = parseFloat(GENListInfo[i].battvolt/10).toFixed(1);		
					GENListInfo[i].frequency = parseFloat(GENListInfo[i].frequency/10).toFixed(1);		
					GENListInfo[i].oiltemp = parseFloat(GENListInfo[i].oiltemp/10).toFixed(1);	
					
					//setRowText(GENParam[0][i], GENListInfo[i].id);
					if (GENListInfo[i].status == 165)
					{
						setRowText(GENParam[1][i], 'Warning');
					}
					else if (GENListInfo[i].status == 170)
					{
						setRowText(GENParam[1][i], 'Disconnected');
					}
					else if (GENListInfo[i].status == 255)
					{
						setRowText(GENParam[1][i], 'Connected');
					}else {	
						setRowText(GENParam[1][i], "Unavailable");
					}	
					
					setRowText(GENParam[2][i], GENListInfo[i].voltage1);						
					setRowText(GENParam[3][i], GENListInfo[i].current1);
					setRowText(GENParam[4][i], GENListInfo[i].oilpressure);
					setRowText(GENParam[5][i], GENListInfo[i].fuellevel);
					setRowText(GENParam[6][i], GENListInfo[i].battvolt);
					setRowText(GENParam[7][i], GENListInfo[i].frequency);
					setRowText(GENParam[8][i], GENListInfo[i].oiltemp);
					setRowText(GENParam[9][i], GENListInfo[i].workhour);
					setRowText(GENParam[10][i], GENListInfo[i].speed);
				}				
			}	
        }
    }
	
			

    if(window.XMLHttpRequest)
    {
        gen = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        gen = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(gen)
    {
        gen.open("GET", "/geninfo_status?=", true);
        gen.onreadystatechange = genComplete;
        gen.send(null);
    }
	t=setTimeout("genStatusGet()",5000);
}


function LoadFrame() {
	genStatusGet();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

