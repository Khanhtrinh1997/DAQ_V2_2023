/**
 * @author SonLQ4
 */
function liionBattItem(id, vendor) {
	this.id = id.trim();
	this.vendor = vendor.trim();
}
function LIIONBATTStatusGet()
{
	var i=0;
	var length=0;
	var text;	
    var liionbatt = false;
    function liionbattComplete()
    {
		var LIIONBATTListInfo = new Array();
        if(liionbatt.readyState == 4)
        {
            if(liionbatt.status == 200)
            {                
				text = liionbatt.responseText;
				length = liionbatt.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							LIIONBATTListInfo.push(new liionBattItem(name[0], name[1]));
						}
					}                                         
				}
				setRowText('idconfigliion_TotalNo', LIIONBATTListInfo.length);
				for (i = 0; i < LIIONBATTListInfo.length; i++)
				{		
					if(LIIONBATTListInfo[i].vendor == 0)
					{
						setRowText(LiionSetParam[0][i], '');
						setRowText(LiionSetParam[1][i], '');		
					} else
					{
						setRowText(LiionSetParam[0][i], LIIONBATTListInfo[i].id);
						
						if(LIIONBATTListInfo[i].vendor == 1)
						{
							setRowText(LiionSetParam[1][i], 'COSLIGHT');
						} 
						else if(LIIONBATTListInfo[i].vendor == 2)
						{
							setRowText(LiionSetParam[1][i], 'COSLIGHT_V1.1');
						} 
						else if(LIIONBATTListInfo[i].vendor == 3)
						{
							setRowText(LiionSetParam[1][i], 'SHOTO');
						} 
						else if(LIIONBATTListInfo[i].vendor == 4)
						{
							setRowText(LiionSetParam[1][i], 'HUAWEI');
						} 
						else if(LIIONBATTListInfo[i].vendor == 5)
						{
							setRowText(LiionSetParam[1][i], 'M1Viettel');
						} else if(LIIONBATTListInfo[i].vendor == 6)
						{
							setRowText(LiionSetParam[1][i], 'ZTT_2020');
						} else if(LIIONBATTListInfo[i].vendor == 7)
						{
							setRowText(LiionSetParam[1][i], 'ZTT');
						} else if(LIIONBATTListInfo[i].vendor == 8)
						{
							setRowText(LiionSetParam[1][i], 'HUAWEI_A1');
						} else if(LIIONBATTListInfo[i].vendor == 9)
						{
							setRowText(LiionSetParam[1][i], 'SAFT');
						} else if(LIIONBATTListInfo[i].vendor == 10)
						{
							setRowText(LiionSetParam[1][i], 'Narada');
						} else if(LIIONBATTListInfo[i].vendor == 11)
						{
							setRowText(LiionSetParam[1][i], 'EVE');
						} else if(LIIONBATTListInfo[i].vendor == 12)
						{
							setRowText(LiionSetParam[1][i], 'SHOTO_SDA10_48100');
						}else if(LIIONBATTListInfo[i].vendor == 13)
						{
							setRowText(LiionSetParam[1][i], 'COSLIGHT_CF4850T');
						}else if(LIIONBATTListInfo[i].vendor == 14)
						{
							setRowText(LiionSetParam[1][i], 'HUAFU_HF48100C');
						}												
					}	
				}						
			}			
        }
    }

    if(window.XMLHttpRequest)
    {
        liionbatt = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        liionbatt = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(liionbatt)
    {
        liionbatt.open("GET", "/liion_status?=", true);
        liionbatt.onreadystatechange = liionbattComplete;
        liionbatt.send(null);
    }
	t=setTimeout("LIIONBATTStatusGet()",5000);
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
				v_pack_vd_set.open("GET", "/liionpackGv_value=" + v_pack_vd_data_set.value + "&id=" + Math.random(), true);
			}else{
				v_pack_vd_set.open("GET", "/liionpack"+index.toString(16).toUpperCase() +"v_value=" + v_pack_vd_data_set.value + "&id=" + Math.random(), true);
			}
			v_pack_vd_set.send(null);
		}
	}
}
function fLiBTotalNumSet (){
	var vLiBTotalNumSet = false;
	var vLiBTotalNumDataSet = document.getElementById("idliion_TotalNo");
	if(window.XMLHttpRequest){
		vLiBTotalNumSet = new XMLHttpRequest();
	}
	else if(window.ActiveXObject){
		vLiBTotalNumSet = new ActiveXObject("Microsoft.XMLHTTP");
	}
	if(vLiBTotalNumSet)	{
		if(vLiBTotalNumDataSet.value != ""){
			vLiBTotalNumSet.open("GET", "/libTotal_value=" + vLiBTotalNumDataSet.value + "&id=" + Math.random(), true);
			vLiBTotalNumSet.send(null);
		}
	}
}
function LoadFrame() {
	LIIONBATTStatusGet();	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}


