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
                // thanhcm3 add for dpc-----------------------------------------------------------------
				vConfigbattCap3             = BATTCFGStatus.responseText.substring(tam[35]+1,tam[36]);
				vConfigbattCap4             = BATTCFGStatus.responseText.substring(tam[36]+1,tam[37]);
				vConfigfanDCStartingPoint   = BATTCFGStatus.responseText.substring(tam[37]+1,tam[38]);
				vConfigfanDCSensivivePoint  = BATTCFGStatus.responseText.substring(tam[38]+1,tam[39]);
                vConfigfanAlarmHighTemp     = BATTCFGStatus.responseText.substring(tam[39]+1,tam[40]);
                vConfigfanAlarmLowTemp      = BATTCFGStatus.responseText.substring(tam[40]+1,tam[41]);
				vConfigbattCCL              = BATTCFGStatus.responseText.substring(tam[41]+1,tam[42]);
				vConfigfanDCHeatingSPoint   = BATTCFGStatus.responseText.substring(tam[42]+1,tam[43]);
				vConfigfanDCHeatSIPoint     = BATTCFGStatus.responseText.substring(tam[43]+1,tam[44]);
				//thanhcm3 add for dpc-------------------------------------------------------------------	
				vConfigbattCCL = parseFloat(vConfigbattCCL/100).toFixed(2);			
				vConfigbattFVolt = parseFloat(vConfigbattFVolt/100).toFixed(2);
				vConfigbattBVolt = parseFloat(vConfigbattBVolt/100).toFixed(2);
				vConfigbattDCLow = parseFloat(vConfigbattDCLow/100).toFixed(2);		
				vConfigbattLLVD = parseFloat(vConfigbattLLVD/100).toFixed(2);		
				vConfigbattBLVD = parseFloat(vConfigbattBLVD/100).toFixed(2);	
				vConfigbattHiTemp = parseFloat(vConfigbattHiTemp/100).toFixed(2);	
				vConfigbattOvTemp = parseFloat(vConfigbattOvTemp/100).toFixed(2);	
				
				vConfigbattTestVolt = parseFloat(vConfigbattTestVolt/100).toFixed(2);
				vConfigbattDCUnder = parseFloat(vConfigbattDCUnder/100).toFixed(2);	
				vConfigbattDCOVer = parseFloat(vConfigbattDCOVer/100).toFixed(2);

				vCfgbtEndVolt = parseFloat(vCfgbtEndVolt/10).toFixed(1);
				vCfgbtEndCap = parseFloat(vCfgbtEndCap/1000).toFixed(3);
				vConfigfanDCStartingPoint     =	parseFloat(vConfigfanDCStartingPoint/100).toFixed(2);
				vConfigfanDCSensivivePoint    =	parseFloat(vConfigfanDCSensivivePoint/100).toFixed(2);
				vConfigfanAlarmHighTemp       =	parseFloat(vConfigfanAlarmHighTemp/100).toFixed(2);
				vConfigfanAlarmLowTemp        =	parseFloat(vConfigfanAlarmLowTemp/100).toFixed(2);
				vConfigfanDCHeatingSPoint     = parseFloat(vConfigfanDCHeatingSPoint/100).toFixed(2);
				vConfigfanDCHeatSIPoint       = parseFloat(vConfigfanDCHeatSIPoint/100).toFixed(2);

				setRowText('idconfigbatt_CCL', vConfigbattCCL);			
				setRowText('idconfigbatt_FVolt', vConfigbattFVolt);	
				setRowText('idconfigbatt_BVolt', vConfigbattBVolt);
				setRowText('idconfigbatt_DCOver', vConfigbattDCOVer);	
				setRowText('idconfigbatt_DCLow', vConfigbattDCLow);	
				
				//setRowText('idconfigbatt_LLVD', vConfigbattLLVD);	
				//setRowText('idconfigbatt_BLVD', vConfigbattBLVD);
				setRowText('idconfigbatt_TempComp', vConfigbattTempComp);				
				setRowText('idconfigbatt_Cap1', vConfigbattCap1);      //thanhcm3 add for dpc
				setRowText('idconfigbatt_Cap2', vConfigbattCap2);      //thanhcm3 add for dpc
				setRowText('idconfigbatt_Cap3', vConfigbattCap3);      //thanhcm3 add for dpc
				setRowText('idconfigbatt_Cap4', vConfigbattCap4);      //thanhcm3 add for dpc
				//setRowText('idconfigbatt_HiTemp', vConfigbattHiTemp);
				setRowText('idconfigbatt_HiThres', vConfigacHi);
				setRowText('idconfigbatt_LowThres', vConfigacLow);
				
				//setRowText('idcfgbt_TestVolt', vConfigbattTestVolt);
				//setRowText('idcfgbt_EndCap', vCfgbtEndCap);
				//setRowText('idcfgbt_EndTime', vCfgbtEndTime);
				
				setRowText('idcfFanStartingPoint', vConfigfanDCStartingPoint);      //thanhcm3 add for dpc
				setRowText('idcfFanSensitivePoint', vConfigfanDCSensivivePoint);      //thanhcm3 add for dpc
				setRowText('idcfFanHighTempPoint', vConfigfanAlarmHighTemp);      //thanhcm3 add for dpc
				setRowText('idcfFanLowTempPoint', vConfigfanAlarmLowTemp);      //thanhcm3 add for dpc
				setRowText('idcfFanHeatStartPoint', vConfigfanDCHeatingSPoint);      //thanhcm3 add for dpc
				setRowText('idcfFanHeatSensitive', vConfigfanDCHeatSIPoint);      //thanhcm3 add for dpc
				
				if(vCfgDCCabinetType != 4){
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
		if(isValidCfgFloat('Float Voltage',vBattCCLDataSet,10.00, 999.0) != 0){
			vBattCCLSet.open("GET", "/battdpcCCL_value=" + vBattCCLDataSet.toString() + "&id=" + Math.random(), true);
			vBattCCLSet.send(null);
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
		if(isValidCfgFloat('DC Over Alarm Voltage',vBattDCOverDataSet,vConfigbattBVolt, 57.00) != 0){
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
		if(isValidCfgFloat('DC Low Alarm Voltage',vBattDCLowDataSet,50.00, vConfigbattFVolt) != 0){
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
		if(isValidCfgInteger('Temp Compensation value',vBattTempCompDataSet,1, 120) != 0){
			vBattTempCompSet.open("GET", "/battTempComp_value=" + vBattTempCompDataSet.toString() + "&id=" + Math.random(), true);
			vBattTempCompSet.send(null);
		}
	}	
}
/*
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
		if(isValidCfgInteger('Battery Capacity',vBattCapDataSet,50.00, 5000.00) != 0){
			vBattCapSet.open("GET", "/battCap1_value=" + vBattCapDataSet.toString() + "&id=" + Math.random(), true);
			vBattCapSet.send(null);
		}
	}	
}*/

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
		if(isValidCfgInteger('Battery 1 Capacity',vBattCap1DataSet,0, 600) != 0){
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
		if(isValidCfgInteger('Battery 2 Capacity',vBattCap2DataSet,0, 600) != 0){
			vBattCap2Set.open("GET", "/battCap2_value=" + vBattCap2DataSet.toString() + "&id=" + Math.random(), true);
			vBattCap2Set.send(null);
		}
	}	
}
function fBattCap3Set (){
	var vBattCap3Set = false;	
	var temp1 = getValue('cfgctot3');
	var vBattCap3DataSet = parseInt(temp1);	

	
	if(window.XMLHttpRequest){
		vBattCap3Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattCap3Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattCap3Set)	{
		if(isValidCfgInteger('Battery 3 Capacity',vBattCap3DataSet,0, 600) != 0){
			vBattCap3Set.open("GET", "/battCap3_value=" + vBattCap3DataSet.toString() + "&id=" + Math.random(), true);
			vBattCap3Set.send(null);
		}
	}	
}
function fBattCap4Set (){
	var vBattCap4Set = false;	
	var temp1 = getValue('cfgctot4');
	var vBattCap4DataSet = parseInt(temp1);	

	
	if(window.XMLHttpRequest){
		vBattCap4Set = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vBattCap4Set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vBattCap4Set)	{
		if(isValidCfgInteger('Battery 4 Capacity',vBattCap4DataSet,0, 600) != 0){
			vBattCap4Set.open("GET", "/battCap4_value=" + vBattCap4DataSet.toString() + "&id=" + Math.random(), true);
			vBattCap4Set.send(null);
		}
	}	
}
function fFanDCStartingPoint (){
	var vFanDCStartingPoint = false;	
	var temp1 = getValue('cffanstp');
	var vFanDCStartingPointdata = parseFloat(temp1)*100;	

	
	if(window.XMLHttpRequest){
		vFanDCStartingPoint = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vFanDCStartingPoint = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vFanDCStartingPoint)	{
		if(isValidCfgFloat('Fan starting point',vFanDCStartingPointdata,20.0, 40.0) != 0){
			vFanDCStartingPoint.open("GET", "/fanstartpoint_value=" + vFanDCStartingPointdata.toString() + "&id=" + Math.random(), true);
			vFanDCStartingPoint.send(null);
		}
	}	
}

function fFanDCsensivivePoint (){
	var vFanDCsensivivePoint = false;	
	var temp1 = getValue('cffansp');
	var vFanDCsensivivePointdata = parseFloat(temp1)*100;	

	
	if(window.XMLHttpRequest){
		vFanDCsensivivePoint = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vFanDCsensivivePoint = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vFanDCsensivivePoint)	{
		if(isValidCfgFloat('Fan sensivive point',vFanDCsensivivePointdata,0, 25.0) != 0){
			vFanDCsensivivePoint.open("GET", "/fansensitpoint_value=" + vFanDCsensivivePointdata.toString() + "&id=" + Math.random(), true);
			vFanDCsensivivePoint.send(null);
		}
	}	
}

function fFanAlarmHighTemp (){
	var vFanAlarmHighTemp = false;	
	var temp1 = getValue('cffanhtp');
	var vFanAlarmHighTempdata = parseFloat(temp1)*100;	

	
	if(window.XMLHttpRequest){
		vFanAlarmHighTemp = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vFanAlarmHighTemp = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vFanAlarmHighTemp)	{
		if(isValidCfgFloat('Fan high temp point',vFanAlarmHighTempdata,30.0, 60.0) != 0){
			vFanAlarmHighTemp.open("GET", "/fanalarmHtemp_value=" + vFanAlarmHighTempdata.toString() + "&id=" + Math.random(), true);
			vFanAlarmHighTemp.send(null);
		}
	}	
}

function fFanAlarmLowTemp (){
	var vFanAlarmLowTemp = false;	
	var temp1 = getValue('cffanltp');
	var vFanAlarmLowTempdata = parseFloat(temp1)*100;	

	
	if(window.XMLHttpRequest){
		vFanAlarmLowTemp = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vFanAlarmLowTemp = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vFanAlarmLowTemp)	{
		if(isValidCfgFloat('Fan low temp point',vFanAlarmLowTempdata,-45.0, 10.0) != 0){
			vFanAlarmLowTemp.open("GET", "/fanalarmLtemp_value=" + vFanAlarmLowTempdata.toString() + "&id=" + Math.random(), true);
			vFanAlarmLowTemp.send(null);
		}
	}	
}
//------------
function fFanDCHeatSPoint (){
	var vFanDCHeatSPoint = false;	
	var temp1 = getValue('cffanhsp');
	var vFanDCHeatSPointdata = parseFloat(temp1)*100;	

	
	if(window.XMLHttpRequest){
		vFanDCHeatSPoint = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vFanDCHeatSPoint = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vFanDCHeatSPoint)	{
		if(isValidCfgFloat('Fan sensivive point',vFanDCHeatSPointdata,-10.0, 15.0) != 0){
			vFanDCHeatSPoint.open("GET", "/fanHeatSpoint_value=" + vFanDCHeatSPointdata.toString() + "&id=" + Math.random(), true);
			vFanDCHeatSPoint.send(null);
		}
	}	
}
//---------------
function fFanDCHeatSI (){
	var vFanDCHeatSI = false;	
	var temp1 = getValue('cffanhsi');
	var vFanDCHeatSIdata = parseFloat(temp1)*100;	

	
	if(window.XMLHttpRequest){
		vFanDCHeatSI = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vFanDCHeatSI = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vFanDCHeatSI)	{
		if(isValidCfgFloat('Fan sensivive point',vFanDCHeatSIdata,0, 30.0) != 0){
			vFanDCHeatSI.open("GET", "/fanHeatSI_value=" + vFanDCHeatSIdata.toString() + "&id=" + Math.random(), true);
			vFanDCHeatSI.send(null);
		}
	}	
}
//----------------




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
		if(isValidCfgFloat('High Temp Alarm',vBattHiTempDataSet,10.00, 50.00) != 0){	
			vBattHiTempSet.open("GET", "/battHiTemp_value=" + vBattHiTempDataSet.toString() + "&id=" + Math.random(), true);
			vBattHiTempSet.send(null);
		}
	}	
}

function fBTTestVoltSet (){
	var vBTTestVoltSet = false;
	var temp1 = getValue('cfgbttv');
	var temp2 = parseFloat(temp1).toFixed(1);
	var vBTTestVoltDataSet;
	
	document.getElementById('cfgbttv').value = temp2;
	temp1 = getValue('cfgbttv');
	vBTTestVoltDataSet = parseFloat(temp1)*10;
		
		if(window.XMLHttpRequest){
			vBTTestVoltSet = new XMLHttpRequest();
		}
		else if(window.ActiveXObject){
			vBTTestVoltSet = new ActiveXObject("Microsoft.XMLHTTP");
		}		
	
		if(vBTTestVoltSet)	{
			if(isValidCfgFloat1('Test Voltage',vBTTestVoltDataSet,43.10, 57.90) != 0){
				vBTTestVoltSet.open("GET", "/btZTETestVolt_value=" + vBTTestVoltDataSet.toString() + "&id=" + Math.random(), true);
				vBTTestVoltSet.send(null);
			}
		}	
}

function fAcHiThresSet (){		
	var vAcHiThresSet = false;	
	var temp1 = getValue('cfgachi');
	var vAcHiThresDataSet = parseInt(temp1);	
	
	
	if(window.XMLHttpRequest){
		vAcHiThresSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vAcHiThresSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vAcHiThresSet)	{
		if(isValidCfgInteger('AC High Level value',vAcHiThresDataSet,210.00, 286.00) != 0){
			vAcHiThresSet.open("GET", "/acHiThres_value=" + vAcHiThresDataSet.toString() + "&id=" + Math.random(), true);
			vAcHiThresSet.send(null);
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
		if(isValidCfgInteger('AC Low Level value',vAcLowThresDataSet,154.00, 230,00) != 0){
			vAcLowThresSet.open("GET", "/acLowThres_value=" + vAcLowThresDataSet.toString() + "&id=" + Math.random(), true);
			vAcLowThresSet.send(null);
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
			if(isValidCfgFloat2('End Voltage',vBTEndCapDataSet,0, 1.0) != 0){
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

function LoadFrame() {

	BATTCFGStatus_get();	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();
}


