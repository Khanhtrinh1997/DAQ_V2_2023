/**
 * @author SonLQ4
 */

function devRebootGet()
{
	
    function devRebootComplete()
    {	
		
    }

    if(window.XMLHttpRequest)
    {
       devReboot = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        devReboot = new ActiveXObject("Microsoft.XMLHTTP");
    }
	var answer = confirm("This will REBOOT the device. Click OK if you are sure you want to do this or Cancel to retain existing status.");
	if(answer == false)
	{
	alert("Existing status have been retained.");
	} else 
	{
		if(devReboot)
		{
			devReboot.open("GET", "/reboot?id=" + Math.random(), false);
			devReboot.onreadystatechange = devRebootComplete;
			devReboot.send(null);
		}
	}    
}
function LoadFrame() {
	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}


