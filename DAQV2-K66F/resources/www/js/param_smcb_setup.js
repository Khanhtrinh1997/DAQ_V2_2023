/**
 * @author SonLQ4
 */
 

 
function smartmcbItem(id, vendor, connect,state) {
	this.id = id.trim();
	this.vendor = vendor.trim();
	this.connect = connect.trim();
	this.state = state.trim();
}

function SMCBStatusGet()
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
		
	
    var smartmcb = false;
    function smartmcbComplete()
    {
		var SMCBListInfo = new Array();
        if(smartmcb.readyState == 4)
        {
            if(smartmcb.status == 200)
            {                
				text = smartmcb.responseText;
				length = smartmcb.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							SMCBListInfo.push(new smartmcbItem(name[0], name[1], name[2], name[3]));
						}
					}                                         
				}
				setRowText('idconfigsmcb_TotalNo', SMCBListInfo.length);
				for (i = 0; i < SMCBListInfo.length; i++)
				{		
					if(SMCBListInfo[i].vendor == 0)//ko co mcb nao duoc cai dat
					{
						setRowText(SMCBSetParam[0][i], '');
						setRowText(SMCBSetParam[1][i], '');	
						setRowText(SMCBSetParam[2][i], '');	
					
					} else// co mcb dc cai dat
					{
						setRowText(SMCBSetParam[0][i], SMCBListInfo[i].id);// id cua mcb thu i
						if(SMCBListInfo[i].vendor == 1)// ten mcb
						{
							setRowText(SMCBSetParam[1][i], 'OPEN');
						}	else if(SMCBListInfo[i].vendor == 2)// ten mcb 
						{
							setRowText(SMCBSetParam[1][i], 'MATIS');
						}   else if(SMCBListInfo[i].vendor == 3)// ten mcb
						{
							setRowText(SMCBSetParam[1][i], 'GOL');
						} 		
//						if(SMCBListInfo[i].connect == 0)// tinh trang ket noi mcb
//						{
//							setRowText(SMCBSetParam[2][i], 'inActive');
//						} else 
						if (SMCBListInfo[i].connect == 255)
						{
							setRowText(SMCBSetParam[2][i], 'normal');
						} else if (SMCBListInfo[i].connect == 170)
						{
							setRowText(SMCBSetParam[2][i], 'noResponse');
						}
						if(SMCBListInfo[i].state == 0)// trang thai cua mcb
						{
							setRowText(SMCBSetParam[3][i], 'OFF');
						} else if(SMCBListInfo[i].state == 1)
						{
							setRowText(SMCBSetParam[3][i], 'ON');
						} else
						{
							setRowText(SMCBSetParam[3][i], 'LOCK');
						}
					}	
				}						
			}			
        }
    }

    if(window.XMLHttpRequest)
    {
        smartmcb = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        smartmcb = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(smartmcb)
    {
        smartmcb.open("GET", "/smcb_status?=", true);
        smartmcb.onreadystatechange = smartmcbComplete;
        smartmcb.send(null);
    }
	t=setTimeout("SMCBStatusGet()",5000);
}

function fSMCBState5Set(){
	var vSMCBState5Set = false;
	var vSMCBState5DataSet = document.getElementById("smcbs5");

	if(window.XMLHttpRequest){		
		
		vSMCBState5Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vSMCBState5Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBState5Set)	{
		if(vSMCBState5DataSet.value != ""){
			vSMCBState5Set.open("GET", "/smcbState5v_value=" + vSMCBState5DataSet.value + "&id=" + Math.random(), true);
			vSMCBState5Set.send(null);
		}
	}
}
function fSMCBState4Set(){
	var vSMCBState4Set = false;
	var vSMCBState4DataSet = document.getElementById("smcbs4");

	if(window.XMLHttpRequest){		
		
		vSMCBState4Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vSMCBState4Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBState4Set)	{
		if(vSMCBState4DataSet.value != ""){
			vSMCBState4Set.open("GET", "/smcbState4v_value=" + vSMCBState4DataSet.value + "&id=" + Math.random(), true);
			vSMCBState4Set.send(null);
		}
	}
}
function fSMCBState3Set(){
	var vSMCBState3Set = false;
	var vSMCBState3DataSet = document.getElementById("smcbs3");

	if(window.XMLHttpRequest){		
		
		vSMCBState3Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vSMCBState3Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBState3Set)	{
		if(vSMCBState3DataSet.value != ""){
			vSMCBState3Set.open("GET", "/smcbState3v_value=" + vSMCBState3DataSet.value + "&id=" + Math.random(), true);
			vSMCBState3Set.send(null);
		}
	}
}
function fSMCBState2Set(){
	var vSMCBState2Set = false;
	var vSMCBState2DataSet = document.getElementById("smcbs2");

	if(window.XMLHttpRequest){		
		
		vSMCBState2Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vSMCBState2Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBState2Set)	{
		if(vSMCBState2DataSet.value != ""){
			vSMCBState2Set.open("GET", "/smcbState2v_value=" + vSMCBState2DataSet.value + "&id=" + Math.random(), true);
			vSMCBState2Set.send(null);
		}
	}
}
function fSMCBState1Set(){
	var vSMCBState1Set = false;
	var vSMCBState1DataSet = document.getElementById("smcbs1");

	if(window.XMLHttpRequest){		
		
		vSMCBState1Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vSMCBState1Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBState1Set)	{
		if(vSMCBState1DataSet.value != ""){
			vSMCBState1Set.open("GET", "/smcbState1v_value=" + vSMCBState1DataSet.value + "&id=" + Math.random(), true);
			vSMCBState1Set.send(null);
		}
	}
}
function fSMCBID5Set () {	
	
	var vSMCBVD5Set = false;
	var vSMCBVD5DataSet = document.getElementById("smcbvd5");

	if(window.XMLHttpRequest){		
		
		vSMCBVD5Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vSMCBVD5Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBVD5Set)	{
		if(vSMCBVD5DataSet.value != ""){
			vSMCBVD5Set.open("GET", "/smcbnum5v_value=" + vSMCBVD5DataSet.value + "&id=" + Math.random(), true);
			vSMCBVD5Set.send(null);
		}
	}
}
function fSMCBID4Set () {	
	
	var vSMCBVD4Set = false;
	var vSMCBVD4DataSet = document.getElementById("smcbvd4");

	if(window.XMLHttpRequest){		
		
		vSMCBVD4Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vSMCBVD4Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBVD4Set)	{
		if(vSMCBVD4DataSet.value != ""){
			vSMCBVD4Set.open("GET", "/smcbnum4v_value=" + vSMCBVD4DataSet.value + "&id=" + Math.random(), true);
			vSMCBVD4Set.send(null);
		}
	}
}
function fSMCBID3Set () {	
	
	var vSMCBVD3Set = false;
	var vSMCBVD3DataSet = document.getElementById("smcbvd3");

	if(window.XMLHttpRequest){		
		
		vSMCBVD3Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vSMCBVD3Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBVD3Set)	{
		if(vSMCBVD3DataSet.value != ""){
			vSMCBVD3Set.open("GET", "/smcbnum3v_value=" + vSMCBVD3DataSet.value + "&id=" + Math.random(), true);
			vSMCBVD3Set.send(null);
		}
	}
}
function fSMCBID2Set () {	
	
	var vSMCBVD2Set = false;
	var vSMCBVD2DataSet = document.getElementById("smcbvd2");

	if(window.XMLHttpRequest){		
		
		vSMCBVD2Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vSMCBVD2Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBVD2Set)	{
		if(vSMCBVD2DataSet.value != ""){
			vSMCBVD2Set.open("GET", "/smcbnum2v_value=" + vSMCBVD2DataSet.value + "&id=" + Math.random(), true);
			vSMCBVD2Set.send(null);
		}
	}
}
function fSMCBID1Set () {	
	
	var vSMCBVD1Set = false;
	var vSMCBVD1DataSet = document.getElementById("smcbvd1");

	if(window.XMLHttpRequest){		
		
		vSMCBVD1Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){		
		
		vSMCBVD1Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBVD1Set)	{
		if(vSMCBVD1DataSet.value != ""){
			vSMCBVD1Set.open("GET", "/smcbnum1v_value=" + vSMCBVD1DataSet.value + "&id=" + Math.random(), true);
			vSMCBVD1Set.send(null);
		}
	}
}

function fSMCBTotalNumSet (){
	var vSMCBTotalNumSet = false;
	var vSMCBTotalNumDataSet = document.getElementById("idsmcb_TotalNo");
	
	if(window.XMLHttpRequest){
		vSMCBTotalNumSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vSMCBTotalNumSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vSMCBTotalNumSet)	{
		if(vSMCBTotalNumDataSet.value != ""){
			vSMCBTotalNumSet.open("GET", "/smcbTotal_value=" + vSMCBTotalNumDataSet.value + "&id=" + Math.random(), true);
			vSMCBTotalNumSet.send(null);
		}
	}
}

function LoadFrame() {

	SMCBStatusGet();	
	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}


