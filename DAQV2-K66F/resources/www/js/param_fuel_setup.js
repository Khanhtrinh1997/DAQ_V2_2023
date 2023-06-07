/**
 * @author SonLQ4
 */
 

 
function fuelItem(id, vendor, serialNo) {
	this.id = id.trim();
	this.vendor = vendor.trim();
	this.serialNo = serialNo.trim();
}

function FUELStatusGet()
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
		
	
    var fuel = false;
    function fuelComplete()
    {
		var FUELListInfo = new Array();
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
				setRowText('idconfigfuel_TotalNo', FUELListInfo.length);
				for (i = 0; i < FUELListInfo.length; i++)
				{		
					if(FUELListInfo[i].vendor == 0)
					{
						setRowText(FUELSetParam[0][i], '');
						setRowText(FUELSetParam[1][i], '');	
					} else
					{
						setRowText(FUELSetParam[0][i], FUELListInfo[i].id);
						if(FUELListInfo[i].vendor == 1)
						{
							setRowText(FUELSetParam[1][i], 'HPT621');
						} 										
					}	
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
        fuel.open("GET", "/fuel_status?=", true);
        fuel.onreadystatechange = fuelComplete;
        fuel.send(null);
    }
	t=setTimeout("FUELStatusGet()",5000);
}

function fFUELID2Set () {	
	
	var vFUELVD2Set = false;
	var vFUELVD2DataSet = document.getElementById("fuelvd2");

	if(window.XMLHttpRequest){		
		
		vFUELVD2Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vFUELVD2Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vFUELVD2Set)	{
		if(vFUELVD2DataSet.value != ""){
			vFUELVD2Set.open("GET", "/fuelnum2v_value=" + vFUELVD2DataSet.value + "&id=" + Math.random(), true);
			vFUELVD2Set.send(null);
		}
	}
}

function fFUELID1Set () {	
	
	var vFUELVD1Set = false;
	var vFUELVD1DataSet = document.getElementById("fuelvd1");

	if(window.XMLHttpRequest){		
		
		vFUELVD1Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vFUELVD1Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vFUELVD1Set)	{
		if(vFUELVD1DataSet.value != ""){
			vFUELVD1Set.open("GET", "/fuelnum1v_value=" + vFUELVD1DataSet.value + "&id=" + Math.random(), true);
			vFUELVD1Set.send(null);
		}
	}
}

function fFUELTotalNumSet (){
	var vFUELTotalNumSet = false;
	var vFUELTotalNumDataSet = document.getElementById("idfuel_TotalNo");
	
	if(window.XMLHttpRequest){
		vFUELTotalNumSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vFUELTotalNumSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vFUELTotalNumSet)	{
		if(vFUELTotalNumDataSet.value != ""){
			vFUELTotalNumSet.open("GET", "/fuelTotal_value=" + vFUELTotalNumDataSet.value + "&id=" + Math.random(), true);
			vFUELTotalNumSet.send(null);
		}
	}
}

function LoadFrame() {

	FUELStatusGet();	
	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}


