/**
 * @author SonLQ4
 */

function DEVIPStatus_get()
{
	var DEVIPStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function DEVIPStatusComplete()
	{
		if(DEVIPStatus.readyState == 4)
		{
			if(DEVIPStatus.status == 200)
			{
				text = DEVIPStatus.responseText;
				length = DEVIPStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}
				
				vDevSip = DEVIPStatus.responseText.substring(0,tam[0]);			
				vDevGip = DEVIPStatus.responseText.substring(tam[0]+1,tam[1]);
				vDevMip = DEVIPStatus.responseText.substring(tam[1]+1,tam[2]);
				vBTSName = 	DEVIPStatus.responseText.substring(tam[2]+1,tam[3]);	
				vCab1Type = 	DEVIPStatus.responseText.substring(tam[3]+1,tam[4]);	
				vCab2Type = 	DEVIPStatus.responseText.substring(tam[4]+1,tam[5]);
				vFTPName = 	DEVIPStatus.responseText.substring(tam[5]+1,tam[6]);
				
				setRowText('iddev_IP', vDevSip);				
				setRowText('iddev_GW', vDevGip);
				setRowText('iddev_SN', vDevMip);
				setRowText('idbts_ID', vBTSName);
				setRowText('idftp_Name', vFTPName);

				if(vCab1Type == 0) {setRowText('idcab1_Type', "Emerson M500D");}
				else if(vCab1Type == 1) {setRowText('idcab1_Type', "ZTE ZXDU58");}
				else if(vCab1Type == 2) {setRowText('idcab1_Type', "Agisson TPS4800");}
				else if(vCab1Type == 3) {setRowText('idcab1_Type', "Emerson M221S");}
				else if(vCab1Type == 4) {setRowText('idcab1_Type', "DKD51 BDP");}
				else if(vCab1Type == 5) {setRowText('idcab1_Type', "VERTIV M830B");}
				
				// if(vCab2Type == 0) {setRowText('idcab2_Type', "Emerson M500D");}
				// else if(vCab2Type == 1) {setRowText('idcab2_Type', "ZTE ZXDU58");}
				// else if(vCab2Type == 2) {setRowText('idcab2_Type', "Agisson TPS4800");}
				// else if(vCab2Type == 3) {setRowText('idcab2_Type', "Emerson M221S");}
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		DEVIPStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		DEVIPStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(DEVIPStatus)
	{
		DEVIPStatus.open("GET","/deviceip_status?=", true);
		DEVIPStatus.onreadystatechange = DEVIPStatusComplete;
		DEVIPStatus.send(null);
	}
t=setTimeout("DEVIPStatus_get()",5000);
}

function fDevSIPSet () {
	var vSIP1Set = false;
	var vSIP1DataSet = document.getElementById("sip1");
	var vSIP2Set = false;
	var vSIP2DataSet = document.getElementById("sip2");	
	var vSIP3Set = false;
	var vSIP3DataSet = document.getElementById("sip3");
	var vSIP4Set = false;
	var vSIP4DataSet = document.getElementById("sip4");
	
	var vGIP1Set = false;
	var vGIP1DataSet = document.getElementById("gip1");
	var vGIP2Set = false;
	var vGIP2DataSet = document.getElementById("gip2");	
	var vGIP3Set = false;
	var vGIP3DataSet = document.getElementById("gip3");
	var vGIP4Set = false;
	var vGIP4DataSet = document.getElementById("gip4");
	
	var vMIP1Set = false;
	var vMIP1DataSet = document.getElementById("mip1");
	var vMIP2Set = false;
	var vMIP2DataSet = document.getElementById("mip2");	
	var vMIP3Set = false;
	var vMIP3DataSet = document.getElementById("mip3");
	var vMIP4Set = false;
	var vMIP4DataSet = document.getElementById("mip4");

	if(window.XMLHttpRequest){
		vSIP1Set = new XMLHttpRequest();
		vSIP2Set = new XMLHttpRequest();
		vSIP3Set = new XMLHttpRequest();
		vSIP4Set = new XMLHttpRequest();
		
		vGIP1Set = new XMLHttpRequest();
		vGIP2Set = new XMLHttpRequest();
		vGIP3Set = new XMLHttpRequest();
		vGIP4Set = new XMLHttpRequest();
		
		vMIP1Set = new XMLHttpRequest();
		vMIP2Set = new XMLHttpRequest();
		vMIP3Set = new XMLHttpRequest();
		vMIP4Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vSIP1Set = new ActiveXObject("Microsoft.XMLHTTP");
		vSIP2Set = new ActiveXObject("Microsoft.XMLHTTP");
		vSIP3Set = new ActiveXObject("Microsoft.XMLHTTP");
		vSIP4Set = new ActiveXObject("Microsoft.XMLHTTP");
		
		vGIP1Set = new ActiveXObject("Microsoft.XMLHTTP");
		vGIP2Set = new ActiveXObject("Microsoft.XMLHTTP");
		vGIP3Set = new ActiveXObject("Microsoft.XMLHTTP");
		vGIP4Set = new ActiveXObject("Microsoft.XMLHTTP");
		
		vMIP1Set = new ActiveXObject("Microsoft.XMLHTTP");
		vMIP2Set = new ActiveXObject("Microsoft.XMLHTTP");
		vMIP3Set = new ActiveXObject("Microsoft.XMLHTTP");
		vMIP4Set = new ActiveXObject("Microsoft.XMLHTTP");
		
	}
	if(vSIP1Set)	{
		if(vSIP1DataSet.value != ""){
			vSIP1Set.open("GET", "/devsip1_value=" + vSIP1DataSet.value + "&id=" + Math.random(), true);
			vSIP1Set.send(null);
		}
	}
	if(vSIP2Set)	{
		if(vSIP2DataSet.value != ""){
			vSIP2Set.open("GET", "/devsip2_value=" + vSIP2DataSet.value + "&id=" + Math.random(), true);
			vSIP2Set.send(null);
		}
	}
	if(vSIP3Set)	{
		if(vSIP3DataSet.value != ""){
			vSIP3Set.open("GET", "/devsip3_value=" + vSIP3DataSet.value + "&id=" + Math.random(), true);
			vSIP3Set.send(null);
		}
	}
	if(vSIP4Set){
		if(vSIP4DataSet.value != ""){
			vSIP4Set.open("GET", "/devsip4_value=" + vSIP4DataSet.value + "&id=" + Math.random(), true);
			vSIP4Set.send(null);
		}
	}	

	if(vGIP1Set)	{
		if(vGIP1DataSet.value != ""){
			vGIP1Set.open("GET", "/devsip5_value=" + vGIP1DataSet.value + "&id=" + Math.random(), true);
			vGIP1Set.send(null);
		}
	}
	if(vGIP2Set)	{
		if(vGIP2DataSet.value != ""){
			vGIP2Set.open("GET", "/devsip6_value=" + vGIP2DataSet.value + "&id=" + Math.random(), true);
			vGIP2Set.send(null);
		}
	}
	if(vGIP3Set)	{
		if(vGIP3DataSet.value != ""){
			vGIP3Set.open("GET", "/devsip7_value=" + vGIP3DataSet.value + "&id=" + Math.random(), true);
			vGIP3Set.send(null);
		}
	}
	if(vGIP4Set){
		if(vGIP4DataSet.value != ""){
			vGIP4Set.open("GET", "/devsip8_value=" + vGIP4DataSet.value + "&id=" + Math.random(), true);
			vGIP4Set.send(null);
		}
	}	
	
	if(vMIP1Set)	{
		if(vMIP1DataSet.value != ""){
			vMIP1Set.open("GET", "/devsip9_value=" + vMIP1DataSet.value + "&id=" + Math.random(), true);
			vMIP1Set.send(null);
		}
	}
	if(vMIP2Set)	{
		if(vMIP2DataSet.value != ""){
			vMIP2Set.open("GET", "/devsipA_value=" + vMIP2DataSet.value + "&id=" + Math.random(), true);
			vMIP2Set.send(null);
		}
	}
	if(vMIP3Set)	{
		if(vMIP3DataSet.value != ""){
			vMIP3Set.open("GET", "/devsipB_value=" + vMIP3DataSet.value + "&id=" + Math.random(), true);
			vMIP3Set.send(null);
		}
	}
	if(vMIP4Set){
		if(vMIP4DataSet.value != ""){
			vMIP4Set.open("GET", "/devsipC_value=" + vMIP4DataSet.value + "&id=" + Math.random(), true);
			vMIP4Set.send(null);
		}
	}	
}

function fDevGIPSet () {
	var vGIP1Set = false;
	var vGIP1DataSet = document.getElementById("gip1");
	var vGIP2Set = false;
	var vGIP2DataSet = document.getElementById("gip2");	
	var vGIP3Set = false;
	var vGIP3DataSet = document.getElementById("gip3");
	var vGIP4Set = false;
	var vGIP4DataSet = document.getElementById("gip4");

	if(window.XMLHttpRequest){
		vGIP1Set = new XMLHttpRequest();
		vGIP2Set = new XMLHttpRequest();
		vGIP3Set = new XMLHttpRequest();
		vGIP4Set = new XMLHttpRequest();	
	}
	else if(window.ActiveXObject){
		vGIP1Set = new ActiveXObject("Microsoft.XMLHTTP");
		vGIP2Set = new ActiveXObject("Microsoft.XMLHTTP");
		vGIP3Set = new ActiveXObject("Microsoft.XMLHTTP");
		vGIP4Set = new ActiveXObject("Microsoft.XMLHTTP");	
	}
	if(vGIP1Set)	{
		if(vGIP1DataSet.value != ""){
			vGIP1Set.open("GET", "/devgip1_value=" + vGIP1DataSet.value + "&id=" + Math.random(), true);
			vGIP1Set.send(null);
		}
	}
	if(vGIP2Set)	{
		if(vGIP2DataSet.value != ""){
			vGIP2Set.open("GET", "/devgip2_value=" + vGIP2DataSet.value + "&id=" + Math.random(), true);
			vGIP2Set.send(null);
		}
	}
	if(vGIP3Set)	{
		if(vGIP3DataSet.value != ""){
			vGIP3Set.open("GET", "/devgip3_value=" + vGIP3DataSet.value + "&id=" + Math.random(), true);
			vGIP3Set.send(null);
		}
	}
	if(vGIP4Set){
		if(vGIP4DataSet.value != ""){
			vGIP4Set.open("GET", "/devgip4_value=" + vGIP4DataSet.value + "&id=" + Math.random(), true);
			vGIP4Set.send(null);
		}
	}
}

function fDevMIPSet () {	
	
	var vMIP1Set = false;
	var vMIP1DataSet = document.getElementById("mip1");
	var vMIP2Set = false;
	var vMIP2DataSet = document.getElementById("mip2");	
	var vMIP3Set = false;
	var vMIP3DataSet = document.getElementById("mip3");
	var vMIP4Set = false;
	var vMIP4DataSet = document.getElementById("mip4");

	if(window.XMLHttpRequest){		
		
		vMIP1Set = new XMLHttpRequest();
		vMIP2Set = new XMLHttpRequest();
		vMIP3Set = new XMLHttpRequest();
		vMIP4Set = new XMLHttpRequest();		
	}
	else if(window.ActiveXObject){		
		
		vMIP1Set = new ActiveXObject("Microsoft.XMLHTTP");
		vMIP2Set = new ActiveXObject("Microsoft.XMLHTTP");
		vMIP3Set = new ActiveXObject("Microsoft.XMLHTTP");
		vMIP4Set = new ActiveXObject("Microsoft.XMLHTTP");
	}	

	if(vMIP1Set)	{
		if(vMIP1DataSet.value != ""){
			vMIP1Set.open("GET", "/devmip1_value=" + vMIP1DataSet.value + "&id=" + Math.random(), true);
			vMIP1Set.send(null);
		}
	}
	if(vMIP2Set)	{
		if(vMIP2DataSet.value != ""){
			vMIP2Set.open("GET", "/devmip2_value=" + vMIP2DataSet.value + "&id=" + Math.random(), true);
			vMIP2Set.send(null);
		}
	}
	if(vMIP3Set)	{
		if(vMIP3DataSet.value != ""){
			vMIP3Set.open("GET", "/devmip3_value=" + vMIP3DataSet.value + "&id=" + Math.random(), true);
			vMIP3Set.send(null);
		}
	}
	if(vMIP4Set){
		if(vMIP4DataSet.value != ""){
			vMIP4Set.open("GET", "/devmip4_value=" + vMIP4DataSet.value + "&id=" + Math.random(), true);
			vMIP4Set.send(null);
		}
	}	
}

function fBtsIDSet () {	
	
	var vBtsIDSet = false;
	var vBtsIDDataSet = document.getElementById("btsid");

	if(window.XMLHttpRequest){		
		
		vBtsIDSet = new XMLHttpRequest();	
	}
	else if(window.ActiveXObject){		
		
		vBtsIDSet = new ActiveXObject("Microsoft.XMLHTTP");
	}	

	if(vBtsIDSet)	{
		if(vBtsIDDataSet.value != ""){
			vBtsIDSet.open("GET", "/btsid_value=" + vBtsIDDataSet.value + "&id=" + Math.random(), true);
			vBtsIDSet.send(null);
		}
	}
	
}

function fCab1TypeSet () {	
	
	var vCab1TypeSet = false;
	var vCab1TypeDataSet = document.getElementById("cab1_Type");

	if(window.XMLHttpRequest){		
		
		vCab1TypeSet = new XMLHttpRequest();	
	}
	else if(window.ActiveXObject){		
		
		vCab1TypeSet = new ActiveXObject("Microsoft.XMLHTTP");
	}	

	if(vCab1TypeSet)	{
		if(vCab1TypeDataSet.value != ""){
			vCab1TypeSet.open("GET", "/cabCtrl1Type_value=" + vCab1TypeDataSet.value + "&id=" + Math.random(), true);
			vCab1TypeSet.send(null);
		}
	}
	
}

function fCab2TypeSet () {	
	
	var vCab2TypeSet = false;
	var vCab2TypeDataSet = document.getElementById("cab2_Type");

	if(window.XMLHttpRequest){		
		
		vCab2TypeSet = new XMLHttpRequest();	
	}
	else if(window.ActiveXObject){		
		
		vCab2TypeSet = new ActiveXObject("Microsoft.XMLHTTP");
	}	

	if(vCab2TypeSet)	{
		if(vCab2TypeDataSet.value != ""){
			vCab2TypeSet.open("GET", "/cabCtrl2Type_value=" + vCab2TypeDataSet.value + "&id=" + Math.random(), true);
			vCab2TypeSet.send(null);
		}
	}
	
}

function fFtpNameSet () {	
	
	var vFtpNameSet = false;
	var vFtpNameDataSet = document.getElementById("ftpname");

	if(window.XMLHttpRequest){		
		
		vFtpNameSet = new XMLHttpRequest();	
	}
	else if(window.ActiveXObject){		
		
		vFtpNameSet = new ActiveXObject("Microsoft.XMLHTTP");
	}	

	if(vFtpNameSet)	{
		if(vFtpNameDataSet.value != ""){
			vFtpNameSet.open("GET", "/ftpname_value=" + vFtpNameDataSet.value + "&id=" + Math.random(), true);
			vFtpNameSet.send(null);
		}
	}
	
}

function LoadFrame() {

	DEVIPStatus_get();	
	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}


