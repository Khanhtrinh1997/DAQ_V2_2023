/**
 * @author SonLQ4
 */
var vConfigrectWITE;
	var vConfigrectWITI;
	var vConfigbattCCL;
	var vConfigbattFVolt;
	var vConfigbattBVolt;
	var vConfigbattDCLow;
	
	var vConfigbattLLVDEn; // Add 29/09/17
	var vConfigbattBLVDEn; // Add 29/09/17
	
	var vConfigbattLLVD;
	var vConfigbattBLVD;
	var vConfigbattTempComp;
	var vConfigbattCap;
	var vConfigbattHiTemp;
	var vConfigacLow;
	var vConfigacUnder;
	var vConfigacHi;
	var vConfigbattOvTemp;
	var vConfigbattTestVolt;
	var vConfigbattCap2;
	var vCfgbtEndVolt;
	var vCfgbtEndCap;	
	var vCfgbtEndTime;
	var vCfgbtPlanTestEn;
	var vCfgbtManStart;
	var vCfgbtPlanTest1;
	var vCfgbtPlanTest2;
	var vCfgbtPlanTest3;
	var vCfgbtPlanTest4;

	var vConfigbattDCUnder;
	var vConfigbattDCOVer;	
	var vCfgbtSCUTime;	
	var vCfgDCCabinetType;
	
function BATTCFGStatus_get()
{
	var BATTCFGStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);
	var nguyen;	
	
	function BATTCFGStatusComplete()
	{
		if(BATTCFGStatus.readyState == 4)
		{
			if(BATTCFGStatus.status == 200)
			{
				text = BATTCFGStatus.responseText;
				length = BATTCFGStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}
				
				vConfigbattCCL = BATTCFGStatus.responseText.substring(0,tam[0]);			
				vConfigbattFVolt = BATTCFGStatus.responseText.substring(tam[0]+1,tam[1]);
				vConfigbattBVolt = BATTCFGStatus.responseText.substring(tam[1]+1,tam[2]);
				vConfigbattDCLow = BATTCFGStatus.responseText.substring(tam[2]+1,tam[3]);		
				vConfigbattLLVD = BATTCFGStatus.responseText.substring(tam[3]+1,tam[4]);		
				vConfigbattBLVD = BATTCFGStatus.responseText.substring(tam[4]+1,tam[5]);
				vConfigbattTempComp = BATTCFGStatus.responseText.substring(tam[5]+1,tam[6]);	
				vConfigbattCap = BATTCFGStatus.responseText.substring(tam[6]+1,tam[7]);	
				vConfigbattHiTemp = BATTCFGStatus.responseText.substring(tam[7]+1,tam[8]);	
				vConfigrectWITE = BATTCFGStatus.responseText.substring(tam[8]+1,tam[9]);	
				vConfigrectWITI = BATTCFGStatus.responseText.substring(tam[9]+1,tam[10]);		
				vConfigacLow = BATTCFGStatus.responseText.substring(tam[10]+1,tam[11]);	
				vConfigacUnder = BATTCFGStatus.responseText.substring(tam[11]+1,tam[12]);	
				vConfigacHi = BATTCFGStatus.responseText.substring(tam[12]+1,tam[13]);	
				vConfigbattOvTemp = BATTCFGStatus.responseText.substring(tam[13]+1,tam[14]);		
				
				vConfigbattTestVolt = BATTCFGStatus.responseText.substring(tam[14]+1,tam[15]);
				vConfigbattCap2 = BATTCFGStatus.responseText.substring(tam[15]+1,tam[16]);	

				vCfgbtEndVolt = BATTCFGStatus.responseText.substring(tam[16]+1,tam[17]);
				vCfgbtEndCap = BATTCFGStatus.responseText.substring(tam[17]+1,tam[18]);
				vCfgbtEndTime = BATTCFGStatus.responseText.substring(tam[18]+1,tam[19]);		
				vCfgbtPlanTestEn = BATTCFGStatus.responseText.substring(tam[19]+1,tam[20]);
				
				vCfgbtManStart = BATTCFGStatus.responseText.substring(tam[20]+1,tam[21]);				
				vCfgbtPlanTest1 = BATTCFGStatus.responseText.substring(tam[21]+1,tam[22]);	
				vCfgbtPlanTest2 = BATTCFGStatus.responseText.substring(tam[22]+1,tam[23]);	
				vCfgbtPlanTest3 = BATTCFGStatus.responseText.substring(tam[23]+1,tam[24]);	
				vCfgbtPlanTest4 = BATTCFGStatus.responseText.substring(tam[24]+1,tam[25]);	
				
				vConfigbattDCUnder = BATTCFGStatus.responseText.substring(tam[25]+1,tam[26]);	
				vConfigbattDCOVer = BATTCFGStatus.responseText.substring(tam[26]+1,tam[27]);	
				vCfgbtSCUTime = BATTCFGStatus.responseText.substring(tam[27]+1,tam[28]);
				vCfgDCCabinetType = BATTCFGStatus.responseText.substring(tam[32]+1,tam[33]);
				vConfigbattLLVDEn = BATTCFGStatus.responseText.substring(tam[33]+1,tam[34]);
				vConfigbattBLVDEn = BATTCFGStatus.responseText.substring(tam[34]+1,tam[35]);
				
				
				vConfigbattCCL = parseFloat(vConfigbattCCL/1000).toFixed(3);			
				vConfigbattFVolt = parseFloat(vConfigbattFVolt/100).toFixed(2);
				vConfigbattBVolt = parseFloat(vConfigbattBVolt/100).toFixed(2);
				vConfigbattDCLow = parseFloat(vConfigbattDCLow/100).toFixed(2);		
				vConfigbattLLVD = parseFloat(vConfigbattLLVD/100).toFixed(2);		
				vConfigbattBLVD = parseFloat(vConfigbattBLVD/100).toFixed(2);	
				vConfigbattHiTemp = parseFloat(vConfigbattHiTemp/100).toFixed(2);	
				vConfigbattOvTemp = parseFloat(vConfigbattOvTemp/100).toFixed(2);	
				
				vConfigbattDCUnder = parseFloat(vConfigbattDCUnder/100).toFixed(2);	
				vConfigbattDCOVer = parseFloat(vConfigbattDCOVer/100).toFixed(2);

				vCfgbtEndVolt = parseFloat(vCfgbtEndVolt/10).toFixed(1);
				vCfgbtEndCap = parseFloat(vCfgbtEndCap/1000).toFixed(3);	
				

				if(vConfigrectWITE == 1) {
				setRowText('idconfigrect_WITE', 'Enable');
				} else if (vConfigrectWITE == 0) {
				setRowText('idconfigrect_WITE', 'Disable');
				}				
				setRowText('idconfigrect_WITI', vConfigrectWITI);					
								
				setRowText('idconfigbatt_CCL', vConfigbattCCL);				
				setRowText('idconfigbatt_FVolt', vConfigbattFVolt);
				setRowText('idconfigbatt_BVolt', vConfigbattBVolt);
				setRowText('idconfigbatt_DCLow', vConfigbattDCLow);	
				setRowText('idconfigbatt_DCUnder', vConfigbattDCUnder);	
				
				if(vConfigbattLLVDEn == 1) {
					setRowText('idconfigbatt_LLVDE', 'Enable');
				} 
				else if (vConfigbattLLVDEn == 0) {
					setRowText('idconfigbatt_LLVDE', 'Disable');
				}
				
				if(vConfigbattBLVDEn == 1) {
					setRowText('idconfigbatt_BLVDE', 'Enable');
				} 
				else if (vConfigbattBLVDEn == 0) {
					setRowText('idconfigbatt_BLVDE', 'Disable');
				}
				
				setRowText('idconfigbatt_LLVD', vConfigbattLLVD);
				setRowText('idconfigbatt_BLVD', vConfigbattBLVD);
				setRowText('idconfigbatt_TempComp', vConfigbattTempComp);				
				setRowText('idconfigbatt_Cap', vConfigbattCap);
				setRowText('idconfigbatt_HiTemp', vConfigbattHiTemp);
				setRowText('idconfigbatt_LowThres', vConfigacLow);
				
				setRowText('idcfgbt_EndVolt', vCfgbtEndVolt);
				setRowText('idcfgbt_EndCap', vCfgbtEndCap);
				setRowText('idcfgbt_EndTime', vCfgbtEndTime);			
					


				if(vCfgbtPlanTestEn == 1) {
					setRowText('idcfgbt_PlanTestEn', 'Enable');
				} 
				else if (vCfgbtPlanTestEn == 0) {
					setRowText('idcfgbt_PlanTestEn', 'Disable');
				}	
				
				setRowText('idcfgbt_plan1', vCfgbtPlanTest1);
				setRowText('idcfgbt_plan2', vCfgbtPlanTest2);
				setRowText('idcfgbt_plan3', vCfgbtPlanTest3);
				setRowText('idcfgbt_plan4', vCfgbtPlanTest4);
				setRowText('idcfgbt_scutime', vCfgbtSCUTime);
				
				if(vCfgDCCabinetType != 0){
					setVisible('ConfigForm',0);
					setVisible('dhcpinfo',0);
				}else{
					setVisible('dhcConfigFormpinfo',1);
					setVisible('dhcpinfo',1);
				}
				
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		BATTCFGStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		BATTCFGStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(BATTCFGStatus)
	{
		BATTCFGStatus.open("GET","/battconfig_status?=", true);
		BATTCFGStatus.onreadystatechange = BATTCFGStatusComplete;
		BATTCFGStatus.send(null);
	}
t=setTimeout("BATTCFGStatus_get()",5000);
}

function fBattCCLSet (){
	var vBattCCLSet = false;
	var temp1 = getValue('cfgcl');
	var temp2 = parseFloat(temp1).toFixed(3);
	var vBattCCLDataSet;
	
	
	document.getElementById('cfgcl').value = temp2;
	temp1 = getValue('cfgcl');
	vBattCCLDataSet = parseFloat(temp1) * 1000;
	
	if(window.XMLHttpRequest){
		vBattCCLSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattCCLSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattCCLSet)	{
		if(isValidCfgFloat2('Float Voltage',vBattCCLDataSet,0.1, 0.25) != 0){
			vBattCCLSet.open("GET", "/battCCL_value=" + vBattCCLDataSet.toString() + "&id=" + Math.random(), true);
			vBattCCLSet.send(null);
		}
	}	
}

function fRectWITESet (){
	var vRectWITESet = false;
	var vRectWITEDataSet = document.getElementById("cfgwiten");
	
	if(window.XMLHttpRequest){
		vRectWITESet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vRectWITESet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vRectWITESet)	{
		if(vRectWITEDataSet.value != ""){
			vRectWITESet.open("GET", "/rectWITE_value=" + vRectWITEDataSet.value + "&id=" + Math.random(), true);
			vRectWITESet.send(null);
		}
	}
}

function fRectWITISet (){
	var vRectWITISet = false;
	var temp1 = getValue('cfgwit');
	var vRectWITIDataSet = parseInt(temp1);	
	
	if(window.XMLHttpRequest){
		vRectWITISet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vRectWITISet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vRectWITISet)	{
		if(isValidCfgInteger('Walk-In-Time Interval',vRectWITIDataSet,8, 128) != 0){
			vRectWITISet.open("GET", "/rectWITI_value=" + vRectWITIDataSet.toString() + "&id=" + Math.random(), true);
			vRectWITISet.send(null);
		}
	}	
}

function fBattFVSet (){
	var vBattFVSet = false;
	var temp1 = getValue('cfgfv');
	var temp2 = parseFloat(temp1).toFixed(2);
	var vBattFVDataSet;
	
	document.getElementById('cfgfv').value = temp2;
	temp1 = getValue('cfgfv');
	vBattFVDataSet = parseFloat(temp1) * 100;
		
		if(window.XMLHttpRequest){
			vBattFVSet = new XMLHttpRequest();
		}
		else if(window.ActiveXObject){
			vBattFVSet = new ActiveXObject("Microsoft.XMLHTTP");
		}
		
	
		if(vBattFVSet)	{
			if(isValidCfgFloat('Float Voltage',vBattFVDataSet,42.00, vConfigbattBVolt) != 0){
				vBattFVSet.open("GET", "/battFV_value=" + vBattFVDataSet.toString() + "&id=" + Math.random(), true);
				vBattFVSet.send(null);
			}
		}	
	
	
}
function fBattBVSet (){
	var vBattBVSet = false;
	var temp1 = getValue('cfgbv');
	var temp2 = parseFloat(temp1).toFixed(2);
	var vBattBVDataSet;
	
	document.getElementById('cfgbv').value = temp2;
	temp1 = getValue('cfgbv');
	vBattBVDataSet = parseFloat(temp1) * 100;
	
	if(window.XMLHttpRequest){
		vBattBVSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattBVSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattBVSet)	{
		if(isValidCfgFloat('Boost Voltage',vBattBVDataSet,vConfigbattFVolt, 58.00) != 0){
			vBattBVSet.open("GET", "/battBV_value=" + vBattBVDataSet.toString() + "&id=" + Math.random(), true);
			vBattBVSet.send(null);
		}
	}	
}

function fBattDCLowSet (){
	var vBattDCLowSet = false;
	var temp1 = getValue('cfgdcav');
	var temp2 = parseFloat(temp1).toFixed(2);
	var vBattDCLowDataSet;
	
	document.getElementById('cfgdcav').value = temp2;
	temp1 = getValue('cfgdcav');
	vBattDCLowDataSet = parseFloat(temp1) * 100;
	
	if(window.XMLHttpRequest){
		vBattDCLowSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattDCLowSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattDCLowSet)	{
		if(isValidCfgFloat('DC Low Alarm Voltage',vBattDCLowDataSet,vConfigbattDCUnder, vConfigbattDCOVer) != 0){
			vBattDCLowSet.open("GET", "/battDCLow_value=" + vBattDCLowDataSet.toString() + "&id=" + Math.random(), true);
			vBattDCLowSet.send(null);
		}
	}	
}

function fBattDCUnderSet (){
	var vBattDCUnderSet = false;
	var temp1 = getValue('cfgdcuv');
	var temp2 = parseFloat(temp1).toFixed(2);
	var vBattDCUnderDataSet;
	
	document.getElementById('cfgdcuv').value = temp2;
	temp1 = getValue('cfgdcuv');
	vBattDCUnderDataSet = parseFloat(temp1) * 100;
	
	if(window.XMLHttpRequest){
		vBattDCUnderSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattDCUnderSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattDCUnderSet)	{
		if(isValidCfgFloat('DC Under Alarm Voltage',vBattDCUnderDataSet,vConfigbattLLVD, vConfigbattDCLow) != 0){
			vBattDCUnderSet.open("GET", "/battDCUnder_value=" + vBattDCUnderDataSet.toString() + "&id=" + Math.random(), true);
			vBattDCUnderSet.send(null);
		}
	}	
}

function fBattLLVDESet (){
	var vBattLLVDESet = false;
	var vBattLLVDEDataSet = document.getElementById("cfgllvden");
	
	if(window.XMLHttpRequest){
		vBattLLVDESet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattLLVDESet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattLLVDESet)	{
		if(vBattLLVDEDataSet.value != ""){
			vBattLLVDESet.open("GET", "/battEnLLVD_value=" + vBattLLVDEDataSet.value + "&id=" + Math.random(), true);
			vBattLLVDESet.send(null);
		}
	}
}

function fBattBLVDESet (){
	var vBattBLVDESet = false;
	var vBattBLVDEDataSet = document.getElementById("cfgblvden");
	
	if(window.XMLHttpRequest){
		vBattBLVDESet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattBLVDESet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattBLVDESet)	{
		if(vBattBLVDEDataSet.value != ""){
			vBattBLVDESet.open("GET", "/battEnBLVD_value=" + vBattBLVDEDataSet.value + "&id=" + Math.random(), true);
			vBattBLVDESet.send(null);
		}
	}
}

function fBattLLVDSet (){
	var vBattLLVDSet = false;
	var temp1 = getValue('cfglvdav');
	var temp2 = parseFloat(temp1).toFixed(2);
	var vBattLLVDDataSet;
	
	document.getElementById('cfglvdav').value = temp2;
	temp1 = getValue('cfglvdav');
	vBattLLVDDataSet = parseFloat(temp1) * 100;
	
	if(window.XMLHttpRequest){
		vBattLLVDSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattLLVDSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattLLVDSet)	{
		if(isValidCfgFloat('LLVD Alarm Voltage',vBattLLVDDataSet,vConfigbattBLVD, vConfigbattDCUnder) != 0){
			vBattLLVDSet.open("GET", "/battLLVD_value=" + vBattLLVDDataSet.toString() + "&id=" + Math.random(), true);
			vBattLLVDSet.send(null);
		}
	}	
}
function fBattBLVDSet (){
	var vBattBLVDSet = false;
	var temp1 = getValue('cfgbvdav');
	var temp2 = parseFloat(temp1).toFixed(2);
	var vBattBLVDDataSet;
	
	document.getElementById('cfgbvdav').value = temp2;
	temp1 = getValue('cfgbvdav');
	vBattBLVDDataSet = parseFloat(temp1) * 100;
	
	if(window.XMLHttpRequest){
		vBattBLVDSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattBLVDSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattBLVDSet)	{
		if(isValidCfgFloat('BLVD Alarm Voltage',vBattBLVDDataSet,40.00, vConfigbattLLVD) != 0){
			vBattBLVDSet.open("GET", "/battBLVD_value=" + vBattBLVDDataSet.toString() + "&id=" + Math.random(), true);
			vBattBLVDSet.send(null);
		}
	}	
}
function fBattTempCompSet (){
	var vBattTempCompSet = false;	
	var temp1 = getValue('cfgtc');
	var vBattTempCompDataSet = parseInt(temp1);	
	
	
	if(window.XMLHttpRequest){
		vBattTempCompSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattTempCompSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattTempCompSet)	{
		if(isValidCfgInteger('Temp Compensation value',vBattTempCompDataSet,0, 500) != 0){
			vBattTempCompSet.open("GET", "/battTempComp_value=" + vBattTempCompDataSet.toString() + "&id=" + Math.random(), true);
			vBattTempCompSet.send(null);
		}
	}	
}
function fBattCapSet (){
	var vBattCapSet = false;	
	var temp1 = getValue('cfgctot');
	var vBattCapDataSet = parseInt(temp1);	

	
	if(window.XMLHttpRequest){
		vBattCapSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattCapSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattCapSet)	{
		if(isValidCfgInteger('Battery Capacity',vBattCapDataSet,50, 5000) != 0){
			vBattCapSet.open("GET", "/battCap1_value=" + vBattCapDataSet.toString() + "&id=" + Math.random(), true);
			vBattCapSet.send(null);
		}
	}	
}
function fBattHiTempSet (){
	var vBattHiTempSet = false;
	var temp1 = getValue('cfghmjta');
	var temp2 = parseFloat(temp1).toFixed(0);
	var vBattHiTempDataSet;
	
	document.getElementById('cfghmjta').value = temp2;
	temp1 = getValue('cfghmjta');
	vBattHiTempDataSet = parseFloat(temp1) * 100;
	
	if(window.XMLHttpRequest){
		vBattHiTempSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattHiTempSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattHiTempSet)	{
		if(isValidCfgFloat('High Temp Alarm',vBattHiTempDataSet,10.00, vConfigbattOvTemp) != 0){
			vBattHiTempSet.open("GET", "/battHiTemp_value=" + vBattHiTempDataSet.toString() + "&id=" + Math.random(), true);
			vBattHiTempSet.send(null);
		}
	}	
}

function fAcLowThresSet (){
	var vAcLowThresSet = false;	
	var temp1 = getValue('cfgaclow');
	var vAcLowThresDataSet = parseInt(temp1);	
	
	
	if(window.XMLHttpRequest){
		vAcLowThresSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vAcLowThresSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vAcLowThresSet)	{
		if(isValidCfgInteger('AC Low Level value',vAcLowThresDataSet,vConfigacUnder, vConfigacHi) != 0){
			vAcLowThresSet.open("GET", "/acLowThres_value=" + vAcLowThresDataSet.toString() + "&id=" + Math.random(), true);
			vAcLowThresSet.send(null);
		}
	}	
}

function fBTEndVoltSet (){
	var vBTEndVoltSet = false;
	var temp1 = getValue('cfgbtev');
	var temp2 = parseFloat(temp1).toFixed(1);
	var vBTEndVoltDataSet;
	
	document.getElementById('cfgbtev').value = temp2;
	temp1 = getValue('cfgbtev');
	vBTEndVoltDataSet = parseFloat(temp1) * 10;
		
		if(window.XMLHttpRequest){
			vBTEndVoltSet = new XMLHttpRequest();
		}
		else if(window.ActiveXObject){
			vBTEndVoltSet = new ActiveXObject("Microsoft.XMLHTTP");
		}		
	
		if(vBTEndVoltSet)	{
			if(isValidCfgFloat1('End Voltage',vBTEndVoltDataSet,43.1, 57.9) != 0){
				vBTEndVoltSet.open("GET", "/btEndVolt_value=" + vBTEndVoltDataSet.toString() + "&id=" + Math.random(), true);
				vBTEndVoltSet.send(null);
			}
		}	
}

function fBTEndCapSet (){
	var vBTEndCapSet = false;
	var temp1 = getValue('cfgbtec');
	var temp2 = parseFloat(temp1).toFixed(3);
	var vBTEndCapDataSet;
	
	document.getElementById('cfgbtec').value = temp2;
	temp1 = getValue('cfgbtec');
	vBTEndCapDataSet = parseFloat(temp1) * 1000;
		
		if(window.XMLHttpRequest){
			vBTEndCapSet = new XMLHttpRequest();
		}
		else if(window.ActiveXObject){
			vBTEndCapSet = new ActiveXObject("Microsoft.XMLHTTP");
		}		
	
		if(vBTEndCapSet)	{
			if(isValidCfgFloat2('End Voltage',vBTEndCapDataSet,0.1, 1.0) != 0){
				vBTEndCapSet.open("GET", "/btEndCap_value=" + vBTEndCapDataSet.toString() + "&id=" + Math.random(), true);
				vBTEndCapSet.send(null);
			}
		}	
}

function fBTEndTimeSet (){
	var vBTEndTimeSet = false;
	var temp1 = getValue('cfgbtet');
	var vBTEndTimeDataSet = parseInt(temp1);	
	
	if(window.XMLHttpRequest){
		vBTEndTimeSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBTEndTimeSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBTEndTimeSet)	{
		if(isValidCfgInteger('End Time',vBTEndTimeDataSet,5, 1440) != 0){
			vBTEndTimeSet.open("GET", "/btEndTime_value=" + vBTEndTimeDataSet.toString() + "&id=" + Math.random(), true);
			vBTEndTimeSet.send(null);
		}
	}	
}

function fBTModeSet (){
	var vBTModeSet = false;
	var vBTModeDataSet = document.getElementById("cfgbtm");
	
	if(window.XMLHttpRequest){
		vBTModeSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBTModeSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBTModeSet)	{
		if(vBTModeDataSet.value != ""){
			vBTModeSet.open("GET", "/btMode_value=" + vBTModeDataSet.value + "&id=" + Math.random(), true);
			vBTModeSet.send(null);
		}
	}	
}

function fBTPlanTestEnSet (){
	var vBTPlanTestEnSet = false;
	var vBTPlanTestEnDataSet = document.getElementById("cfgbtpte");
	
	if(window.XMLHttpRequest){
		vBTPlanTestEnSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBTPlanTestEnSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBTPlanTestEnSet)	{
		if(vBTPlanTestEnDataSet.value != ""){
			vBTPlanTestEnSet.open("GET", "/btPlanEn_value=" + vBTPlanTestEnDataSet.value + "&id=" + Math.random(), true);
			vBTPlanTestEnSet.send(null);
		}
	}	
}

function fBTManStartSet (){
	var vBTManStartSet = false;
	var vBTManStartDataSet = document.getElementById("cfgbtms");
	
	if(window.XMLHttpRequest){
		vBTManStartSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBTManStartSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBTManStartSet)	{
		if(vBTManStartDataSet.value != ""){
			vBTManStartSet.open("GET", "/btManStart_value=" + vBTManStartDataSet.value + "&id=" + Math.random(), true);
			vBTManStartSet.send(null);
		}
	}	
}

function fBTPL1Set () {
	var vBTPL1mSet = false;
	var vBTPL1mDataSet = document.getElementById("cfgbpt1m");
	var vBTPL1dSet = false;
	var vBTPL1dDataSet = document.getElementById("cfgbpt1d");	
	var vBTPL1hSet = false;
	var vBTPL1hDataSet = document.getElementById("cfgbpt1h");	

	if(window.XMLHttpRequest){
		vBTPL1mSet = new XMLHttpRequest();
		vBTPL1dSet = new XMLHttpRequest();
		vBTPL1hSet = new XMLHttpRequest();		
	}
	else if(window.ActiveXObject){
		vBTPL1mSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTPL1dSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTPL1hSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBTPL1mSet)	{
		if(vBTPL1mDataSet.value != ""){
			vBTPL1mSet.open("GET", "/plantest1m_value=" + vBTPL1mDataSet.value + "&id=" + Math.random(), true);
			vBTPL1mSet.send(null);
		}
	}
	if(vBTPL1dSet)	{
		if(vBTPL1dDataSet.value != ""){
			vBTPL1dSet.open("GET", "/plantest1d_value=" + vBTPL1dDataSet.value + "&id=" + Math.random(), true);
			vBTPL1dSet.send(null);
		}
	}
	if(vBTPL1hSet)	{
		if(vBTPL1hDataSet.value != ""){
			vBTPL1hSet.open("GET", "/plantest1h_value=" + vBTPL1hDataSet.value + "&id=" + Math.random(), true);
			vBTPL1hSet.send(null);
		}
	}			
}

function fBTPL2Set () {
	var vBTPL2mSet = false;
	var vBTPL2mDataSet = document.getElementById("cfgbpt2m");
	var vBTPL2dSet = false;
	var vBTPL2dDataSet = document.getElementById("cfgbpt2d");	
	var vBTPL2hSet = false;
	var vBTPL2hDataSet = document.getElementById("cfgbpt2h");	

	if(window.XMLHttpRequest){
		vBTPL2mSet = new XMLHttpRequest();
		vBTPL2dSet = new XMLHttpRequest();
		vBTPL2hSet = new XMLHttpRequest();		
	}
	else if(window.ActiveXObject){
		vBTPL2mSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTPL2dSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTPL2hSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBTPL2mSet)	{
		if(vBTPL2mDataSet.value != ""){
			vBTPL2mSet.open("GET", "/plantest2m_value=" + vBTPL2mDataSet.value + "&id=" + Math.random(), true);
			vBTPL2mSet.send(null);
		}
	}
	if(vBTPL2dSet)	{
		if(vBTPL2dDataSet.value != ""){
			vBTPL2dSet.open("GET", "/plantest2d_value=" + vBTPL2dDataSet.value + "&id=" + Math.random(), true);
			vBTPL2dSet.send(null);
		}
	}
	if(vBTPL2hSet)	{
		if(vBTPL2hDataSet.value != ""){
			vBTPL2hSet.open("GET", "/plantest2h_value=" + vBTPL2hDataSet.value + "&id=" + Math.random(), true);
			vBTPL2hSet.send(null);
		}
	}			
}

function fBTPL3Set () {
	var vBTPL3mSet = false;
	var vBTPL3mDataSet = document.getElementById("cfgbpt3m");
	var vBTPL3dSet = false;
	var vBTPL3dDataSet = document.getElementById("cfgbpt3d");	
	var vBTPL3hSet = false;
	var vBTPL3hDataSet = document.getElementById("cfgbpt3h");	

	if(window.XMLHttpRequest){
		vBTPL3mSet = new XMLHttpRequest();
		vBTPL3dSet = new XMLHttpRequest();
		vBTPL3hSet = new XMLHttpRequest();		
	}
	else if(window.ActiveXObject){
		vBTPL3mSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTPL3dSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTPL3hSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBTPL3mSet)	{
		if(vBTPL3mDataSet.value != ""){
			vBTPL3mSet.open("GET", "/plantest3m_value=" + vBTPL3mDataSet.value + "&id=" + Math.random(), true);
			vBTPL3mSet.send(null);
		}
	}
	if(vBTPL3dSet)	{
		if(vBTPL3dDataSet.value != ""){
			vBTPL3dSet.open("GET", "/plantest3d_value=" + vBTPL3dDataSet.value + "&id=" + Math.random(), true);
			vBTPL3dSet.send(null);
		}
	}
	if(vBTPL3hSet)	{
		if(vBTPL3hDataSet.value != ""){
			vBTPL3hSet.open("GET", "/plantest3h_value=" + vBTPL3hDataSet.value + "&id=" + Math.random(), true);
			vBTPL3hSet.send(null);
		}
	}			
}

function fBTPL4Set () {
	var vBTPL4mSet = false;
	var vBTPL4mDataSet = document.getElementById("cfgbpt4m");
	var vBTPL4dSet = false;
	var vBTPL4dDataSet = document.getElementById("cfgbpt4d");	
	var vBTPL4hSet = false;
	var vBTPL4hDataSet = document.getElementById("cfgbpt4h");	

	if(window.XMLHttpRequest){
		vBTPL4mSet = new XMLHttpRequest();
		vBTPL4dSet = new XMLHttpRequest();
		vBTPL4hSet = new XMLHttpRequest();		
	}
	else if(window.ActiveXObject){
		vBTPL4mSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTPL4dSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTPL4hSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBTPL4mSet)	{
		if(vBTPL4mDataSet.value != ""){
			vBTPL4mSet.open("GET", "/plantest4m_value=" + vBTPL4mDataSet.value + "&id=" + Math.random(), true);
			vBTPL4mSet.send(null);
		}
	}
	if(vBTPL4dSet)	{
		if(vBTPL4dDataSet.value != ""){
			vBTPL4dSet.open("GET", "/plantest4d_value=" + vBTPL4dDataSet.value + "&id=" + Math.random(), true);
			vBTPL4dSet.send(null);
		}
	}
	if(vBTPL4hSet)	{
		if(vBTPL4hDataSet.value != ""){
			vBTPL4hSet.open("GET", "/plantest4h_value=" + vBTPL4hDataSet.value + "&id=" + Math.random(), true);
			vBTPL4hSet.send(null);
		}
	}			
}

function fBTSCUTimeSet () {
	var vBTSCUTimeySet = false;	
	var temp1 = getValue('cfgbscuy');
	var vBTSCUTimeyDataSet = parseInt(temp1);	
	
/* 	var vBTSCUTimeySet = false;
	var vBTSCUTimeyDataSet = document.getElementById("cfgbscuy"); */
	var vBTSCUTimemSet = false;
	var vBTSCUTimemDataSet = document.getElementById("cfgbscum");
	var vBTSCUTimedSet = false;
	var vBTSCUTimedDataSet = document.getElementById("cfgbscud");	
	var vBTSCUTimehSet = false;
	var vBTSCUTimehDataSet = document.getElementById("cfgbscuh");
	var vBTSCUTimepSet = false;
	var vBTSCUTimepDataSet = document.getElementById("cfgbscumi");
	var vBTSCUTimesSet = false;
	var vBTSCUTimesDataSet = document.getElementById("cfgbscus");	

	if(window.XMLHttpRequest){
		vBTSCUTimeySet = new XMLHttpRequest();
		vBTSCUTimemSet = new XMLHttpRequest();
		vBTSCUTimedSet = new XMLHttpRequest();
		vBTSCUTimehSet = new XMLHttpRequest();		
		vBTSCUTimepSet = new XMLHttpRequest();
		vBTSCUTimesSet = new XMLHttpRequest();	
	}
	else if(window.ActiveXObject){
		vBTSCUTimeySet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTSCUTimemSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTSCUTimedSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTSCUTimehSet = new ActiveXObject("Microsoft.XMLHTTP");		
		vBTSCUTimepSet = new ActiveXObject("Microsoft.XMLHTTP");
		vBTSCUTimesSet = new ActiveXObject("Microsoft.XMLHTTP");
	}	
	
	if(vBTSCUTimeySet)	{
		
		if(isValidCfgInteger('Year',vBTSCUTimeyDataSet,2000, 2099) != 0){
			vBTSCUTimeySet.open("GET", "/scutimey_value=" + vBTSCUTimeyDataSet.toString() + "&id=" + Math.random(), true);
			vBTSCUTimeySet.send(null);
		}
	}
	if(vBTSCUTimemSet)	{
		if(vBTSCUTimemDataSet.value != ""){
			vBTSCUTimemSet.open("GET", "/scutimem_value=" + vBTSCUTimemDataSet.value + "&id=" + Math.random(), true);
			vBTSCUTimemSet.send(null);
		}
	}
	if(vBTSCUTimedSet)	{
		if(vBTSCUTimedDataSet.value != ""){
			vBTSCUTimedSet.open("GET", "/scutimed_value=" + vBTSCUTimedDataSet.value + "&id=" + Math.random(), true);
			vBTSCUTimedSet.send(null);
		}
	}
	if(vBTSCUTimehSet)	{
		if(vBTSCUTimehDataSet.value != ""){
			vBTSCUTimehSet.open("GET", "/scutimeh_value=" + vBTSCUTimehDataSet.value + "&id=" + Math.random(), true);
			vBTSCUTimehSet.send(null);
		}
	}	
	if(vBTSCUTimepSet)	{
		if(vBTSCUTimepDataSet.value != ""){
			vBTSCUTimepSet.open("GET", "/scutimep_value=" + vBTSCUTimepDataSet.value + "&id=" + Math.random(), true);
			vBTSCUTimepSet.send(null);
		}
	}
	if(vBTSCUTimesSet)	{
		if(vBTSCUTimesDataSet.value != ""){
			vBTSCUTimesSet.open("GET", "/scutimes_value=" + vBTSCUTimesDataSet.value + "&id=" + Math.random(), true);
			vBTSCUTimesSet.send(null);
		}
	}		
}

function LoadFrame() {

	BATTCFGStatus_get();	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();
}


