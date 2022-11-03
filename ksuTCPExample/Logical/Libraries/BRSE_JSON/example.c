/*! \page Example

\section Example Task

This task is used together with the JSON Client (see next section) to read and send data using JSON.
Making it possible to read/write variables and create files through a normal webbrowser.  
\arg Global Types
\code
TYPE
	New_typ1 : 	STRUCT 
		Middle2 : UINT;
		Middle1 : ARRAY[0..1] OF New_typ;
	END_STRUCT;
	New_typ : 	STRUCT 
		Inner1 : ARRAY[0..10] OF SINT;
		Inner2 : UINT;
		Inner3 : DATE_AND_TIME;
		Inner5 : ARRAY[0..2] OF STRING[20];
	END_STRUCT;
	PV_var_typ : 	STRUCT 
		Outer1 : ARRAY[0..1] OF New_typ1;
		Outer2 : BOOL;
	END_STRUCT;
	ParameterMachineDayTank_typ : 	STRUCT 
		ChemicalType : USINT;
	END_STRUCT;
	ParameterMachine_typ : 	STRUCT 
		DayTank : ARRAY[0..MAX_NUMBER_OF_DAYTANKS_MACHINE] OF ParameterMachineDayTank_typ;
		NumberOfDayTanks : USINT;
	END_STRUCT;
	ParameterMainTank_typ : 	STRUCT 
		ChemicalType : USINT; (*Type of chemical tank (0 - 8)*)
		LowLevel : INT; (*Level when not able to run the machine (0 - 100%)*)
		OrderLevel : INT; (*Level when to make an order alarm (0 - 100%)*)
		TimeBeforeCirculation : TIME; (*Time before cirulation is done in a circulating system*)
		CirculationTime : TIME; (*How long are the circulation done*)
		MaxTimeForPump : TIME; (*Maximum time of pump beeing run before alarm*)
	END_STRUCT;
	Parameter_typ : 	STRUCT 
		MainTank : ARRAY[0..MAX_NUMBER_OF_MAINTANKS] OF ParameterMainTank_typ;
		Machine : ARRAY[0..MAX_NUMBER_OF_MACHINES] OF ParameterMachine_typ;
		NumberOfMainTanks : USINT;
		NumberOfMachines : USINT;
		SystemType : USINT; (*Circulating or Manifold*)
		SetupDone : BOOL; (*The setup of the machine is done*)
	END_STRUCT;
	MAIN_STATES : 
		(
		MAIN_INIT, (*Wait for init*)
		MAIN_AUTO, (*System running*)
		MAIN_STOP, (*System stoped*)
		MAIN_FORCEIO (*Force IO*)
		);
	MainInputCommand_typ : 	STRUCT  (*..documented in the code*)
		Auto : BOOL;
		Stop : BOOL;
		EmergencyStop : BOOL;
		ForceIO : BOOL;
		Reset : BOOL;
	END_STRUCT;
	MainInputAccess_typ : 	STRUCT  (*Adresses to other interfaces which are accessed*)
		OtherInterface : UDINT;
	END_STRUCT;
	MainInput_typ : 	STRUCT 
		Command : MainInputCommand_typ;
		Access : MainInputAccess_typ;
	END_STRUCT;
	MainOutputStatus_typ : 	STRUCT  (*..documented in the code*)
		SystemRunning : BOOL;
		SystemStoped : BOOL;
		ForceIOActive : BOOL;
		EmergencyStop : BOOL;
		MaintankLowLevel : BOOL;
		DaytankOverfilled : BOOL;
		PumpRunToLong : BOOL;
		LeakOnMaintank : BOOL;
		HighPressure : BOOL;
		OrderConsumables : BOOL;
	END_STRUCT;
	MainOutput_typ : 	STRUCT 
		Status : MainOutputStatus_typ;
		DaytankNumberOverfilled : USINT; (*What is my purpose?*)
		MachineNumberOverfilled : USINT; (*What is my purpose?*)
	END_STRUCT;
	Main_typ : 	STRUCT 
		Address : UDINT;
		Input : MainInput_typ;
		Output : MainOutput_typ;
		State : MAIN_STATES;
		SubState : UINT;
	END_STRUCT;
END_TYPE

\endcode

\arg Global Variables
\code

	VAR RETAIN
		PV_var1 : ARRAY[0..2] OF PV_var_typ;
		File_List : FILE_LIST_typ;
		Main : ARRAY[0..MAX_NUMBER_OF_MAINTANKS] OF Main_typ;
		Parameter : Parameter_typ;
	END_VAR
	VAR CONSTANT
		MAX_NUMBER_OF_MAINTANKS : USINT := 4;
		MAX_NUMBER_OF_DAYTANKS_MACHINE : USINT := 3;
		MAX_NUMBER_OF_MACHINES : USINT := 7;
	END_VAR

\endcode

\arg Local Variabels
\code
	VAR RETAIN
		BigBuffer : ARRAY[0..400] OF STRING[50];
		Debug : ARRAY[0..29] OF STRING[50];
		VariablesNotFound : ARRAY[0..29] OF STRING[50];
		StName : STRING[60] := 'Parameter';
		JSON_STRINGIFY_0 : JSON_Stringify;
		JSON_PARSE_0 : JSON_Parse;
		CtrlVar : USINT;
		STATUS : UDINT;
		JSONBuffer : ARRAY[0..99] OF STRING[50];
		notreadrd : UDINT := 0;
		readsize : UDINT := 0;
		pOuter : REFERENCE TO New_typ1;
	END_VAR
\endcode

\arg Init
\code
	PROGRAM _INIT

		(* Make the PV variables accessable *)
		PV_var1[0].Outer1[0].Middle1[0].Inner1[0] := 0;
		Parameter.Machine[0].DayTank[0].ChemicalType := 0;
		Main[0].Input.Access.OtherInterface := 0;
		pOuter ACCESS ADR(PV_var1[0].Outer1[0]);	(* pointer variables can be accessed by "JSON_DEMO:*pOuter" *)

		(* define the output for the Webserver *)
		WEB_INIT();
		WEB_PAGE(ADR('/'));					(* Link to the admin panel *)
		WEB_HTML(ADR('<html><body><h1><font color="orange">B&R SGC Webserver with JSON support</font></h1><br><a href="/admin.br">Admin Panel</a></body></html> ')); 	
		WEB_PAGE(ADR('/read'));
		WEB_HTML(ADR(JSONBuffer)); 			(* response of the Stringify function after reading a variable *)
		WEB_PAGE(ADR('/write'));
		WEB_HTML(ADR(VariablesNotFound)); 	(* the variables that could not be found by the Parse function after writing a variable *)
		WEB_PAGE(ADR('/writetofile'));
		WEB_HTML(ADR(JSONBuffer));			(* {"result":true} for success {"result":false} if writing the file failed *)

		(* try to read the file and parse the content => Variable will be written during startup *)
		notreadrd := 1;
		WHILE notreadrd = 1 DO
			notreadrd := FILE_READ(File_List,ADR('setPVvar1.txt'),readsize);
			IF notreadrd < 2 THEN
				readsize := readsize + strlen(FILE_OUTPUT());
				JSON_PARSE_0.pVariablesNotFound := ADR(VariablesNotFound);
				JSON_PARSE_0.SizeVariablesNotFound := SIZEOF(VariablesNotFound);
				JSON_PARSE_0.pDebug := ADR(Debug);
				JSON_PARSE_0.SizeDebug := SIZEOF(Debug);
				JSON_PARSE_0.pMessage := FILE_OUTPUT();
				JSON_PARSE_0();
			END_IF
		END_WHILE

	END_PROGRAM
\endcode

\arg Cyclic	
\code
	PROGRAM _CYCLIC

		STATUS := FILE_WEBSER(BigBuffer[0], SIZEOF(BigBuffer), ADR(CtrlVar),File_List); (* run webserver *)

		IF CtrlVar=1 THEN 												(* stop webserver between read and output process *)
			(* memset to restart the function blocks *)
			memset(ADR(JSON_STRINGIFY_0),0,SIZEOF(JSON_STRINGIFY_0));
			memset(ADR(JSON_PARSE_0),0,SIZEOF(JSON_PARSE_0));
			memset(ADR(JSONBuffer),0,SIZEOF(JSONBuffer));
	
			IF strcmp(HTTP_URL(),ADR('/write'))=0 THEN					(* start parse/write process when the url '/write' is called *)
				JSON_PARSE_0.pVariablesNotFound := ADR(VariablesNotFound);
				JSON_PARSE_0.SizeVariablesNotFound := SIZEOF(VariablesNotFound);
				JSON_PARSE_0.pDebug := ADR(Debug);
				JSON_PARSE_0.SizeDebug := SIZEOF(Debug);
				JSON_PARSE_0.pMessage := HTTP_POST(ADR(BigBuffer));		(* POST content of the Request is JSON request *)
				JSON_PARSE_0();
		
			ELSIF strcmp(HTTP_URL(),ADR('/writetofile'))=0 THEN			(* overwrite file with new content if the url 'writetofile' is called *)
				FILE_DELETE(File_List,ADR('setPVvar1.txt'));
				IF FILE_STORE(File_List,ADR('setPVvar1.txt'),HTTP_POST(ADR(BigBuffer)),strlen(HTTP_POST(ADR(BigBuffer)))) = 1 THEN
					strcpy(ADR(JSONBuffer),ADR('{"result":true}'));		(* put response the the Webserver Output *)
				ELSE
					strcpy(ADR(JSONBuffer),ADR('{"result":false}'));
				END_IF
		
			ELSIF strcmp(HTTP_URL(),ADR('/read'))=0 THEN				(* read and stringify the requested Variable *)
				strcpy(ADR(StName),HTTP_POST(ADR(BigBuffer)));
				JSON_STRINGIFY_0.pStName := ADR(StName);
				WHILE JSON_STRINGIFY_0.Status = 0 DO					(* run stringify until the whole structure is created *)
					JSON_STRINGIFY_0();
					IF (SIZEOF(JSONBuffer)-strlen(ADR(JSONBuffer))-1) > strlen(ADR(JSON_STRINGIFY_0.Output)) THEN
						strcat(ADR(JSONBuffer),ADR(JSON_STRINGIFY_0.Output));
					END_IF
				END_WHILE
			END_IF
			CtrlVar:=0; (* webserver continues with output process *)
		END_IF

	END_PROGRAM
\endcode

\section Example JSON Script
This script shows how JSON data structuring can be used to read and set variables.
The interface is easy to use and shows the user what strings the JSON parser is working with.
\image html SCRIPTOV.jpg
It is possible to select working variable and server IP.
There are three different example structures available. Main and Parameter fakes a process interface which makes the structure easy to relate to.
Ideal for presenting the library.
 
By selecting METHOD and klicking send different commands can be executed. 

READ: Reads the value(s) from the variable(structure). \n
WRITE: Writes the values present in the webbrowser to the CPU. Need to build the output first. \n
WRITE TO FILE: Stores the values to a file on the CPU so that they can be loaded after a restart. Need to build the output first. \n

Requirement: There must be a data object called "CFGDMLIB" which contains a value greater than 3 for the file handling to work.
See the standard library DM_lib for more information. Also, make sure that the runtime version is D2.31 or newer.


Some interresting line of codes for the user is the where the IP and the the working variable is set incase the user whants to use a different standard IP and variable.
\image html STANDARDIPANDVAR.jpg

Create a html file with the following content. Save and run in Internet Explorer. Change the IP to match the CPU.

\code
	<html>
	   <style type="text/css">
		table {font-family: arial; font-size : 10pt;}
		blockquote { margin-top: 0px; margin-bottom: 0px; padding: 0px; }
	   </style>



	    <head>
	        <title>JSON</title>
	        <script type="text/javascript">
        
	         var responseObject = null;
	         var req = null;
	         var output1 = '';
         
	function isArray(obj) {
	    //returns true is it is an array
	    if (obj.constructor.toString().indexOf('Array') == -1)
	        return false;
	    else
	        return true;
	}    

	function valuesToObject(o,Name) {
	    var type = typeof o 
	    var outName = Name;
	    if (type == "object") {
	        for (var key in o) {
	            if(!isArray(o)){
	                if(Name != ''){
	                    outName = Name + "['" + key + "']";
	                }
	                else{
	                    outName = Name + "['" + key + "']";
	                }
	            }
	            else{
	                outName = Name + '[' + key + ']';
	            }
	            valuesToObject(o[key],outName);
	        }
	    }
	    else{
	        var test1 = '\'' + document.getElementById(outName).value + '\'';
	        eval('responseObject' + outName + '=' + test1);
	    }
	}

        
	function js_traverse(o,Name,lastkey) {
	    var type = typeof o 
	    var outName = Name;
	    if (type == "object") {
	        if(!isArray(o))
	            output1 = output1 + '<br>{<blockquote>';
	        for (var key in o) {
	            if(!isArray(o)){
	                output1 = output1 + key;
	                if(Name != ''){
	                    outName = Name + "['" + key + "']";
	                    //outName = Name + '.' + key;
	                }
	                else{
	                    outName = Name + "['" + key + "']";
	                    //outName = Name + key;
	                }
	            }
	            else{
	                if(key != '0'){
	                    output1 = output1 + lastkey + '[' + key + ']';
	                }
	                else{
	                    output1 = output1 + '[' + key + ']';
	                }
	                outName = Name + '[' + key + ']';
	            }
	            js_traverse(o[key], outName, key);
	        }
	        if(!isArray(o))
	            output1 = output1 + '</blockquote>}<br>';
	    }
	    else{
	        output1 = output1 + ' <input type="text" id="' + outName + '" value="' + o + '"/><br>';
	    }
	}
              
	function callBack(){            
	    switch(req.readyState) {
	        case 4:
	            if(req.status!=200 && 0) {
	            alert("Errorjjj:"+req.status); 
	            }else{
	            var output_field = document.getElementById("outputObject");
	            var output1_field = document.getElementById("output1");
	            var rdState = document.getElementById('rdState');
	            test = req.responseText;
	            output1_field.innerHTML = test;
	            try{
	                responseObject = JSON.parse(test);
	                js_traverse(responseObject,'','');
	                rdState.innerHTML = 'Ready';
	            }
	            catch (e){
	                output1 = '';
	                rdState.innerHTML = 'Error';
	            }
	            output_field.innerHTML = output1;
	            output1 = '';
	            }
	        break;

	        default:
	            return false;
	        break;     
	    }
	}
            
	function call(){         
	     //create the call
     
	    try{
	        req = new XMLHttpRequest();
	    }
	    catch (e){
	        try{
	            req = new ActiveXObject("Msxml2.XMLHTTP");
	        } 
	        catch (e){
	            try{
	                req = new ActiveXObject("Microsoft.XMLHTTP");
	            } 
	            catch (failed){
	                req = null;
	            }
	        }  
	    }

	    if (req == null)
	        alert("Error creating request object!");

	    var requestString = document.getElementById('requeststring').value;
	    var readVar = document.getElementById('readVar').value;
	    var rdState = document.getElementById('rdState');
	    var ipaddress = document.getElementById('ipaddress').value;
	    //send request over GET or POST
	    if(document.getElementsByName("sendmethod")[1].checked==true){
	        var url = 'C:/xampp/xampp/htdocs/JSON/JSON-php-POST/Document.txt';
	        var url = 'http://'+ipaddress+'/read';
	        var params = readVar;
	        req.open("POST", url, true);
	        req.setRequestHeader("Content-type", "application/json");
	        req.setRequestHeader("Content-length", params.length);
	        req.setRequestHeader("Connection", "close");
	        req.onreadystatechange = callBack;
	        rdState.innerHTML = 'Reading '+readVar+' ...';
	        req.send(params);
	    }
	    else if(document.getElementsByName("sendmethod")[0].checked==true){
	        var url = 'C:/xampp/xampp/htdocs/JSON/JSON-php-POST/Document.txt';
	        var url = 'http://'+ipaddress+'/write';
	        var params = requestString;
	        req.open("POST", url, true);
	        req.setRequestHeader("Content-type", "application/json");
	        req.setRequestHeader("Content-length", params.length);
	        req.setRequestHeader("Connection", "close");
	        req.onreadystatechange = callBack;
	        rdState.innerHTML = 'Writing ...';
	        req.send(params);
	    }
	    else if(document.getElementsByName("sendmethod")[2].checked==true){
	        var url = 'C:/xampp/xampp/htdocs/JSON/JSON-php-POST/Document.txt';
	        var url = 'http://'+ipaddress+'/writetofile';
	        var params = requestString;
	        req.open("POST", url, true);
	        req.setRequestHeader("Content-type", "application/json");
	        req.setRequestHeader("Content-length", params.length);
	        req.setRequestHeader("Connection", "close");
	        req.onreadystatechange = callBack;
	        rdState.innerHTML = 'Writing to file ...';
	        req.send(params);
	    }      
	}
            
            
            
	function pdbversion(){ 
	    valuesToObject(responseObject,'');
	    document.getElementById('requeststring').value = JSON.stringify(responseObject);
	}

	function pdbbrowse(){ 
	    document.getElementById('requeststring').value = '{ "json-rpc" : "2.0", "method" : "pdb.browse", "id" : 1}';
	}
        
	        </script>
	        <script type="text/javascript">
        
	if (!this.JSON) {
	    this.JSON = {};
	}

	(function () {

	    function f(n) {
	        // Format integers to have at least two digits.
	        return n < 10 ? '0' + n : n;
	    }

	    if (typeof Date.prototype.toJSON !== 'function') {

	        Date.prototype.toJSON = function (key) {

	            return isFinite(this.valueOf()) ?
	                   this.getUTCFullYear()   + '-' +
	                 f(this.getUTCMonth() + 1) + '-' +
	                 f(this.getUTCDate())      + 'T' +
	                 f(this.getUTCHours())     + ':' +
	                 f(this.getUTCMinutes())   + ':' +
	                 f(this.getUTCSeconds())   + 'Z' : null;
	        };

	        String.prototype.toJSON =
	        Number.prototype.toJSON =
	        Boolean.prototype.toJSON = function (key) {
	            return this.valueOf();
	        };
	    }

	    var cx = /[\u0000\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g,
	        escapable = /[\\\"\x00-\x1f\x7f-\x9f\u00ad\u0600-\u0604\u070f\u17b4\u17b5\u200c-\u200f\u2028-\u202f\u2060-\u206f\ufeff\ufff0-\uffff]/g,
	        gap,
	        indent,
	        meta = {    // table of character substitutions
	            '\b': '\\b',
	            '\t': '\\t',
	            '\n': '\\n',
	            '\f': '\\f',
	            '\r': '\\r',
	            '"' : '\\"',
	            '\\': '\\\\'
	        },
	        rep;


	    function quote(string) {

	        escapable.lastIndex = 0;
	        return escapable.test(string) ?
	            '"' + string.replace(escapable, function (a) {
	                var c = meta[a];
	                return typeof c === 'string' ? c :
	                    '\\u' + ('0000' + a.charCodeAt(0).toString(16)).slice(-4);
	            }) + '"' :
	            '"' + string + '"';
	    }


	    function str(key, holder) {


	        var i,          // The loop counter.
	            k,          // The member key.
	            v,          // The member value.
	            length,
	            mind = gap,
	            partial,
	            value = holder[key];


	        if (value && typeof value === 'object' &&
	                typeof value.toJSON === 'function') {
	            value = value.toJSON(key);
	        }


	        if (typeof rep === 'function') {
	            value = rep.call(holder, key, value);
	        }


	        switch (typeof value) {
	        case 'string':
	            return quote(value);

	        case 'number':


	            return isFinite(value) ? String(value) : 'null';

	        case 'boolean':
	        case 'null':


	            return String(value);

	        case 'object':

	            if (!value) {
	                return 'null';
	            }


	            gap += indent;
	            partial = [];


	            if (Object.prototype.toString.apply(value) === '[object Array]') {


	                length = value.length;
	                for (i = 0; i < length; i += 1) {
	                    partial[i] = str(i, value) || 'null';
	                }


	                v = partial.length === 0 ? '[]' :
	                    gap ? '[\n' + gap +
	                            partial.join(',\n' + gap) + '\n' +
	                                mind + ']' :
	                          '[' + partial.join(',') + ']';
	                gap = mind;
	                return v;
	            }


	            if (rep && typeof rep === 'object') {
	                length = rep.length;
	                for (i = 0; i < length; i += 1) {
	                    k = rep[i];
	                    if (typeof k === 'string') {
	                        v = str(k, value);
	                        if (v) {
	                            partial.push(quote(k) + (gap ? ': ' : ':') + v);
	                        }
	                    }
	                }
	            } else {


	                for (k in value) {
	                    if (Object.hasOwnProperty.call(value, k)) {
	                        v = str(k, value);
	                        if (v) {
	                            partial.push(quote(k) + (gap ? ': ' : ':') + v);
	                        }
	                    }
	                }
	            }


	            v = partial.length === 0 ? '{}' :
	                gap ? '{\n' + gap + partial.join(',\n' + gap) + '\n' +
	                        mind + '}' : '{' + partial.join(',') + '}';
	            gap = mind;
	            return v;
	        }
	    }


	    if (typeof JSON.stringify !== 'function') {
	        JSON.stringify = function (value, replacer, space) {


	            var i;
	            gap = '';
	            indent = '';

	            if (typeof space === 'number') {
	                for (i = 0; i < space; i += 1) {
	                    indent += ' ';
	                }

	            } else if (typeof space === 'string') {
	                indent = space;
	            }

	            rep = replacer;
	            if (replacer && typeof replacer !== 'function' &&
	                    (typeof replacer !== 'object' ||
	                     typeof replacer.length !== 'number')) {
	                throw new Error('JSON.stringify');
	            }


	            return str('', {'': value});
	        };
	    }


	    if (typeof JSON.parse !== 'function') {
	        JSON.parse = function (text, reviver) {

	            var j;

	            function walk(holder, key) {

	                var k, v, value = holder[key];
	                if (value && typeof value === 'object') {
	                    for (k in value) {
	                        if (Object.hasOwnProperty.call(value, k)) {
	                            v = walk(value, k);
	                            if (v !== undefined) {
	                                value[k] = v;
	                            } else {
	                                delete value[k];
	                            }
	                        }
	                    }
	                }
	                return reviver.call(holder, key, value);
	            }

	            text = String(text);
	            cx.lastIndex = 0;
	            if (cx.test(text)) {
	                text = text.replace(cx, function (a) {
	                    return '\\u' +
	                        ('0000' + a.charCodeAt(0).toString(16)).slice(-4);
	                });
	            }


	            if (/^[\],:{}\s]*$/
	.test(text.replace(/\\(?:["\\\/bfnrt]|u[0-9a-fA-F]{4})/g, '@')
	.replace(/"[^"\\\n\r]*"|true|false|null|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?/g, ']')
	.replace(/(?:^|:|,)(?:\s*\[)+/g, ''))) {


	                j = eval('(' + text + ')');


	                return typeof reviver === 'function' ?
	                    walk({'': j}, '') : j;
	            }

	            throw new SyntaxError('JSON.parse');
	        };
	    }
	}());
        
	        </script>

	    </head>
	<body>
	<div style="text-align:center;">
	<table style="width: 800px; margin-left: auto;
	margin-right: auto;" border="0" cellpadding="0" cellspacing="0">
	  <tbody>
	    <tr>
	      <td style="height: 50px; background-color: rgb(255, 131, 0);"><font size="+2">&nbsp;<span style="font-weight: bold; font-size : 20pt;">BRSE JSON</span></font></td>
	    </tr>
	    <tr>
	      <td style="height: 10px; background-color: rgb(102, 102, 102);"></td>
	    </tr>
	    <tr>
	      <td>
	      <table style="width: 100%; border-collapse: collapse;" border="1" bordercolor="#666666" cellpadding="5" cellspacing="0"><tr><td style="background-color: rgb(204, 204, 204);">
		        Outgoing JSON: <br>
	            <textarea cols="95" rows="10" id="requeststring"></textarea><br />
		        Variable to Read: <input type="text" id="readVar" value="PV_var1"/><br />
		        IP Address: <input type="text" id="ipaddress" value="10.46.10.211"/><br />
	            Method: <input type="radio" name="sendmethod" value="POST"> WRITE
	                <input type="radio" name="sendmethod" value="POST" checked> READ
	                <input type="radio" name="sendmethod" value="POST"> WRITE TO FILE<br>
	                <input type="submit" value="Send" onclick="call();" /> 
	                <input type="submit" value="Build Output" onclick="pdbversion();" /><br>
	            <br />
	            Status: <b id="rdState" style="color:black"></b><br>
	            </td>
	            </tr>
	            <tr><td>
	            <div id="outputObject"></div><br>
	            <div id="output1" style="color:green; word-break:break-all; word-wrap:break-word;"></div>
	            </td>
	            </tr>
	            <tr>
	            <td>
	            </td>
	            </tr>
	      </table>
	      </td>
	    </tr>
	    <tr>
	      <td style="height: 5px; background-color: rgb(102, 102, 102);"></td>
	    </tr>
	  </tbody>
	</table>
	</div>

	    </body>
	</html>
\endcode

*/
