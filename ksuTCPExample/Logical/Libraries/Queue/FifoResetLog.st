
(* TODO: Add your comment here *)
FUNCTION FifoResetLog
	
	brsmemset(ADR(Queue), 0, SIZEOF(Queue));
	
	FifoResetLog := TRUE;
	
END_FUNCTION
