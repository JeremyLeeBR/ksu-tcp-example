
TYPE
	FifoQueue_typ : 	STRUCT 
		NextAvailableIndex : UDINT;
		Queue : ARRAY[0..QUEUE_SIZE_M1]OF Data_typ;
	END_STRUCT;
	Data_typ : 	STRUCT 
		Data : ARRAY[0..MAX_DATA_LENGTH_M1]OF USINT;
	END_STRUCT;
	FifoLogQueue_typ : 	STRUCT 
		NextAvailableIndex : UDINT;
		Queue : ARRAY[0..QUEUE_SIZE_M1]OF Data_Log_typ;
	END_STRUCT;
	Data_Log_typ : 	STRUCT 
		Data : ARRAY[0..MAX_DATA_LOG_LENGTH_M1]OF USINT;
	END_STRUCT;
END_TYPE
