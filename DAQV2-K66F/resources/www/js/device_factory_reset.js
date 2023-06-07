/**
 * @author SonLQ4
 */

function factResGet()
{		
    function factResComplete()
    {	
		
    }

    if(window.XMLHttpRequest)
    {
       factRes = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        factRes = new ActiveXObject("Microsoft.XMLHTTP");
    }
	var answer = confirm("This will erase all existing configuration changes and restore factory default settings. Click OK if you are sure you want to do this or Cancel to retain existing settings.");
	if(answer == false)
	{
	alert("Existing configuration settings have been retained.");
	} else 
	{
		if(factRes)
		{
			factRes.open("GET", "/restore?id=" + Math.random(), true);
			factRes.onreadystatechange = factResComplete;
			factRes.send(null);
		}
	}    
}
function LoadFrame() {
	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}


