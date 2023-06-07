/**
 * @author SonLQ4
 */

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
				vConfigbattCap1 = BATTCFGStatus.responseText.substring(tam[6]+1,tam[7]);	
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
				
				vCfgbtEndTime = BATTCFGStatus.responseText.substring(tam[18]+1,tam[19]);
				
				vConfigbattLoTemp = BATTCFGStatus.responseText.substring(tam[28]+1,tam[29]);	
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
				vConfigbattLoTemp = parseFloat(vConfigbattLoTemp/100).toFixed(2);	
				
				vCfgbtEndVolt = parseFloat(vCfgbtEndVolt/100).toFixed(2);
								
				setRowText('idconfigbatt_CCL', vConfigbattCCL);				
				setRowText('idconfigbatt_FVolt', vConfigbattFVolt);
				setRowText('idconfigbatt_BVolt', vConfigbattBVolt);
				setRowText('idconfigbatt_DCLow', vConfigbattDCLow);		
				
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
				setRowText('idconfigbatt_Cap1', vConfigbattCap1);
				setRowText('idconfigbatt_Cap2', vConfigbattCap2);
				setRowText('idconfigbatt_HiTemp', vConfigbattHiTemp);
				setRowText('idconfigbatt_LowThres', vConfigacLow);
				
				setRowText('idconfigbatt_LowThres', vConfigacLow);				
				setRowText('idconfigbatt_LowThres', vConfigacLow);
				
				setRowText('idcfgbt_EndVolt', vCfgbtEndVolt);
				
				setRowText('idcfgbt_EndTime', vCfgbtEndTime);
				
				if(vCfgDCCabinetType != 2){
					setVisible('ConfigForm',0);
				}else{
					setVisible('ConfigForm',1);
				}
			/* 	if(vConfigrectWITE == 1) {
				setRowText('idconfigrect_WITE', 'Enable');
				} else if (vConfigrectWITE == 0) {
				setRowText('idconfigrect_WITE', 'Disable');
				}				
				setRowText('idconfigrect_WITI', vConfigrectWITI);		 */						
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
		if(isValidCfgFloat2('Float Voltage',vBattCCLDataSet,0.05, 0.25) != 0){
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
			if(isValidCfgFloatA('Float Voltage',vBattFVDataSet,parseFloat(parseFloat(vConfigbattDCLow) + 2).toFixed(2), vConfigbattBVolt) != 0){
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
		if(isValidCfgFloatU('Boost Voltage',vBattBVDataSet,vConfigbattFVolt, 57.60) != 0){
			vBattBVSet.open("GET", "/battBV_value=" + vBattBVDataSet.toString() + "&id=" + Math.random(), true);
			vBattBVSet.send(null);
		}
	}	
}

function fBattDCLowSet (){
	var vBattDCLowSet = false;
	var temp1 = getValue('cfgdcav');
	var temp2 = parseFloat(temp1).toFixed(1);
	var vBattDCLowDataSet;
	
	document.getElementById('cfgdcav').value = temp2;
	temp1 = getValue('cfgdcav');
	vBattDCLowDataSet =parseInt( parseFloat(temp1) * 100);
	
	if(window.XMLHttpRequest){
		vBattDCLowSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattDCLowSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattDCLowSet)	{
		if(isValidCfgFloatA('DC Low Alarm Voltage',vBattDCLowDataSet,vConfigbattLLVD, parseFloat(parseFloat(vConfigbattFVolt) - 2).toFixed(2)) != 0){
			vBattDCLowSet.open("GET", "/battDCLow_value=" + vBattDCLowDataSet.toString() + "&id=" + Math.random(), true);
			vBattDCLowSet.send(null);
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
	var temp2 = parseFloat(temp1).toFixed(1);
	var vBattLLVDDataSet;
	
	document.getElementById('cfglvdav').value = temp2;
	temp1 = getValue('cfglvdav');
	vBattLLVDDataSet = parseInt(parseFloat(temp1) * 100);
	
	if(window.XMLHttpRequest){
		vBattLLVDSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattLLVDSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattLLVDSet)	{
		if(isValidCfgFloatU('LLVD Alarm Voltage',vBattLLVDDataSet,vConfigbattBLVD, vConfigbattDCLow) != 0){
			vBattLLVDSet.open("GET", "/battLLVD_value=" + vBattLLVDDataSet.toString() + "&id=" + Math.random(), true);
			vBattLLVDSet.send(null);
		}
	}	
}
function fBattBLVDSet (){
	var vBattBLVDSet = false;
	var temp1 = getValue('cfgbvdav');
	var temp2 = parseFloat(temp1).toFixed(1);
	var vBattBLVDDataSet;
	
	document.getElementById('cfgbvdav').value = temp2;
	temp1 = getValue('cfgbvdav');
	vBattBLVDDataSet = parseInt(parseFloat(temp1) * 100);
	
	if(window.XMLHttpRequest){
		vBattBLVDSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattBLVDSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattBLVDSet)	{
		if(isValidCfgFloatL('BLVD Alarm Voltage',vBattBLVDDataSet,35.00, vConfigbattLLVD) != 0){
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
function fBattCap1Set (){
	var vBattCap1Set = false;	
	var temp1 = getValue('cfgctot1');
	var vBattCap1DataSet = parseInt(temp1);	

	
	if(window.XMLHttpRequest){
		vBattCap1Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattCap1Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattCap1Set)	{
		if(isValidCfgInteger('Battery 1 Capacity',vBattCap1DataSet,5, 1000) != 0){
			vBattCap1Set.open("GET", "/battCap1_value=" + vBattCap1DataSet.toString() + "&id=" + Math.random(), true);
			vBattCap1Set.send(null);
		}
	}	
}
function fBattCap2Set (){
	var vBattCap2Set = false;	
	var temp1 = getValue('cfgctot2');
	var vBattCap2DataSet = parseInt(temp1);	

	
	if(window.XMLHttpRequest){
		vBattCap2Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattCap2Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattCap2Set)	{
		if(isValidCfgInteger('Battery 2 Capacity',vBattCap2DataSet,5, 1000) != 0){
			vBattCap2Set.open("GET", "/battCap2_value=" + vBattCap2DataSet.toString() + "&id=" + Math.random(), true);
			vBattCap2Set.send(null);
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
		if(isValidCfgFloatU('High Temp Alarm',vBattHiTempDataSet,vConfigbattLoTemp, 100) != 0){
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
		if(isValidCfgInteger('AC Low Level value',vAcLowThresDataSet,60, 299) != 0){
			vAcLowThresSet.open("GET", "/acLowThres_value=" + vAcLowThresDataSet.toString() + "&id=" + Math.random(), true);
			vAcLowThresSet.send(null);
		}
	}	
}


function fBTEndVoltSet (){
	var vBTEndVoltSet = false;
	var temp1 = getValue('cfgbtev');
	var temp2 = parseFloat(temp1).toFixed(2);
	var vBTEndVoltDataSet;
	
	document.getElementById('cfgbtev').value = temp2;
	temp1 = getValue('cfgbtev');
	vBTEndVoltDataSet = parseFloat(temp1) * 100;
		
		if(window.XMLHttpRequest){
			vBTEndVoltSet = new XMLHttpRequest();
		}
		else if(window.ActiveXObject){
			vBTEndVoltSet = new ActiveXObject("Microsoft.XMLHTTP");
		}		
	
		if(vBTEndVoltSet)	{
			if(isValidCfgFloat('End Voltage',vBTEndVoltDataSet,1.70, 2.20) != 0){
				vBTEndVoltSet.open("GET", "/btEndVolt_value=" + vBTEndVoltDataSet.toString() + "&id=" + Math.random(), true);
				vBTEndVoltSet.send(null);
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
		if(isValidCfgInteger('End Time',vBTEndTimeDataSet,1, 100) != 0){
			vBTEndTimeSet.open("GET", "/btEndTime_value=" + vBTEndTimeDataSet.toString() + "&id=" + Math.random(), true);
			vBTEndTimeSet.send(null);
		}
	}	
}

function fBTPLStartSet (){
	var vBTPLStartSet = false;
	
	if(window.XMLHttpRequest){
		vBTPLStartSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBTPLStartSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	
	var answer = confirm("This will START the battery test! Do you want to do this or Cancel?");
	if(answer == false)
	{
	alert("Battery Test DO NOT start!.");
	} else 
	{
		if(vBTPLStartSet)	{
			vBTPLStartSet.open("GET", "/btplstart?id="+ Math.random(), true);
			vBTPLStartSet.send(null);
		}	
	}
}

function fBTPLStopSet (){
	var vBTPLStopSet = false;
	
	if(window.XMLHttpRequest){
		vBTPLStopSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBTPLStopSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	
	var answer = confirm("This will STOP the battery test! Do you want to do this or Cancel?");
	if(answer == false)
	{
	alert("Battery Test DO NOT stop!.");
	} else 
	{
		if(vBTPLStopSet)	{
			vBTPLStopSet.open("GET", "/btplstop?id="+ Math.random(), true);
			vBTPLStopSet.send(null);
		}	
	}
}

function LoadFrame() {

	BATTCFGStatus_get();	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();
}


