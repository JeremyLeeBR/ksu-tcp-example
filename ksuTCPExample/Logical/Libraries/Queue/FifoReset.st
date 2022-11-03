
(* TODO: Add your comment here *)
FUNCTION FifoReset
	
	brsmemset(ADR(Queue), 0, SIZEOF(Queue));
	
	FifoReset := TRUE;
	
END_FUNCTION
