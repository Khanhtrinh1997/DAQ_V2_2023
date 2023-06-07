/**
 * @author Anvd33
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
	var vConfigbattCap1;
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

	var vConfigbattCap3;             //thanhcm3 add for dpc
	var vConfigbattCap4;
	var vConfigfanDCStartingPoint;   //thanhcm3 add for dpc
	var vConfigfanDCSensivivePoint;  //thanhcm3 add for dpc
	var vConfigfanAlarmHighTemp;     //thanhcm3 add for dpc
	var vConfigfanAlarmLowTemp;      //thanhcm3 add for dpc
	var vConfigfanDCHeatingSPoint;   //thanhcm3 add for dpc
	var vConfigfanDCHeatSIPoint;     //thanhcm3 add for dpc
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
				
				vConfigbattCCL 		= BATTCFGStatus.responseText.substring(0,tam[0]);			
				vConfigbattFVolt 	= BATTCFGStatus.responseText.substring(tam[0]+1,tam[1]);
				vConfigbattBVolt	= BATTCFGStatus.responseText.substring(tam[1]+1,tam[2]);
				vConfigbattDCLow 	= BATTCFGStatus.responseText.substring(tam[2]+1,tam[3]);		
				vConfigbattLLVD 	= BATTCFGStatus.responseText.substring(tam[3]+1,tam[4]);		
				vConfigbattBLVD 	= BATTCFGStatus.responseText.substring(tam[4]+1,tam[5]);
				vConfigbattTempComp = BATTCFGStatus.responseText.substring(tam[5]+1,tam[6]);	
				vConfigbattCap1 	= BATTCFGStatus.responseText.substring(tam[6]+1,tam[7]);	
				vConfigbattHiTemp	= BATTCFGStatus.responseText.substring(tam[7]+1,tam[8]);	
				vConfigrectWITE 	= BATTCFGStatus.responseText.substring(tam[8]+1,tam[9]);	
				vConfigrectWITI 	= BATTCFGStatus.responseText.substring(tam[9]+1,tam[10]);		
				vConfigacLow		= BATTCFGStatus.responseText.substring(tam[10]+1,tam[11]);	
				vConfigacUnder 		= BATTCFGStatus.responseText.substring(tam[11]+1,tam[12]);	
				vConfigacHi 		= BATTCFGStatus.responseText.substring(tam[12]+1,tam[13]);	
				vConfigbattOvTemp 	= BATTCFGStatus.responseText.substring(tam[13]+1,tam[14]);		
				
				vConfigbattTestVolt = BATTCFGStatus.responseText.substring(tam[14]+1,tam[15]);
				vConfigbattCap2 	= BATTCFGStatus.responseText.substring(tam[15]+1,tam[16]);	

				vCfgbtEndVolt 		= BATTCFGStatus.responseText.substring(tam[16]+1,tam[17]);
				vCfgbtEndCap 		= BATTCFGStatus.responseText.substring(tam[17]+1,tam[18]);
				vCfgbtEndTime 		= BATTCFGStatus.responseText.substring(tam[18]+1,tam[19]);		
				vCfgbtPlanTestEn 	= BATTCFGStatus.responseText.substring(tam[19]+1,tam[20]);
				
				vCfgbtManStart 		= BATTCFGStatus.responseText.substring(tam[20]+1,tam[21]);				
				vCfgbtPlanTest1 	= BATTCFGStatus.responseText.substring(tam[21]+1,tam[22]);	
				vCfgbtPlanTest2 	= BATTCFGStatus.responseText.substring(tam[22]+1,tam[23]);	
				vCfgbtPlanTest3 	= BATTCFGStatus.responseText.substring(tam[23]+1,tam[24]);	
				vCfgbtPlanTest4 	= BATTCFGStatus.responseText.substring(tam[24]+1,tam[25]);	
				
				vConfigbattDCUnder 	= BATTCFGStatus.responseText.substring(tam[25]+1,tam[26]);	
				vConfigbattDCOVer 	= BATTCFGStatus.responseText.substring(tam[26]+1,tam[27]);	
				vCfgbtSCUTime 		= BATTCFGStatus.responseText.substring(tam[27]+1,tam[28]);
				vCfgDCCabinetType 	= BATTCFGStatus.responseText.substring(tam[32]+1,tam[33]);
				vConfigbattLLVDEn 	= BATTCFGStatus.responseText.substring(tam[33]+1,tam[34]);
				vConfigbattBLVDEn 	= BATTCFGStatus.responseText.substring(tam[34]+1,tam[35]);
                
				vConfigbattCCL 				= parseFloat(vConfigbattCCL/100).toFixed(2);			
				vConfigbattFVolt 			= parseFloat(vConfigbattFVolt/100).toFixed(2);
				vConfigbattBVolt 			= parseFloat(vConfigbattBVolt/100).toFixed(2);
				vConfigbattDCLow 			= parseFloat(vConfigbattDCLow/100).toFixed(2);		
				vConfigbattLLVD 			= parseFloat(vConfigbattLLVD/100).toFixed(2);		
				vConfigbattBLVD 			= parseFloat(vConfigbattBLVD/100).toFixed(2);	
				vConfigbattHiTemp 			= parseFloat(vConfigbattHiTemp/100).toFixed(2);	
				vConfigbattOvTemp 			= parseFloat(vConfigbattOvTemp/100).toFixed(2);	
				
				vConfigbattTestVolt 		= parseFloat(vConfigbattTestVolt/100).toFixed(2);
				vConfigbattDCUnder 			= parseFloat(vConfigbattDCUnder/100).toFixed(2);	
				vConfigbattDCOVer 			= parseFloat(vConfigbattDCOVer/100).toFixed(2);

				vCfgbtEndVolt 				= parseFloat(vCfgbtEndVolt/10).toFixed(1);
				vCfgbtEndCap 				= parseFloat(vCfgbtEndCap/1000).toFixed(3);
				vConfigfanDCStartingPoint   = parseFloat(vConfigfanDCStartingPoint/100).toFixed(2);
				vConfigfanDCSensivivePoint  = parseFloat(vConfigfanDCSensivivePoint/100).toFixed(2);
				vConfigfanAlarmHighTemp     = parseFloat(vConfigfanAlarmHighTemp/100).toFixed(2);
				vConfigfanAlarmLowTemp      = parseFloat(vConfigfanAlarmLowTemp/100).toFixed(2);
				vConfigfanDCHeatingSPoint   = parseFloat(vConfigfanDCHeatingSPoint/100).toFixed(2);
				vConfigfanDCHeatSIPoint     = parseFloat(vConfigfanDCHeatSIPoint/100).toFixed(2);

				setRowText('idconfigbatt_CCL', vConfigbattCCL);			
				setRowText('idconfigbatt_FVolt', vConfigbattFVolt);	
				setRowText('idconfigbatt_BVolt', vConfigbattBVolt);
				setRowText('idconfigbatt_DCOver', vConfigbattDCOVer);	
				setRowText('idconfigbatt_DCLow', vConfigbattDCLow);				
				setRowText('idconfigbatt_Cap1', vConfigbattCap1); 	
				setRowText('idconfigbatt_DCUnder', vConfigbattDCUnder);				 
				setRowText('idconfigbatt_TempComp', vConfigbattTempComp);
				setRowText('idconfigbatt_HiTemp', vConfigbattHiTemp);
			 	if(vConfigrectWITE == 1) {
				setRowText('idconfigrect_WITE', 'Enable');
				} else if (vConfigrectWITE == 0) {
				setRowText('idconfigrect_WITE', 'Disable');
				}				
				setRowText('idconfigrect_WITI', vConfigrectWITI);		 				    
				
				if(vCfgDCCabinetType != 5){
					setVisible('ConfigForm',0);
				}else{
					setVisible('dhcConfigFormpinfo',1);
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

function fBattCCLSet (){
	var vBattCCLSet = false;
	var temp1 = getValue('cfgcl');
	var temp2 = parseFloat(temp1).toFixed(2);
	var vBattCCLDataSet;
	
	
	document.getElementById('cfgcl').value = temp2;
	temp1 = getValue('cfgcl');
	vBattCCLDataSet = parseFloat(temp1) * 100;
	
	if(window.XMLHttpRequest){
		vBattCCLSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattCCLSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattCCLSet)	{
		if(isValidCfgFloat('Current Limitted',vBattCCLDataSet,0.05,0.99) != 0){
			vBattCCLSet.open("GET", "/battCCL_value=" + vBattCCLDataSet.toString() + "&id=" + Math.random(), true);
			vBattCCLSet.send(null);
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
function fBattFVSet (){//
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
			if(isValidCfgFloat('Float Voltage',vBattFVDataSet,vConfigbattDCLow, vConfigbattBVolt) != 0){
				vBattFVSet.open("GET", "/battFV_value=" + vBattFVDataSet.toString() + "&id=" + Math.random(), true);
				vBattFVSet.send(null);
			}
		}		
}
function fBattBVSet (){//
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
		if(isValidCfgFloat('Boost Voltage',vBattBVDataSet,vConfigbattFVolt, vConfigbattDCOVer) != 0){
			vBattBVSet.open("GET", "/battBV_value=" + vBattBVDataSet.toString() + "&id=" + Math.random(), true);
			vBattBVSet.send(null);
		}
	}	
}

function fBattDCOverSet (){			
	var vBattDCOverSet = false;
	var temp1 = getValue('cfgdcov');
	var temp2 = parseFloat(temp1).toFixed(2);
	var vBattDCOverDataSet;
	
	document.getElementById('cfgdcov').value = temp2;
	temp1 = getValue('cfgdcov');
	vBattDCOverDataSet = parseFloat(temp1) * 100;
	
	if(window.XMLHttpRequest){
		vBattDCOverSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattDCUnderSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattDCOverSet)	{
		if(isValidCfgFloat('DC Over Alarm Voltage',vBattDCOverDataSet,vConfigbattBVolt, 58.00) != 0){
			vBattDCOverSet.open("GET", "/battDCOver_value=" + vBattDCOverDataSet.toString() + "&id=" + Math.random(), true);
			vBattDCOverSet.send(null);
		}
	}	
}

function fBattDCLowSet (){			
	var vBattDCLowSet = false;
	var temp1 = getValue('cfgdcav');
	var temp2 = parseFloat(temp1).toFixed(2);
	var vBattDCLowDataSet;
	var temp3 = vConfigbattDCOVer;
	//if (vConfigbattDCOVer>vConfigbattTestVolt) {temp3 = vConfigbattTestVolt - 1};
	
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
		if(isValidCfgFloat('DC Low Alarm Voltage',vBattDCLowDataSet,46.00, vConfigbattFVolt) != 0){
			vBattDCLowSet.open("GET", "/battDCLow_value=" + vBattDCLowDataSet.toString() + "&id=" + Math.random(), true);
			vBattDCLowSet.send(null);
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
		if(isValidCfgFloat('LLVD Alarm Voltage',vBattLLVDDataSet,vConfigbattBLVD, vConfigbattDCLow) != 0){	
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
		if(isValidCfgInteger('Temp Compensation value',vBattTempCompDataSet,1, 500) != 0){
			vBattTempCompSet.open("GET", "/battTempComp_value=" + vBattTempCompDataSet.toString() + "&id=" + Math.random(), true);
			vBattTempCompSet.send(null);
		}
	}	
}

function fBattCapSet (){
	var vBattCapSet = false;	
	var temp1 = getValue('cfgctot1');
	var vBattCapDataSet = parseInt(temp1);	

	
	if(window.XMLHttpRequest){
		vBattCapSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattCapSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattCapSet)	{
		if(isValidCfgInteger('Battery Capacity',vBattCapDataSet,50.00, 5000.00) != 0){
			vBattCapSet.open("GET", "/battCap1_value=" + vBattCapDataSet.toString() + "&id=" + Math.random(), true);
			vBattCapSet.send(null);
		}
	}	
}

function fBattHiTempSet (){
	var vBattHiTempSet = false;
	var temp1 = getValue('cfghmjta');
	var temp2 = parseFloat(temp1).toFixed(2);
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
		if(isValidCfgFloatU('High Temp Alarm',vBattHiTempDataSet,0, 100) != 0){
			vBattHiTempSet.open("GET", "/battHiTemp_value=" + vBattHiTempDataSet.toString() + "&id=" + Math.random(), true);
			vBattHiTempSet.send(null);
		}
	}	
}
function LoadFrame() {

	BATTCFGStatus_get();	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();
}


