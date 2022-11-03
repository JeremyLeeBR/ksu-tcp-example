
{REDUND_ERROR} FUNCTION FifoPush : BOOL (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Queue : REFERENCE TO FifoQueue_typ;
		Value : Data_typ;
	END_VAR
END_FUNCTION

{REDUND_ERROR} FUNCTION FifoPop : UINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Queue : REFERENCE TO FifoQueue_typ;
	END_VAR
END_FUNCTION

{REDUND_ERROR} FUNCTION FifoReset : BOOL (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Queue : REFERENCE TO FifoQueue_typ;
	END_VAR
END_FUNCTION

{REDUND_ERROR} FUNCTION FifoPushLog : BOOL (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Queue : REFERENCE TO FifoLogQueue_typ;
		Value : Data_Log_typ;
	END_VAR
END_FUNCTION

{REDUND_ERROR} FUNCTION FifoPopLog : UINT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Queue : REFERENCE TO FifoLogQueue_typ;
	END_VAR
END_FUNCTION

{REDUND_ERROR} FUNCTION FifoResetLog : BOOL (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		Queue : REFERENCE TO FifoLogQueue_typ;
	END_VAR
END_FUNCTION
