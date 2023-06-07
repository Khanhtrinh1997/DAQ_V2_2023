/**
 * @author SonLQ4
 */
function labmBattItem(id, vendor) {
	this.id = id.trim();
	this.vendor = vendor.trim();
}
function LABMStatusGet()
{
	var i=0;
	var t= 0;
	var length=0;
	var text;
    var labmbatt = false;
    function labmbattComplete()
    {
		var LABMListInfo = new Array();
        if(labmbatt.readyState == 4)
        {
            if(labmbatt.status == 200)
            {                
				text = labmbatt.responseText;
				length = labmbatt.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							LABMListInfo.push(new labmBattItem(name[0], name[1]));
						}
					}                                         
				}
				setRowText('idconfiglabm_TotalNo', LABMListInfo.length);
				for (i = 0; i < LABMListInfo.length; i++)
				{		
					if(LABMListInfo[i].vendor == 15)
					{
						setRowText(LabmSetParam[0][i], '');
						setRowText(LabmSetParam[1][i], '');		
					} else
					{
						setRowText(LabmSetParam[0][i], LABMListInfo[i].id);
						if(LABMListInfo[i].vendor == 1)
						{
							setRowText(LabmSetParam[1][i], 'M1Viettel');
						}						
					}	
				}						
			}			
        }
    }
    if(window.XMLHttpRequest)
    {
        labmbatt = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        labmbatt = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(labmbatt)
    {
        labmbatt.open("GET", "/labm_status?=", true);
        labmbatt.onreadystatechange = labmbattComplete;
        labmbatt.send(null);
    }
	t=setTimeout("LABMStatusGet()",5000);
}
function f_fack_id_set(index){
	var v_pack_vd_set       = false;
	var v_pack_vd_data_set  = document.getElementById("packvd"+index);
	if(window.XMLHttpRequest){
		v_pack_vd_set = new XMLHttpRequest();
	}else if(window.ActiveXObject)
	{
		v_pack_vd_set = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(v_pack_vd_set)
	{
		if(v_pack_vd_data_set.value != ""){
			if(index == 16){
				v_pack_vd_set.open("GET", "/labmpackGv_value=" + v_pack_vd_data_set.value + "&id=" + Math.random(), true);
			}else{
				v_pack_vd_set.open("GET", "/labmpack"+index.toString(16).toUpperCase() +"v_value=" + v_pack_vd_data_set.value + "&id=" + Math.random(), true);
			}
			v_pack_vd_set.send(null);
		}
	}
}
function fLabmTotalNumSet (){
	var vLabmTotalNumSet = false;
	var vLabmTotalNumDataSet = document.getElementById("idlabm_TotalNo");
	
	if(window.XMLHttpRequest){
		vLabmTotalNumSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vLabmTotalNumSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vLabmTotalNumSet)	{
		if(vLabmTotalNumDataSet.value != ""){
			vLabmTotalNumSet.open("GET", "/labmTotal_value=" + vLabmTotalNumDataSet.value + "&id=" + Math.random(), true);
			vLabmTotalNumSet.send(null);
		}
	}
}
function LoadFrame() {
	LABMStatusGet();	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}