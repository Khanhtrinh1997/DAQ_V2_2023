var hexVals = new Array("0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
              "A", "B", "C", "D", "E", "F");
var unsafeString = "\"<>%\\^[]`\+\$\,'#&";
// deleted these chars from the include list ";", "/", "?", ":", "@", "=", "&" and #
// so that we could analyze actual URLs


function IsvalidCalendar(sid,did){
	// var month = $("#cfgbscum").val();
	var month = $('#'+sid).val();
	// alert(month);
	
	switch(month){
		
		case "1":
		case "3":
		case "5":
		case "7":
		case "8":
		case "10":
		case "12":
		{
			$('select[name*="'+did+'"] option[value="30"]').show();
			$('select[name*="'+did+'"] option[value="31"]').show();
		} break;
		
		case "4":
		case "6":
		case "9":
		case "11":
		{
			$('select[name*="'+did+'"] option[value="30"]').show();
			$('select[name*="'+did+'"] option[value="31"]').hide();
		} break; 
		
		case "2":
		{
			$('select[name*="'+did+'"] option[value="30"]').hide();
			$('select[name*="'+did+'"] option[value="31"]').hide();
		} break;
	
	}	 
}

function isUnsafe(compareChar)
// this function checks to see if a char is URL unsafe.
// Returns bool result. True = unsafe, False = safe
{
   if ( unsafeString.indexOf(compareChar) == -1 && compareChar.charCodeAt(0) > 32
        && compareChar.charCodeAt(0) < 123 )
      return false; // found no unsafe chars, return false
   else
      return true;
}

function decToHex(num, radix)
// part of the hex-ifying functionality
{
   var hexString = "";
   while ( num >= radix ) {
      temp = num % radix;
      num = Math.floor(num / radix);
      hexString += hexVals[temp];
   }
   hexString += hexVals[num];
   return reversal(hexString);
}

function reversal(s)
// part of the hex-ifying functionality
{
   var len = s.length;
   var trans = "";
   for (i = 0; i < len; i++)
      trans = trans + s.substring(len-i-1, len-i);
   s = trans;
   return s;
}

function convert(val)
// this converts a given char to url hex form
{
   return  "%" + decToHex(val.charCodeAt(0), 16);
}

function encodeUrl(val)
{
   var len     = val.length;
   var i       = 0;
   var newStr  = "";
   var original = val;

   for ( i = 0; i < len; i++ ) {
      if ( val.substring(i,i+1).charCodeAt(0) < 255 ) {
         // hack to eliminate the rest of unicode from this
         if (isUnsafe(val.substring(i,i+1)) == false)
            newStr = newStr + val.substring(i,i+1);
         else
            newStr = newStr + convert(val.substring(i,i+1));
      } else {
         // woopsie! restore.
         alert ("Found a non-ISO-8859-1 character at position: " + (i+1) + ",\nPlease eliminate before continuing.");
         newStr = original;
         // short-circuit the loop and exit
         i = len;
      }
   }

   return newStr;
}

function encodeUrlISO_8859_1(val)
{
   var len     = val.length;
   var i       = 0;
   var newStr  = "";
   var original = val;

   for ( i = 0; i < len; i++ ) {
      if ( val.substring(i,i+1).charCodeAt(0) < 255 ) {
         // hack to eliminate the rest of unicode from this
         if (isUnsafe(val.substring(i,i+1)) == false)
            newStr = newStr + val.substring(i,i+1);
         else
            newStr = newStr + convert(val.substring(i,i+1));
      } else {
         // woopsie! restore.
         //alert ("Found a non-ISO-8859-1 character at position: " + (i+1) + ",\nPlease eliminate before continuing.");
         return -1;
      }
   }

   return newStr;
}


var g_testFlag = 1;

var g_needMakeData = 0;

var TabWidth = "75%";
function isValidAscii(val)
{
    for ( var i = 0 ; i < val.length ; i++ )
    {
        var ch = val.charAt(i);
        if ( ch < ' ' || ch > '~' )
        {
            return ch;
        }
    }
    return '';
}

function isValidCfgStr(cfgName, val, len)
{
	if (isValidAscii(val) != '')         
	{            
		alert(cfgName + ' has invalid character "' + isValidAscii(val) + '".')          
		return false;       
    }
   if (val.length > len)
   {
	   alert(cfgName + ' cannot exceed ' + len  + ' characters.');
	   return false;
   }		
}

function isHexaDigit(digit) {
   var hexVals = new Array("0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                           "A", "B", "C", "D", "E", "F", "a", "b", "c", "d", "e", "f");
   var len = hexVals.length;
   var i = 0;
   var ret = false;

   for ( i = 0; i < len; i++ )
      if ( digit == hexVals[i] ) break;

   if ( i < len )
      ret = true;

   return ret;
}

function last8isHexaNumber(number)
{
    for (var index = 4; index < number.length; index++)
    {
        if (isHexaDigit(number.charAt(index)) == false)
        {
            return false;
        }
    }
    return true;
}

function isSafeStringExc(compareStr, UnsafeStr)
{
	for (var i = 0; i < compareStr.length; i++)
	{
		var c = compareStr.charAt(i);
		if (isValidAscii(c) != '')
		{
			 return false;
	    }
		else
		{
			if (UnsafeStr.indexOf(c) > -1)
			{
				return false;
			}
		}
	}
    return true;
}

function isSafeStringIn(compareStr, UnsafeStr)
{
	for (var i = 0; i < compareStr.length; i++)
	{
		var c = compareStr.charAt(i);
		if (isValidAscii(c) != '')
		{
			 return false;
	    }
		else
		{
			if (UnsafeStr.indexOf(c) == -1)
			{
				return false;
			}
		}
	}
    return true;
}

function isEndGTEStart(EndIp, StartIp)
{
   addrEnd = EndIp.split('.');
   addrStart = StartIp.split('.');
   E = parseInt(addrEnd[3],10) + 1;
   S = parseInt(addrStart[3],10) + 1;
   if (E < S) 
      return false;
   return true;
}

function ipIsInIP(subStartIp, subEndIp, StartIp, EndIp)
{
   addrEnd = EndIp.split('.');
   addrStart = StartIp.split('.'); 
   subAddrEnd = subEndIp.split('.');
   subAddrStart = subStartIp.split('.'); 

   for(i = 0; i < 4; i++)
   {
       seg = parseInt(addrEnd[i],10);
       subSeg = parseInt(subAddrEnd[i],10);
       if(subSeg > seg)
          return false;

       seg = parseInt(addrStart[i],10);
       subSeg = parseInt(subAddrStart[i],10);  
       if((subSeg > 0) && (subSeg < seg))
          return false;
   }

   return true;
}

// Check if a name valid
function isValidName(name) 
{
   return isSafeStringExc(name,'"<>%\\^[]`\+\$\,=\'#&: \t');
}

//a valid string do not contain '"' and each char is validAscII
function isValidString(name) 
{
   return isSafeStringExc(name,'"\\');
}

function isInteger(value)
{   	
	if (/^(\+|-)?\d+$/.test(value)) 
	{
	   return true;
	} 
	else 
	{
	    return false;
	}
}

function isPlusInteger(value)
{
	if (isInteger(value) && parseInt(value) >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

function isFloat(value)
{   	
	if (/^(\+|-)?\d+($|\.\d+$)/.test(value))
	{
	   return true;
	} 
	else 
	{
	   return false;
	}
}

function CheckNumber(Value, MinRange, MaxRange)
{
    //prevent Hexadecimal or octal
    if ( Value.length > 1 && Value.charAt(0) == '0' )
    {
        return false;
    }
    
    if (false == isInteger(Value))
    {
        return false;
    }
	
    var t = parseInt(Value, 10);
    if (isNaN(t) ==  true)
    {
        return false;
    }
    
    if (t < MinRange || t > MaxRange)
    {
        return false;
    }
    
    return true;
}

function isValidCfgInteger(cfgName, val, start, end)
{
	   if (isInteger(val) == false)
	   {
	       alert(cfgName + ' is invalid, it must be digist.');
		   return false;
	   }
	   var temp = parseInt(val);
	   if (temp < start || temp > end)
	   {
	       alert(cfgName + ' must be greater than or equal to ' + start.toString()
		         + ' and less than or equal to ' + end.toString() + '.');
		   return false;
	   }	
}

function isValidCfgFloat(cfgName, val, start, end)
{
	   if (isFloat(val) == false)
	   {
	       alert(cfgName + ' is invalid, it must be digist.');
		   return false;
	   }
	   var temp = parseFloat(val)/100;
	   if (temp < start || temp > end)
	   {
	       alert(cfgName + ' must be greater than or equal to ' + start.toString()
		         + ' and less than or equal to ' + end.toString() + '.');
		   return false;
	   } else {
			return true;
	   }	   
}


function isValidCfgFloatU(cfgName, val, start, end)
{
	   if (isFloat(val) == false)
	   {
	       alert(cfgName + ' is invalid, it must be digist.');
		   return false;
	   }
	   var temp = parseFloat(val)/100;
	   if (temp <= start || temp > end)
	   {
	       alert(cfgName + ' must be greater than ' + start.toString()
		         + ' and less than or equal to ' + end.toString() + '.');
		   return false;
	   } else {
			return true;
	   }	   
}


function isValidCfgFloatL(cfgName, val, start, end)
{
	   if (isFloat(val) == false)
	   {
	       alert(cfgName + ' is invalid, it must be digist.');
		   return false;
	   }
	   var temp = parseFloat(val)/100;
	   if (temp < start || temp >= end)
	   {
	       alert(cfgName + ' must be greater than or equal to ' + start.toString()
		         + ' and less than ' + end.toString() + '.');
		   return false;
	   } else {
			return true;
	   }	   
}


function isValidCfgFloatA(cfgName, val, start, end)
{
	   if (isFloat(val) == false)
	   {
	       alert(cfgName + ' is invalid, it must be digist.');
		   return false;
	   }
	   var temp = parseFloat(val)/100;
	   if (temp <= start || temp >= end)
	   {
	       alert(cfgName + ' must be greater than ' + start.toString()
		         + ' and less than ' + end.toString() + '.');
		   return false;
	   } else {
			return true;
	   }	   
}

function isValidCfgFloat1(cfgName, val, start, end)
{
	   if (isFloat(val) == false)
	   {
	       alert(cfgName + ' is invalid, it must be digist.');
		   return false;
	   }
	   var temp = parseFloat(val)/10;
	   if (temp < start || temp > end)
	   {
	       alert(cfgName + ' must be greater than ' + start.toString()
		         + ' and less than ' + end.toString() + '.');
		   return false;
	   } else {
			return true;
	   }	   
}

function isValidCfgFloat2(cfgName, val, start, end)
{
	   if (isFloat(val) == false)
	   {
	       alert(cfgName + ' is invalid, it must be digist.');
		   return false;
	   }
	   var temp = parseFloat(val)/1000;
	   if (temp < start || temp > end)
	   {
	       alert(cfgName + ' must be greater than or equal to ' + start.toString()
		         + ' and less than or equal to ' + end.toString() + '.');
		   return false;
	   } else {
			return true;
	   }	   
}

function checkSpace(str)
{
	var len=str.length;

	if(len==0)
	{
		return false;
	}
	if(str.charAt(0)==' ')
	{
		return false;
	}
	if(str.charAt(len-1)==' ')
	{
		return false;
	}
	return true;
}

function CheckUrlParameter(inputUrl)
{
	//check left or right space
	if(checkSpace(inputUrl)==false)
	{
	  return false;
	}
	
	if(inputUrl.indexOf('http://')!=-1)
	{
		/*There http://'s in the middle*/
		if(inputUrl.indexOf('http://')!=0)
		{
			return false;
		}
		/*Only http://*/
		if(inputUrl=="http://")
		{
			return false;
		}
		/*Remove http:// head*/
		inputUrl=inputUrl.substring(7);
	}
	/*The domain name is empty, is that http:/// or /*/
	if(inputUrl.indexOf('/')==0)
	{
		return false;
	}
	var CutUrl=inputUrl.split('/');
	var Domine=CutUrl[0];
	var ports=Domine.split(':');
	var len=ports.length;
	//There are port
	if(ports.length>1)
	{
		//Port is not between 0-65536
		if((parseInt(ports[len-1],10)>0&&parseInt(ports[len-1],10)<65536)==false)
		{
			return false;
		}
		//Remove the port
		Domine=Domine.substring(0,(Domine.length)-1-ports[len-1].length);
	}
	
	
	//Filter out the ip address of the first 0
	var i=0;
	var adds=Domine.split('.');
	if(adds[0]=='0'&&adds.length==4)
	{
		var isip=1;
		for(var key=1;key<=3;key++)
		{
			if(adds[key]<=255 && adds[key]>=0)
			{
				continue;
			}
			else
			{
				isip=0;
				break;
			}
		}
		if(isip==1)
		{
			return false;
		}
	}
	//Remove the url in front of a space
	while(Domine.indexOf(" ")==0)
	{
		Domine=Domine.substring(1);
	}
	/*All 0、All F*/
	if(Domine=='0.0.0.0'||Domine=='255.255.255.255')
	{
		return false;
	}
	if ((isValidIpAddress(Domine) == true))
	{			
		var addrs=Domine.split('.');
		/*Multicast address*/
		if(parseInt(addrs[0],10)>=224)
		{
			return false; 
		}
		/*Loopback address*/
		if(Domine=='127.0.0.1')
		{
			return false;
		}
		/*Network Address*/
		if(addrs[3]=='0')
		{
			return false; 
		}
	}
	return true;
	
}

function isNull( str )
{
	if ( str == "" ) return true;
	var regu = "^[ ]+$";
	var re = new RegExp(regu);
	return re.test(str);
}

function IsUrlValid(_Url)
{
	if(true == isNull(_Url))
	{
		return false;
	}
	var Url = new String(_Url.toLocaleLowerCase().replace("http://",""));
	var ExitColon = false;
	var ColonLocation = 0;
	var ColorReg = new RegExp(".*[a-zA-Z0-9]+:[0-9]+/*");

	var ArrayOfUrl = Url.split("//");

	if(ArrayOfUrl[0].toUpperCase() == "FTP:" || ArrayOfUrl[0].toUpperCase() == "HTTPS:")
	{
		return false;
	}
	if (ArrayOfUrl.length >= 2)
	{
		Url = ArrayOfUrl[1];
	}

	if (Url.length == 0)
	{
		return false;
	}

	ColonLocation = Url.indexOf(":", 0);
	if (ColonLocation == 0)
	{
		return false;
	}

	ExitColon = ColonLocation > 0 ? true : false;

	if (ExitColon == false)
	{
		return true;
	}

	return ColorReg.test(Url);
}


/* END:   Added by c00147909, 2009/2/6 */
//web Element 
/*get element by name or id*/
function getElById(sId)
{
	return getElement(sId);
}

function getElementById(sId)
{
	if (document.getElementById)
	{
		return document.getElementById(sId);	
	}
	else if (document.all)
	{
		// old IE
		return document.all(sId);
	}
	else if (document.layers)
	{
		// Netscape 4
		return document.layers[sId];
	}
	else
	{
		return null;
	}
}

/*getElByName*/
function getElementByName(sId)
{    // standard
	if (document.getElementsByName)
	{
		var element = document.getElementsByName(sId);
		
		if (element.length == 0)
		{
			return null;
		}
		else if (element.length == 1)
		{
			return 	element[0];
		}
		
		return element;		
	}
}

function getElement(sId)
{
	 var ele = getElementByName(sId); 
	 if (ele == null)
	 {
		 return getElementById(sId);
	 }
	 return ele;
}

function getOptionIndex(sId,sValue)
{
	var selObj = getElement(sId);
	if (selObj == null)
	{
		return -1;
	}
	
	for (i = 0; i < selObj.length; i++)
	{
		if (selObj.options[i].value == sValue)
		{
			return i;
		}
	}
	
	return -1;
}

/*----------------getLength-----------------*/
function getValue(sId)
{
	var item;
	if (null == (item = getElement(sId)))
	{
		debug(sId + " is not existed" );
		return -1;
	}

	return item.value;
}


/* Web page manipulation functions */
function setDisplay (sId, sh)
{
    var status;
    if (sh > 0) 
	{
        status = "";
    }
    else 
	{
        status = "none";
    }
	var item = getElement(sId);
	if (null != item)
	{
		getElement(sId).style.display = status;
	}
}

function setVisible(sId, sh)
{
    var status;
    if (sh > 0) 
	{
        status = "visible";
    }
    else 
	{
        status = "hidden"
    }
    var item = getElement(sId);
	if (null != item)
	{
		getElement(sId).style.visibility = status;
	}
}

function setSelect(sId, sValue)
{
	var item;
	if (null == (item = getElement(sId)))
	{
		debug(sId + " is not existed" );
		return false;
	}
	
	for (var i = 0; i < item.options.length; i++) 
	{
        if (item.options[i].value == sValue)
		{

        	item.selectedIndex = i;
        	return true;
        }
    }

    debug("the option which value is " + sValue + " is not existed in " + sId);
    return false;
}

function setText (sId, sValue)
{
	var item;
	if (null == (item = getElement(sId)))
	{
		debug(sId + " is not existed" );
		return false;
	}
    
	if(null != sValue)
	{
		sValue = sValue.toString().replace(/&nbsp;/g," ");
		sValue = sValue.toString().replace(/&quot;/g,"\"");
		sValue = sValue.toString().replace(/&gt;/g,">");
		sValue = sValue.toString().replace(/&lt;/g,"<");
		sValue = sValue.toString().replace(/&#39;/g, "\'");
		sValue = sValue.toString().replace(/&#40;/g, "\(");
		sValue = sValue.toString().replace(/&#41;/g, "\)");
		sValue = sValue.toString().replace(/&amp;/g,"&");
	}
	item.value = sValue;
	return true;
}


function setCheck(sId, value)
{
	var item;
	if (null == (item = getElement(sId)))
	{
		debug(sId + " is not existed" );
		return false;
	}
	
    if ( value == '1' ) 
	{    
       item.checked = true;
    }
	else
	{
       item.checked = false;
    }

    return true;
}

function setRadio(sId, sValue)
{
	var item;
	if (null == (item = getElement(sId)))
	{
		debug(sId + " is not existed" );
		return false;
	}
	
	for (i=0; i<item.length; i++)
	{
		if (item[i].value == sValue) 
		{
			item[i].checked = true;
			return true;
		}
    }

    debug("the option which value is " + sValue + " is not existed in " + sId);
    return false;
}

function getDivInnerId(divID)
{
    var nameStartPos = -1;
    var nameEndPos = -1;
    var ele;

    divHTML = getElement(divID).innerHTML;
    nameStartPos = divHTML.indexOf("name=");
    nameEndPos   = divHTML.indexOf(' ', nameStartPos);
    if(nameEndPos <= 0)
    {
        nameEndPos = divHTML.indexOf(">", nameStartPos);
    }
    
    try
    {
        ele = eval(divHTML.substring(nameStartPos+3, nameEndPos));
    }
    catch (e)
    {
        ele = divHTML.substring(nameStartPos+3, nameEndPos);
    }
    return ele;
}

function setDisable(sId, flag)
{
	var item;
	if (null == (item = getElement(sId)))
	{
		debug(sId + " is not existed" );
		return false;
	}

    if ( typeof(item.disabled) == 'undefined' )
    {
        if ( item.tagName == "DIV" || item.tagName == "div" )
        {
            var ele = getDivInnerId(sId);            
            setDisable(ele, flag);
        }
    }
    else
    {
        if ( flag == 1 || flag == '1' ) 
        {
             item.disabled = true;
			 item.style.color = '#ACA899';
        }
        else
        {
             item.disabled = false;
			 item.style.color = '#000000';
        }     
    }
    
    return true;
}

function getCheckVal(sId)
{
	var item;
	if (null == (item = getElement(sId)))
	{
		debug(sId + " is not existed" );
		return -1;
	}
	if (item.checked == true)
	{
		return 1;
	}

	else
	{
		return 0;
	}
}

function getRadioVal(sId)
{
	var item;
	if (null == (item = getElement(sId)))
	{
		debug(sId + " is not existed" );
		return -1;
	}
	
	for (i = 0; i < item.length; i++)
	{
		if (item[i].checked == true)
		{
		   return item[i].value;
		}
	}
	
	return -1;
}

function getSelectVal(sId)
{
   return getValue(sId);
}

function getBoolValue(param)
{
    var value = parseInt(param);
    if (isNaN(value) == true )
    {
       var LowerParam = param.toLowerCase();
       if (LowerParam == 'enable')
       {
          return 1;
       }
       else
       {
          return 0;
       }
    }
    else
    {
       return value;
    }
}

function isSpecialCharacter(str) {
    var iChars = "~`!#$%^&*+=-[]\\\';,/{}|\":<>?";

    for (var i = 0; i < str.length; i++) {
       if (iChars.indexOf(str.charAt(i)) != -1) {
           return true;
       }
    }
    return false;
}

function isSpecialCharacterInString(str)
{
	var specia_Reg =/^.*[`~!@#\$%\^&\*\(\)_\+\-=\[\]\{\}\'\;\,\./:\"\?><\\\|]{1,}.*$/; /*MinhBQ Ref#3837- remove space character*/
	var MyReg = new RegExp(specia_Reg);
	if ( MyReg.test(str) )
	{	
		return true;
	}
  	return false;
}

function isDigitInString(str)
{
	var digit_reg = /^.*([0-9])+.*$/;
	var MyReg = new RegExp(digit_reg);
	if ( MyReg.test(str) )
	{	
		return true;
	}
	return false;
}

function isUppercaseInString(str)
{
		var upper_reg = /^.*([A-Z])+.*$/;
		var MyReg = new RegExp(upper_reg);
		if ( MyReg.test(str) )
		{	
			return true;
		}
		return false;
}

function isLowercaseInString(str)
{
		var lower_reg = /^.*([a-z])+.*$/;
		var MyReg = new RegExp(lower_reg);
		if ( MyReg.test(str) )
		{	
			return true;
		}
		return false;
}

function CompareString(srcstr,deststr)
{
	var reverestr=(srcstr.split("").reverse().join(""));
	if ( srcstr == deststr )
	{
		return false;
	}
	
	if (reverestr == deststr )
	{
		return false;
	}
	return true;
}

function ShowNewRow(oldstring)
{
	var newstring = '';
	var LineLength = 200;
	for (j = 0; j < parseInt((oldstring.length)/LineLength); j++)
	{
		newstring += oldstring.substr(LineLength*j,LineLength*(j+1)) + ' ';
	}
	newstring +=  oldstring.substr(LineLength*j,oldstring.length);
	return newstring;
}

function isNumber (o) {
  return ! isNaN (o-0) && o != null;
}


function setRowText(rowID, value) {
	var row = document.getElementById(rowID);
	row.innerHTML = value;
}
function LoadLanguageForTableTitle() {
	var all = document.getElementsByTagName("td");
	for (var i = 0; i < all.length; i++) {
		var b = all[i];
		if (b.getAttribute("BindText") == null) {
			continue;
		}
		b.innerHTML = LanguageUI[b.getAttribute("BindText")];
	}
}

function LoadLanguageForSelection() {
	var all = document.getElementsByTagName("option");
	for (var i = 0; i < all.length; i++) {
		var b = all[i];
		if (b.getAttribute("BindText") == null) {
			continue;
		}
		b.innerHTML = LanguageUI[b.getAttribute("BindText")];
	}
}
