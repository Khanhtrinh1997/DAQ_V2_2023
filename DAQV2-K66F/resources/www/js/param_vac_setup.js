/**
 * @author SonLQ4
 */
 

 
function vacItem(id, vendor, serialNo) {
	this.id = id.trim();
	this.vendor = vendor.trim();
	this.serialNo = serialNo.trim();
}

function VACStatusGet()
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
		
	
    var vac = false;
    function vacComplete()
    {
		var VACListInfo = new Array();
        if(vac.readyState == 4)
        {
            if(vac.status == 200)
            {                
				text = vac.responseText;
				length = vac.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							VACListInfo.push(new vacItem(name[0], name[1], name[2]));
						}
					}                                         
				}
				setRowText('idconfigvac_TotalNo', VACListInfo.length);
				for (i = 0; i < VACListInfo.length; i++)
				{		
					if(VACListInfo[i].vendor == 0)
					{
						setRowText(VACSetParam[0][i], '');
						setRowText(VACSetParam[1][i], '');
						/*setRowText(VACSetParam[2][i], '');*/		
					
					} else
					{
						setRowText(VACSetParam[0][i], VACListInfo[i].id);
						if(VACListInfo[i].vendor == 1)
						{
							setRowText(VACSetParam[1][i], 'Viettel');
						} 				
						/*setRowText(VACSetParam[2][i], VACListInfo[i].serialNo);*/						
					}	
				}						
			}			
        }
    }

    if(window.XMLHttpRequest)
    {
        vac = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        vac = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(vac)
    {
        vac.open("GET", "/vac_status?=", true);
        vac.onreadystatechange = vacComplete;
        vac.send(null);
    }
	t=setTimeout("VACStatusGet()",5000);
}

function fVACID2Set () {	
	
	var vVACVD2Set = false;
	var vVACVD2DataSet = document.getElementById("vacvd2");

	if(window.XMLHttpRequest){		
		
		vVACVD2Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vVACVD2Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vVACVD2Set)	{
		if(vVACVD2DataSet.value != ""){
			vVACVD2Set.open("GET", "/vacnum2v_value=" + vVACVD2DataSet.value + "&id=" + Math.random(), true);
			vVACVD2Set.send(null);
		}
	}
}

function fVACID1Set () {	
	
	var vVACVD1Set = false;
	var vVACVD1DataSet = document.getElementById("vacvd1");

	if(window.XMLHttpRequest){		
		
		vVACVD1Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vVACVD1Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vVACVD1Set)	{
		if(vVACVD1DataSet.value != ""){
			vVACVD1Set.open("GET", "/vacnum1v_value=" + vVACVD1DataSet.value + "&id=" + Math.random(), true);
			vVACVD1Set.send(null);
		}
	}
}

function fVACTotalNumSet (){
	var vVACTotalNumSet = false;
	var vVACTotalNumDataSet = document.getElementById("idvac_TotalNo");
	
	if(window.XMLHttpRequest){
		vVACTotalNumSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vVACTotalNumSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vVACTotalNumSet)	{
		if(vVACTotalNumDataSet.value != ""){
			vVACTotalNumSet.open("GET", "/vacTotal_value=" + vVACTotalNumDataSet.value + "&id=" + Math.random(), true);
			vVACTotalNumSet.send(null);
		}
	}
}

function LoadFrame() {

	VACStatusGet();	
	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}


