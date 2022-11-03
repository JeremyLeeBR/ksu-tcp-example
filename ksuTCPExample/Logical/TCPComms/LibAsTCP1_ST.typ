(********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Package: LibAsTCP
 * File: LibAsTCP.typ
 * Author: Bernecker + Rainer
 * Created: February 21, 2008
 ********************************************************************
 * Data types of package LibAsTCP
 ********************************************************************)

TYPE
	LibAsTCP1_ST_typ : 	STRUCT  (*Datatyp for global Variables*)
		receive_data : ARRAY[0..400]OF USINT; (*Data which has been received*)
		send_data : ARRAY[0..100]OF UDINT; (*Data which should be sent*)
	END_STRUCT;
END_TYPE
