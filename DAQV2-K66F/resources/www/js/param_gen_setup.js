/**
 * @author SonLQ4
 */
 

 
function generatorItem(id, vendor, serialNo) {
	this.id = id.trim();
	this.vendor = vendor.trim();
	this.serialNo = serialNo.trim();
}

function GENStatusGet()
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
		
	
    var generator = false;
    function generatorComplete()
    {
		var GENListInfo = new Array();
        if(generator.readyState == 4)
        {
            if(generator.status == 200)
            {                
				text = generator.responseText;
				length = generator.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							GENListInfo.push(new generatorItem(name[0], name[1], name[2]));
						}
					}                                         
				}
				setRowText('idconfiggen_TotalNo', GENListInfo.length);
				for (i = 0; i < GENListInfo.length; i++)
				{		
					if(GENListInfo[i].vendor == 0)
					{
						setRowText(GenSetParam[0][i], '');
						setRowText(GenSetParam[1][i], '');
						/*setRowText(GenSetParam[2][i], '');*/		
					
					} else
					{
						setRowText(GenSetParam[0][i], GENListInfo[i].id);
						if(GENListInfo[i].vendor == 1)
						{
							setRowText(GenSetParam[1][i], 'EMKO');
						} else if(GENListInfo[i].vendor == 2)
						{
							setRowText(GenSetParam[1][i], 'BE142');
						} else if(GENListInfo[i].vendor == 3)
						{
							setRowText(GenSetParam[1][i], 'DEEPSEA');
						}  else if(GENListInfo[i].vendor == 4)
						{
							setRowText(GenSetParam[1][i], 'LR2057');
						}  else if(GENListInfo[i].vendor == 5)
						{
							setRowText(GenSetParam[1][i], 'HIMOINSA');
						}  else if(GENListInfo[i].vendor == 6)
						{
							setRowText(GenSetParam[1][i], 'QC315');
						}  else if(GENListInfo[i].vendor == 7)
						{
							setRowText(GenSetParam[1][i], 'CUMMIN PS500');
						}  	 								
						/*setRowText(GenSetParam[2][i], GENListInfo[i].serialNo);*/						
					}	
				}						
			}			
        }
    }

    if(window.XMLHttpRequest)
    {
        generator = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        generator = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(generator)
    {
        generator.open("GET", "/generator_status?=", true);
        generator.onreadystatechange = generatorComplete;
        generator.send(null);
    }
	t=setTimeout("GENStatusGet()",5000);
}

function fGenID2Set () {	
	
	var vGenVD2Set = false;
	var vGenVD2DataSet = document.getElementById("genvd2");

	if(window.XMLHttpRequest){		
		
		vGenVD2Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vGenVD2Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vGenVD2Set)	{
		if(vGenVD2DataSet.value != ""){
			vGenVD2Set.open("GET", "/gennum2v_value=" + vGenVD2DataSet.value + "&id=" + Math.random(), true);
			vGenVD2Set.send(null);
		}
	}
}

function fGenID1Set () {	
	
	var vGenVD1Set = false;
	var vGenVD1DataSet = document.getElementById("genvd1");

	if(window.XMLHttpRequest){		
		
		vGenVD1Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vGenVD1Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vGenVD1Set)	{
		if(vGenVD1DataSet.value != ""){
			vGenVD1Set.open("GET", "/gennum1v_value=" + vGenVD1DataSet.value + "&id=" + Math.random(), true);
			vGenVD1Set.send(null);
		}
	}
}

function fGenTotalNumSet (){
	var vGenTotalNumSet = false;
	var vGenTotalNumDataSet = document.getElementById("idgen_TotalNo");
	
	if(window.XMLHttpRequest){
		vGenTotalNumSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vGenTotalNumSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vGenTotalNumSet)	{
		if(vGenTotalNumDataSet.value != ""){
			vGenTotalNumSet.open("GET", "/genTotal_value=" + vGenTotalNumDataSet.value + "&id=" + Math.random(), true);
			vGenTotalNumSet.send(null);
		}
	}
}

function LoadFrame() {

	GENStatusGet();	
	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}


