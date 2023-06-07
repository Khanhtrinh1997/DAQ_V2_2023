/**
 * @author SonLQ4
 */

	
function FWUPDATEStatus_get()
{
	var FWUPDATEStatus = false;
	var i=0;
	var y=0;
	var length=0;
	var text;
	var tam=Array(100);
	var nguyen;	
	
	function FWUPDATEStatusComplete()
	{
		if(FWUPDATEStatus.readyState == 4)
		{
			if(FWUPDATEStatus.status == 200)
			{
				text = FWUPDATEStatus.responseText;
				length = FWUPDATEStatus.responseText.length;
				y=0;
				for(i=0;i<length;i++)
				{
					if(text.charAt(i)==";")
					{
						tam[y]=i;
						y=y+1;
					}
				}
				
				vFwUpdateDev = FWUPDATEStatus.responseText.substring(0,tam[0]);	
			
				if(vFwUpdateDev == 0) {
				setRowText('idfwupdate_DEV', 'DAQ-V2');
				} else if (vFwUpdateDev == 1) {
				setRowText('idfwupdate_DEV', 'VAC-V1');
			    } else if (vFwUpdateDev == 2) {
				setRowText('idfwupdate_DEV', 'VAC-V2');
				}	else if (vFwUpdateDev == 255) {
				setRowText('idfwupdate_DEV', 'No Device');
				}					
			}
		}
	}

	if(window.XMLHttpRequest)
	{
		FWUPDATEStatus = new XMLHttpRequest();
	}
	else if(window.ActiveXObject)
	{
		FWUPDATEStatus = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(FWUPDATEStatus)
	{
		FWUPDATEStatus.open("GET","/fwupdate_status?=", true);
		FWUPDATEStatus.onreadystatechange = FWUPDATEStatusComplete;
		FWUPDATEStatus.send(null);
	}
t=setTimeout("FWUPDATEStatus_get()",5000);
}
 
function firmUpdateGet()
{
	
    function firmUpdateComplete()
    {	
		
    }

    if(window.XMLHttpRequest)
    {
       firmUpdate = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        firmUpdate = new ActiveXObject("Microsoft.XMLHTTP");
    }
	var answer = confirm("This will UPDATE the device's firmware. Click OK if you are sure you want to do this or Cancel to retain existing firmware version.");
	if(answer == false)
	{
	alert("Existing firmware version have been retained.");
	} else 
	{
		if(firmUpdate)
		{
			firmUpdate.open("GET", "/update?id=" + Math.random(), true);
			firmUpdate.onreadystatechange = firmUpdateComplete;
			firmUpdate.send(null);
		}
	}    
}


function fFwUpdateDevSet (){
	var vFwUpdateDevSet = false;
	var vFwUpdateDevDataSet = document.getElementById("fwupdatedev");
	
	if(window.XMLHttpRequest){
		vFwUpdateDevSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vFwUpdateDevSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vFwUpdateDevSet)	{
		if(vFwUpdateDevDataSet.value != ""){
			vFwUpdateDevSet.open("GET", "/fwUpdateDev_value=" + vFwUpdateDevDataSet.value + "&id=" + Math.random(), true);
			vFwUpdateDevSet.send(null);
		}
	}
}

function readBlob(opt_startByte, opt_stopByte) {
	var files = document.getElementById('files').files;
	if(!files.length) {
		alert('Please select a file!');
		return;
	}
	
	var file = files[0];
	var start = parseInt(opt_startByte) || 0;
	var stop = parseInt(opt_stopByte) || file.size - 1;
	
	var reader = new FileReader();
	
	reader.onloadend = function(evt) {
		if(evt.target.readyState == FileReader.DONE) {
			document.getElementById('byte_content').textContent = evt.target.result;
			document.getElementById('byte_range').textContent = ['Read bytes:  ', start + 1, ' - ', stop + 1, ' of ', file.size, 'byte file'].join('');
					
		}
	};
	var blob = file.slice(start, stop + 1);
	reader.readAsBinaryString(blob);

}

var reader = new FileReader();
var file;

function eventButtonListener() {

	var fileInput = document.getElementById('fileInput');
	var fileDisplayArea = document.getElementById('fileDisplayArea');

	fileInput.addEventListener('change', function(e) {
		
		file = fileInput.files[0];


		reader.onload = function(e) {
			fileDisplayArea.innerText = reader.result;
		}

		
		var blob = file.slice(0, 1024);

		reader.readAsBinaryString(blob);	
	
	});
	
}


function firmUploadSent (){	
	
	var data = new FormData();
	data.append('userfile',file,'firmware');

	var xhr = new XMLHttpRequest();
	xhr.open('POST', 'somewhere', true);
	xhr.onload = function () {
		// do something to response
		console.log(this.responseText);
	};
	xhr.send(data);
}

function LoadFrame() {
	FWUPDATEStatus_get();
	//eventButtonListener();
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}


