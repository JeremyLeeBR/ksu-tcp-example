(********************************************************************
 * COPYRIGHT -- B&R
 ********************************************************************
 * Library: BRSE_JSON
 * File: BRSE_JSON.typ
 * Author: Florian Damerow
 * Created: March 09, 2011
 ********************************************************************
 * Data types of library BRSE_JSON
 ********************************************************************)

TYPE
	TaskDoc1_typ : 	STRUCT 
		Output : Output1_typ;
		Input : Input1_typ;
		State : USINT;
	END_STRUCT;
	Input1_typ : 	STRUCT 
		StructDepth : DINT;
	END_STRUCT;
	Output1_typ : 	STRUCT 
		StatusGet : UINT;
		StatusInfo : UINT;
		StatusSearch : UINT;
	END_STRUCT;
END_TYPE
