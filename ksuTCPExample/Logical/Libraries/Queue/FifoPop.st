
(* TODO: Add your comment here *)
FUNCTION FifoPop
	
	//we want to return something more complex than a basic data type but we can't. Instead, just use pop to remove the top item without returning it
	FifoPop := 0;
	//FifoPop := Queue.Queue[0];
	
	brsmemmove(ADR(Queue.Queue[0]), ADR(Queue.Queue[1]), SIZEOF(Queue.Queue) - SIZEOF(Queue.Queue[0]));
	
	IF Queue.NextAvailableIndex > 0 THEN
		Queue.NextAvailableIndex := Queue.NextAvailableIndex - 1;
	END_IF
	
END_FUNCTION
