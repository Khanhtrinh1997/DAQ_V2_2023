/**
 * @author SonLQ4
 */

function SNMPStatus_get()
{
	var SNMPStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var s_atsID=Array(3);			
	var s_atsACGen1;
	var s_atsACGen2;
	var s_atsG1Blk;
	var s_atsG2Blk;
	var s_atsRR;
	var s_atsMode;

	var nguyen;

	function SNMPStatusComplete()
	{
		if(SNMPStatus.readyState == 4)
		{
			if(SNMPStatus.status == 200)
			{
				text = SNMPStatus.responseText;
				length = SNMPStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}
				
				vSnmpTrapEnb = SNMPStatus.responseText.substring(0,tam[0]);			
				vSnmpTrapInt = SNMPStatus.responseText.substring(tam[0]+1,tam[1]);
				vSnmpServerIP = SNMPStatus.responseText.substring(tam[1]+1,tam[2]);				
						
				if (vSnmpTrapEnb == 0)
				{
					setRowText('idsnmp_TrapEnb', "Off-Trap");
				}	
				else if(vSnmpTrapEnb == 1)
				{
					setRowText('idsnmp_TrapEnb', "One-Shot");	
				}
				else if(vSnmpTrapEnb == 2)
				{
					setRowText('idsnmp_TrapEnb', "Continuous");	
				} 				
				setRowText('idsnmp_TrapInt', vSnmpTrapInt);
				setRowText('idsnmp_ServerIP', vSnmpServerIP);
								
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		SNMPStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		SNMPStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(SNMPStatus)
	{
		SNMPStatus.open("GET","/snmp_status?=", true);
		SNMPStatus.onreadystatechange = SNMPStatusComplete;
		SNMPStatus.send(null);
	}
t=setTimeout("SNMPStatus_get()",5000);
}
function fTrapModeSet (){
	var vTrapModeSet = false;
	var vTrapModeDataSet = document.getElementById("telnetlp");
	
	if(window.XMLHttpRequest){
		vTrapModeSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vTrapModeSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vTrapModeSet)	{
		if(vTrapModeDataSet.value != ""){
			vTrapModeSet.open("GET", "/trapmode_value=" + vTrapModeDataSet.value + "&id=" + Math.random(), true);
			vTrapModeSet.send(null);
		}
	}	
}

function fTrapTimeSet (){
	var vTrapTimeSet = false;
	var vTrapTimeDataSet = document.getElementById("telnett");
	
	if(window.XMLHttpRequest){
		vTrapTimeSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vTrapTimeSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vTrapTimeSet)	{
		if(vTrapTimeDataSet.value != ""){
			vTrapTimeSet.open("GET", "/traptime_value=" + vTrapTimeDataSet.value + "&id=" + Math.random(), true);
			vTrapTimeSet.send(null);
		}
	}	
}

function fServerIPSet () {
	var vIP1Set = false;
	var vIP1DataSet = document.getElementById("telnetip1");
	var vIP2Set = false;
	var vIP2DataSet = document.getElementById("telnetip2");	
	var vIP3Set = false;
	var vIP3DataSet = document.getElementById("telnetip3");
	var vIP4Set = false;
	var vIP4DataSet = document.getElementById("telnetip4");
	

	if(window.XMLHttpRequest){
		vIP1Set = new XMLHttpRequest();
		vIP2Set = new XMLHttpRequest();
		vIP3Set = new XMLHttpRequest();
		vIP4Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vIP1Set = new ActiveXObject("Microsoft.XMLHTTP");
		vIP2Set = new ActiveXObject("Microsoft.XMLHTTP");
		vIP3Set = new ActiveXObject("Microsoft.XMLHTTP");
		vIP4Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vIP1Set)	{
		if(vIP1DataSet.value != ""){
			vIP1Set.open("GET", "/telnetip1_value=" + vIP1DataSet.value + "&id=" + Math.random(), true);
			vIP1Set.send(null);
		}
	}
	if(vIP2Set)	{
		if(vIP2DataSet.value != ""){
			vIP2Set.open("GET", "/telnetip2_value=" + vIP2DataSet.value + "&id=" + Math.random(), true);
			vIP2Set.send(null);
		}
	}
	if(vIP3Set)	{
		if(vIP3DataSet.value != ""){
			vIP3Set.open("GET", "/telnetip3_value=" + vIP3DataSet.value + "&id=" + Math.random(), true);
			vIP3Set.send(null);
		}
	}
	if(vIP4Set){
		if(vIP4DataSet.value != ""){
			vIP4Set.open("GET", "/telnetip4_value=" + vIP4DataSet.value + "&id=" + Math.random(), true);
			vIP4Set.send(null);
		}
	}		
}

function LoadFrame() {

	SNMPStatus_get();	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();
}


