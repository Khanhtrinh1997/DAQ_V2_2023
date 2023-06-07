/**
 * @author SonLQ4
 */

function PMStatus_get()
{
	var PMStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);

	var nguyen;

	function PMStatusComplete()
	{
		if(PMStatus.readyState == 4)
		{
			if(PMStatus.status == 200)
			{
				text = PMStatus.responseText;
				length = PMStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}	
			
				vPMConnectStatus = PMStatus.responseText.substring(0,tam[0]);			
				vPMInstalledPM = PMStatus.responseText.substring(tam[0]+1,tam[1]);
				vPMRunningPM = PMStatus.responseText.substring(tam[1]+1,tam[2]);
				vPMModel = PMStatus.responseText.substring(tam[2]+1,tam[3]);		
				vPMSerialNumber = PMStatus.responseText.substring(tam[3]+1,tam[4]);		
				vPMTotalCurr = PMStatus.responseText.substring(tam[4]+1,tam[5]);
				vPMFreq = PMStatus.responseText.substring(tam[5]+1,tam[6]);	
				vPMPF = PMStatus.responseText.substring(tam[6]+1,tam[7]);	
				vPMActivePower = PMStatus.responseText.substring(tam[7]+1,tam[8]);	
				vPMReactivePower = PMStatus.responseText.substring(tam[8]+1,tam[9]);	
				vPMActiveEnergy = PMStatus.responseText.substring(tam[9]+1,tam[10]);	
				vPMReactiveEnergy = PMStatus.responseText.substring(tam[10]+1,tam[11]);
				vPMVoltage1 = PMStatus.responseText.substring(tam[11]+1,tam[12]);//1
				vPMCurr1 = PMStatus.responseText.substring(tam[12]+1,tam[13]);
				vPMPF1 = PMStatus.responseText.substring(tam[13]+1,tam[14]);
				vPMActivePower1 = PMStatus.responseText.substring(tam[14]+1,tam[15]);
				vPMReactivePower1 = PMStatus.responseText.substring(tam[15]+1,tam[16]);
				vPMVoltage2 = PMStatus.responseText.substring(tam[16]+1,tam[17]);//2
				vPMCurr2 = PMStatus.responseText.substring(tam[17]+1,tam[18]);
				vPMPF2 = PMStatus.responseText.substring(tam[18]+1,tam[19]);
				vPMActivePower2 = PMStatus.responseText.substring(tam[19]+1,tam[20]);
				vPMReactivePower2 = PMStatus.responseText.substring(tam[20]+1,tam[21]);
				vPMVoltage3 = PMStatus.responseText.substring(tam[21]+1,tam[22]);//3
				vPMCurr3 = PMStatus.responseText.substring(tam[22]+1,tam[23]);
				vPMPF3 = PMStatus.responseText.substring(tam[23]+1,tam[24]);
				vPMActivePower3 = PMStatus.responseText.substring(tam[24]+1,tam[25]);
				vPMReactivePower3 = PMStatus.responseText.substring(tam[25]+1,tam[26]);

				vPMVoltage1 = parseFloat(vPMVoltage1/100).toFixed(2);	
				vPMVoltage2 = parseFloat(vPMVoltage2/100).toFixed(2);
				vPMVoltage3 = parseFloat(vPMVoltage3/100).toFixed(2);
				
				vPMTotalCurr = parseFloat(vPMTotalCurr/100).toFixed(2);		
				vPMCurr1 = parseFloat(vPMCurr1/100).toFixed(2);
				vPMCurr2 = parseFloat(vPMCurr2/100).toFixed(2);
				vPMCurr3 = parseFloat(vPMCurr3/100).toFixed(2);
				
				vPMPF = parseFloat(vPMPF/100).toFixed(2);
				vPMPF1 = parseFloat(vPMPF1/100).toFixed(2);
				vPMPF2 = parseFloat(vPMPF2/100).toFixed(2);
				vPMPF3 = parseFloat(vPMPF3/100).toFixed(2);		
				
				vPMFreq = parseFloat(vPMFreq/10).toFixed(1);	
								
				vPMActivePower = parseFloat(vPMActivePower/100).toFixed(2);		
				vPMActivePower1 = parseFloat(vPMActivePower1/100).toFixed(2);	
				vPMActivePower2 = parseFloat(vPMActivePower2/100).toFixed(2);	
				vPMActivePower3 = parseFloat(vPMActivePower3/100).toFixed(2);	
				
				vPMReactivePower = parseFloat(vPMReactivePower/100).toFixed(2);	
				vPMReactivePower1 = parseFloat(vPMReactivePower1/100).toFixed(2);	
				vPMReactivePower2 = parseFloat(vPMReactivePower2/100).toFixed(2);	
				vPMReactivePower3 = parseFloat(vPMReactivePower3/100).toFixed(2);	
				
				vPMActiveEnergy = parseFloat(vPMActiveEnergy/100).toFixed(2);		
				vPMReactiveEnergy = parseFloat(vPMReactiveEnergy/100).toFixed(2);
				
				if (vPMConnectStatus == 170) {
					setRowText('idpm_Connect', "Disconnected");
				} else if (vPMConnectStatus == 255) {
					setRowText('idpm_Connect', "Connected");
				} else if (vPMConnectStatus == 165) {
					setRowText('idpm_Connect', "Warning");
				} else {	
					setRowText('idpm_Connect', "Unavailable");
				}				
				
				
				if (vPMModel == 0) {
					setRowText('idpm_Model', "Unavailable");	
				} else if (vPMModel == 1) {
					setRowText('idpm_Model', "FINECO");		
				} else if (vPMModel == 2) {
					setRowText('idpm_Model', "APM-980");		
				} else if (vPMModel == 3) {	
					setRowText('idpm_Model', "SDM220");	
				}else if (vPMModel == 4) {	
					setRowText('idpm_Model', "PMC-220");		
				}else if (vPMModel == 5) {	
					setRowText('idpm_Model', "SPM91");		
				}
				// else if (vPMModel == 6) {	
				// 	setRowText('idpm_Model', "DRS-205C");		
				// }
				else if (vPMModel == 15) {	
					setRowText('idpm_Model', "SPM93");	
				} 
				else if (vPMModel == 17) {	
					setRowText('idpm_Model', "PM2230");	
				}
				else if (vPMModel == 18) {	
					setRowText('idpm_Model', "SDM72D");	
				} 
				else if (vPMModel == 19) {	
					setRowText('idpm_Model', "DDS353H-2");	
				}  
				
				setRowText('idpm_InstalledPM', vPMInstalledPM);
				setRowText('idpm_RunningPM', vPMRunningPM);
				setRowText('idpm_SerialNumber', vPMSerialNumber);
				
				setRowText('idpm_Voltage1', vPMVoltage1);
				setRowText('idpm_Voltage2', vPMVoltage2);
				setRowText('idpm_Voltage3', vPMVoltage3);
				
				setRowText('idpm_TotalCurr', vPMTotalCurr);
				setRowText('idpm_Curr1', vPMCurr1);
				setRowText('idpm_Curr2', vPMCurr2);
				setRowText('idpm_Curr3', vPMCurr3);
				
				setRowText('idpm_Freq', vPMFreq);
				
				setRowText('idpm_PF', vPMPF);
				setRowText('idpm_PF1', vPMPF1);
				setRowText('idpm_PF2', vPMPF2);
				setRowText('idpm_PF3', vPMPF3);
				
				setRowText('idpm_ActivePower', vPMActivePower);
				setRowText('idpm_ActivePower1', vPMActivePower1);
				setRowText('idpm_ActivePower2', vPMActivePower2);
				setRowText('idpm_ActivePower3', vPMActivePower3);
				
				setRowText('idpm_ReactivePower', vPMReactivePower);
				setRowText('idpm_ReactivePower1', vPMReactivePower1);
				setRowText('idpm_ReactivePower2', vPMReactivePower2);
				setRowText('idpm_ReactivePower3', vPMReactivePower3);
				
				setRowText('idpm_ActiveEnergy', vPMActiveEnergy);
				setRowText('idpm_ReactiveEnergy', vPMReactiveEnergy);
							
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		PMStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		PMStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(PMStatus)
	{
		PMStatus.open("GET","/pmInfo_status?=", true);
		PMStatus.onreadystatechange = PMStatusComplete;
		PMStatus.send(null);
	}
t=setTimeout("PMStatus_get()",5000);
}

function LoadFrame() {
	PMStatus_get();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

