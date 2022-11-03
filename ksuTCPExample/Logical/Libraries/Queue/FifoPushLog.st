
(* TODO: Add your comment here *)
FUNCTION FifoPushLog
	
	
	Queue.Queue[Queue.NextAvailableIndex] := Value;
	
	Queue.NextAvailableIndex := Queue.NextAvailableIndex + 1;
	
	IF Queue.NextAvailableIndex > QUEUE_SIZE_M1 THEN
		Queue.NextAvailableIndex := QUEUE_SIZE_M1;
	END_IF
	
	FifoPushLog := TRUE;
	
END_FUNCTION
