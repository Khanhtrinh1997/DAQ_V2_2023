/**
 * @author SonLQ4
 */

var ALARMListInfo = new Array();
 
function alarmHisItem(no, name , content, startTime, endTime) {
	this.no = no.trim();
	this.name = name.trim();
	this.content = content.trim();
	this.startTime = startTime.trim();
	this.endTime = endTime.trim();
}

function alarmHisStatusGet()
{
	var i=0;
	var j=0;
	var y=0;
	var x = 0;
	var t= 0;
	var m = 0;
	var length=0;
	var text;
	var temp1=Array(300);
	var temp2=Array(300);	
		
	
    var alarmHis = false;
    function alarmHisComplete()
    {
        if(alarmHis.readyState == 4)
        {
            if(alarmHis.status == 200)
            {                
				text = alarmHis.responseText;
				length = alarmHis.responseText.length;
				
				var info = text.split('@');
				if (info != '') {
					for ( i = 0; i < info.length; i++) {
						var name = info[i].split(';');
						if (name.length > 1) {
							ALARMListInfo.push(new alarmHisItem(name[0], name[1], name[2], name[3], name[4]));
						}
					}
				}				
			}
			
        }
    }

    if(window.XMLHttpRequest)
    {
        alarmHis = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        alarmHis = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(alarmHis)
    {
        alarmHis.open("GET", "/alarmHis_status?=", true);
        alarmHis.onreadystatechange = alarmHisComplete;
        alarmHis.send(null);
    }
}

function alarmTableGet()
{	
    function alarmTableComplete()
    {							
    }

    if(window.XMLHttpRequest)
    {
        alrmtable = new XMLHttpRequest();
    }
        else if(window.ActiveXObject)
    {
        alrmtable = new ActiveXObject("Microsoft.XMLHTTP");
    }
    if(alrmtable)
    {
        alrmtable.open("GET", "/alrmget?id=" + Math.random(), true);
        alrmtable.onreadystatechange = alarmTableComplete;
        alrmtable.send(null);
    }
}

function GenerateTable() {
	//Build an array containing Customer records.
	var customers = new Array();

	customers.push(["No", "  Alarm Name  ", "  Alarm Content  ", "  Start Time  ", "  End Time  "]);   

	for (var k =0; k < 200; k++)
	{
		customers.push([ALARMListInfo[k].no, ALARMListInfo[k].name,ALARMListInfo[k].content,ALARMListInfo[k].startTime,ALARMListInfo[k].endTime]);   
	}
	//Create a HTML Table element.
	var table = document.createElement("TABLE");
	//table.border = "1";
	table.setAttribute("class","tabal_bg width_100p");
	table.setAttribute("align","center");
	table.setAttribute("border","0");
	table.setAttribute("cellpadding","0");
	table.setAttribute("cellspacing","1");

	//Get the count of columns.
	var columnCount = customers[0].length;

	//Add the header row.
	var row = table.insertRow(-1);
	row.setAttribute("class","tabal_title"); 
	//for (var i = 0; i < columnCount; i++) {
	//	var headerCell = document.createElement("TH");
	//	headerCell.innerHTML = customers[0][i];
	//	row.appendChild(headerCell);
	//}
	
	var headerCell0 = document.createElement("TH");
	headerCell0.setAttribute("class","tabal_center01 width_10p");
	headerCell0.innerHTML = customers[0][0];
	row.appendChild(headerCell0);
	
	var headerCell1 = document.createElement("TH");
	headerCell1.setAttribute("class","tabal_center01 width_30p");
	headerCell1.innerHTML = customers[0][1];
	row.appendChild(headerCell1);
	
	var headerCell2 = document.createElement("TH");
	headerCell2.setAttribute("class","tabal_center01 width_20p");
	headerCell2.innerHTML = customers[0][2];
	row.appendChild(headerCell2);
	
	var headerCell3 = document.createElement("TH");
	headerCell3.setAttribute("class","tabal_center01 width_20p");
	headerCell3.innerHTML = customers[0][3];
	row.appendChild(headerCell3);
	
	var headerCell4 = document.createElement("TH");
	headerCell4.setAttribute("class","tabal_center01 width_20p");
	headerCell4.innerHTML = customers[0][4];
	row.appendChild(headerCell4);
	
	
	//Add the data rows.
	for (var i = 1; i < customers.length; i++) {
		row = table.insertRow(-1);
		row.setAttribute("class","tabal_center01 trTabContent");
		for (var j = 0; j < columnCount; j++) {
			var cell = row.insertCell(-1);			
			cell.innerHTML = customers[i][j];
		}
	}

	var dvTable = document.getElementById("dvTable");
	dvTable.innerHTML = "";
	dvTable.appendChild(table);
}

function move() {
    var elem = document.getElementById("myBar");
    var width = 0;
	var moveCompleted;
    var id = setInterval(frame, 1250);
    function frame() {
        if (width >= 100) {
            clearInterval(id);
			document.getElementById('ReadLogButton').disabled = false;
        } else {
            width++;
            elem.style.width = width + '%';
            elem.innerHTML = width * 1 + '%';
			document.getElementById('ReadLogButton').disabled = true;
        }
    }
} 

function buttonPush(){
	
	
	setTimeout(GenerateTable,135000);
	setTimeout(alarmHisStatusGet,130000);
	alarmTableGet();
	move();
}


function LoadFrame() {
	
	LoadLanguageForTableTitle();
	LoadLanguageForSelection();	
}

