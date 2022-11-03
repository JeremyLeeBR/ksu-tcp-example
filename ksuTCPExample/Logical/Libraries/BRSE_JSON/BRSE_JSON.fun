(********************************************************************
 * COPYRIGHT -- B&R
 ********************************************************************
 * Library: Library
 * File: Library.fun
 * Author: Florian Damerow
 * Created: March 09, 2011
 ********************************************************************
 * Functions and function blocks of library Library
 ********************************************************************)

FUNCTION_BLOCK JSON_Parse (*parses a JSON message and writes the content to the structure on the PLC*)
	VAR_INPUT
		pMessage : REFERENCE TO UDINT; (*Pointer to the JSON Message (String)*)
		pDebug : REFERENCE TO UDINT; (*Pointer Debug Buffer (String)*)
		pVariablesNotFound : REFERENCE TO UDINT; (*Pointer Buffer of VariablesNotFound (String)*)
		SizeVariablesNotFound : UDINT; (*size of the VariablesNotFound Buffer*)
		SizeDebug : UDINT; (*size of the Debug Buffer*)
	END_VAR
	VAR_OUTPUT
		ErrorInfo : UINT;
	END_VAR
	VAR
		parselevel : USINT;
		pvIndex : ARRAY[0..MAX_STRUCT_DEPTH_Parse] OF INT;
		Buffer : STRING[80];
		parsestateold : USINT;
		parsestate : USINT;
		pvName : ARRAY[0..MAX_STRUCT_DEPTH_Parse] OF STRING[64];
		isArray : ARRAY[0..MAX_STRUCT_DEPTH_Parse] OF BOOL;
		fullName : STRING[MAX_STRUCT_NAME_LENGTH_Parse];
		j : UINT;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK JSON_Stringify (*converts a PV variable and the whole structure into a JSON Object using PV_ninfo*)
	VAR_INPUT
		pStName : REFERENCE TO UDINT; (*address to variable name (STRING)*)
	END_VAR
	VAR_OUTPUT
		Status : UINT; (*address to status-variable 0=not done; 1=done; 2=error*)
		Output : STRING[MAX_OUTPUT_LENGTH_Stringify]; (*Output contains the content of one step*)
	END_VAR
	VAR
		tmpStruct : STRING[MAX_OUTPUT_LENGTH_Stringify];
		pvName : ARRAY[0..MAX_STRUCT_DEPTH_Stringify] OF STRING[100];
		j : UDINT;
		pvIndex : ARRAY[0..MAX_STRUCT_DEPTH_Stringify] OF UINT;
		TaskDoc : TaskDoc1_typ;
		pvType : UDINT;
		pvLength : UDINT;
		arrMem : ARRAY[0..MAX_STRUCT_DEPTH_Stringify] OF UINT;
		pvStructMembers : ARRAY[0..MAX_STRUCT_DEPTH_Stringify] OF UINT;
		tmpValue : STRING[MAX_OUTPUT_LENGTH_Stringify];
		Buffer : STRING[MAX_OUTPUT_LENGTH_Stringify];
		i : DINT;
		pvLevel : DINT;
		tmpDepth : STRING[MAX_OUTPUT_LENGTH_Stringify];
		state : USINT;
		pvContinue : BOOL;
		pvSearchComplete : BOOL;
		data_len : UDINT;
		isArray : ARRAY[0..MAX_STRUCT_DEPTH_Stringify] OF BOOL;
		arrIndex : UINT;
		pv_adresse : UDINT;
	END_VAR
END_FUNCTION_BLOCK
