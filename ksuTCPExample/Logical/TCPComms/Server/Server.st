(********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Program: Server
 * File: Server.st
 * Author: Bernecker + Rainer
 * Created: February 21, 2008
 ********************************************************************
 * Implementation of program Server
 ********************************************************************)
PROGRAM _INIT
	
	OwnPort := 3000;
	//OwnPort := 10000;
	Server.client_address := '127.0.0.1';
	TON_Reopen.PT := t#10s;
	TON_MessageTimeout.PT := t#100ms;
	
END_PROGRAM

PROGRAM _CYCLIC
	
	//Server Idle Status
	gServerIdle := Server.sStep = 20;


	CASE Server.sStep OF
 	
		0:  (* Open Ethernet Interface *)
			prevStep := Server.sStep;
			Server.TcpOpen_0.enable := 1;	
			Server.TcpOpen_0.pIfAddr := 0;  (* Listen on all TCP/IP Interfaces*)
			Server.TcpOpen_0.port := OwnPort;  (* Port to listen*)
			Server.TcpOpen_0.options := 0;	
			Server.TcpOpen_0;  (* Call the Function*)
						
			IF Server.TcpOpen_0.status = 0 THEN  (* TcpOpen successfull*)
				Server.sStep := 5;	
			ELSIF Server.TcpOpen_0.status = ERR_FUB_BUSY THEN  (* TcpOpen not finished -> redo *)			
			  (* Busy *)	
			ELSE  (* Goto Error Step *)
				Server.sStep := 100;
			END_IF
		
		5:
			prevStep := Server.sStep;
			Server.linger_opt.lLinger := 0; (* linger Time = 0 *)
			Server.linger_opt.lOnOff := 0; (* linger Option OFF *)
				 
		 	Server.TcpIoctl_0.enable := 1;
			Server.TcpIoctl_0.ident := Server.TcpOpen_0.ident; (* Connection Ident from AsTCP.TCP_Open *)
			Server.TcpIoctl_0.ioctl := tcpSO_LINGER_SET; (* Set Linger Options *)
			Server.TcpIoctl_0.pData := ADR(Server.linger_opt);
			Server.TcpIoctl_0.datalen := SIZEOF(Server.linger_opt);
   			Server.TcpIoctl_0;	    
		
			IF Server.TcpIoctl_0.status = 0 THEN  (* TcpIoctl successfull *)
				Server.sStep := 10;											
			ELSIF Server.TcpIoctl_0.status = ERR_FUB_BUSY THEN  (* TcpIoctl not finished -> redo *)				
			  (* Busy *)
			ELSE  (* Goto Error Step *)
				Server.sStep := 100;
			END_IF
		
		10: (* Wait for Client Connection *)
			prevStep := Server.sStep;
			Server.TcpServer_0.enable := 1;
			Server.TcpServer_0.ident := Server.TcpOpen_0.ident;  (* Connection Ident from AsTCP.TCP_Open *)
			Server.TcpServer_0.backlog := 1;  (* Number of clients waiting simultaneously for a connection*)
			Server.TcpServer_0.pIpAddr := ADR(Server.client_address);  (* Where to write the client IP-Address*)
			Server.TcpServer_0;  (* Call the Function*)
			
			IF Server.TcpServer_0.status = 0 THEN  (* Status = 0 if an client connects to server *)
				Server.sStep := 15;
			ELSIF Server.TcpServer_0.status = ERR_FUB_BUSY THEN  (* TcpServer not finished -> redo *)			
			  (* Busy *)	
			ELSE  (* Goto Error Step *)
				Server.sStep := 100;
			END_IF
			
			
		15:
			prevStep := Server.sStep;
		 	Server.TcpIoctl_0.enable := 1;
			Server.TcpIoctl_0.ident := Server.TcpServer_0.identclnt; (* Connection Ident from AsTCP.TCP_Server *)
			Server.TcpIoctl_0.ioctl := tcpSO_LINGER_SET;  (* Set Linger Options *)
			Server.TcpIoctl_0.pData := ADR(Server.linger_opt);
			Server.TcpIoctl_0.datalen := SIZEOF(Server.linger_opt);
   			Server.TcpIoctl_0;	    
		
			IF Server.TcpIoctl_0.status = 0 THEN  (* TcpIoctl successfull *)
				Server.sStep := 20;											
			ELSIF Server.TcpIoctl_0.status = ERR_FUB_BUSY THEN  (* TcpIoctl not finished -> redo *)				
			  (* Busy *)
			ELSE  (* Goto Error Step *)
				Server.sStep := 100;
			END_IF
			
		20: (* Wait for Data *)
			prevStep := Server.sStep;
			Server.TcpRecv_0.enable := 1;
			Server.TcpRecv_0.ident := Server.TcpServer_0.identclnt;  (* Client Ident from AsTCP.TCP_Server *)	
			Server.TcpRecv_0.pData	:= ADR(Server.data_buffer);  (* Where to store the incoming data *)
			Server.TcpRecv_0.datamax := SIZEOF(Server.data_buffer);  (* Lenght of data buffer *)
			Server.TcpRecv_0.flags := 0;
			Server.TcpRecv_0;  (* Call the Function*)
		
			IF Server.TcpRecv_0.status = 0 AND (Server.data_buffer[1] = 0 OR Server.data_buffer[1] = (gMessageSequenceNumberServer)) THEN  (* Data received and Message count match*)
				
				Server.recv_timeout := 0;
				
				//Log Received message based on type
				brsmemset(ADR(LogMessageData.AdditionalData), 0, SIZEOF(LogMessageData.AdditionalData));
				brsmemset(ADR(LogQueueData), 0, SIZEOF(LogQueueData));	
				LogMessageData.AdditionalDataFormat := arEVENTLOG_ADDFORMAT_TEXT;
				LogMessageData.EnteredBy := 'B&R Msg In';
				LogMessageData.EventID := ArEventLogMakeEventID(Severity := arEVENTLOG_SEVERITY_INFO, Facility := UDINT_TO_UINT(Server.data_buffer[2]), ErrorCode := 0);
				
				CASE Server.data_buffer[2] OF
					
					0: //Heartbeat
						LogMessageData.AdditionalData := 'Heartbeat';
						Server.sStep := 30;
					
					1: //Reset after error
						gProcess.Commands.Reset := TRUE;
						LogMessageData.AdditionalData := 'Error Reset';
						Server.sStep := 30;
						
					2: //Start the Track
						gProcess.Commands.PowerOn := TRUE;
						LogMessageData.AdditionalData := 'Trak Power On';
						Server.sStep := 30;
						
					3: //Run the Track
						gProcess.Commands.Start := TRUE;
						LogMessageData.AdditionalData := 'Trak Start';
						Server.sStep := 30;
						
					4: //Stop the Track
						gProcess.Commands.Stop := TRUE;
						LogMessageData.AdditionalData := 'Trak Stop';
						gProcess.Status.StoppedTrack := Server.data_buffer[4];
						Server.sStep := 30;
						
					
					5: //Request shuttle to a Carton insert station.
						brsmemcpy(ADR(RouteUINTArray),ADR(Server.data_buffer[4]), 8);
						Byte2Bit_0(byteadr := ADR(RouteUINTArray[0]), length := 16); (* convert byte array into bit array *)
						brsmemcpy(ADR(RouteBoolArray[0]), Byte2Bit_0.bitadr, 16);
						Byte2Bit_0(byteadr := ADR(RouteUINTArray[1]), length := 16); (* convert byte array into bit array *)
						brsmemcpy(ADR(RouteBoolArray[16]), Byte2Bit_0.bitadr, 16);
						Byte2Bit_0(byteadr := ADR(RouteUINTArray[2]), length := 16); (* convert byte array into bit array *)
						brsmemcpy(ADR(RouteBoolArray[32]), Byte2Bit_0.bitadr, 16);
						Byte2Bit_0(byteadr := ADR(RouteUINTArray[3]), length := 16); (* convert byte array into bit array *)
						brsmemcpy(ADR(RouteBoolArray[48]), Byte2Bit_0.bitadr, 16);
						
						LogMessageData.AdditionalData := 'Shuttle Request, Shuttle Route: ';
						
						j := 0;
						FOR i := 0 TO 63 DO
							IF RouteBoolArray[i] = TRUE THEN
								ShuttleRoute[j] := i;
								j := j + 1;
								routeStep := INT_TO_STRING(i);
								brsstrcat(ADR(LogMessageData.AdditionalData), ADR(routeStep));
								brsstrcat(ADR(LogMessageData.AdditionalData), ADR(' '));
							END_IF
						END_FOR;
						ShuttleRoute[j] := UDINT_TO_USINT(SCAN);
						brsmemcpy(ADR(Data),ADR(ShuttleRoute),SIZEOF(ShuttleRoute));
						FifoPush(ADR(gShutReqQueue), Data);
						Server.sStep := 30;
					
					7: //Release shuttle from station
						IF (gShuttleIDAtLocation[Server.data_buffer[5]] <> 0) THEN
							gReleaseFromStation[Server.data_buffer[5]] := TRUE;
						END_IF
						
						LogMessageData.AdditionalData := 'Release Shuttle, ';
						brsstrcat(ADR(LogMessageData.AdditionalData), ADR('Shuttle ID: '));
						brsmemcpy(ADR(shuttleID_int), ADR(gShuttleIDAtLocation[Server.data_buffer[5]]), 4);
						shuttleID := INT_TO_STRING(shuttleID_int);
						brsstrcat(ADR(LogMessageData.AdditionalData), ADR(shuttleID));
						brsstrcat(ADR(LogMessageData.AdditionalData), ADR(' Shuttle Loc: '));
						brsmemcpy(ADR(shuttleLoc_int), ADR(Server.data_buffer[5]), 8);
						shuttleLoc := UDINT_TO_STRING(shuttleLoc_int);
						brsstrcat(ADR(LogMessageData.AdditionalData), ADR(shuttleLoc));	
						
						Server.sStep := 30;
						
					9: //Shuttle Route
						
						//brsmemcpy(ADR(gShuttleRoutes[Server.data_buffer[4], 0]), ADR(Server.data_buffer[5]), 8);
						brsmemcpy(ADR(RouteUINTArray),ADR(Server.data_buffer[5]), 8);
						Byte2Bit_0(byteadr := ADR(RouteUINTArray[0]), length := 16); (* convert byte array into bit array *)
						brsmemcpy(ADR(RouteBoolArray[0]), Byte2Bit_0.bitadr, 16);
						Byte2Bit_0(byteadr := ADR(RouteUINTArray[1]), length := 16); (* convert byte array into bit array *)
						brsmemcpy(ADR(RouteBoolArray[16]), Byte2Bit_0.bitadr, 16);
						Byte2Bit_0(byteadr := ADR(RouteUINTArray[2]), length := 16); (* convert byte array into bit array *)
						brsmemcpy(ADR(RouteBoolArray[32]), Byte2Bit_0.bitadr, 16);
						Byte2Bit_0(byteadr := ADR(RouteUINTArray[3]), length := 16); (* convert byte array into bit array *)
						brsmemcpy(ADR(RouteBoolArray[48]), Byte2Bit_0.bitadr, 16);
						
						LogMessageData.AdditionalData := 'Route Decision, Shuttle Route: ';
						
						j := 0;
						FOR i := 0 TO 63 DO
							IF RouteBoolArray[i] = TRUE THEN
								gShuttleRoutes[Server.data_buffer[4], j] := i;
								j := j + 1;
								routeStep := INT_TO_STRING(i);
								brsstrcat(ADR(LogMessageData.AdditionalData), ADR(routeStep));
								brsstrcat(ADR(LogMessageData.AdditionalData), ADR(' '));
							END_IF
						END_FOR;
						
						brsstrcat(ADR(LogMessageData.AdditionalData), ADR(' ShuttleID: '));
						brsmemcpy(ADR(shuttleID_int), ADR(Server.data_buffer[4]), 4);
						shuttleID := INT_TO_STRING(shuttleID_int);
						brsstrcat(ADR(LogMessageData.AdditionalData), ADR(shuttleID));
						
						gRouteRecieved[Server.data_buffer[4]] := TRUE;
						gBypassLHD[Server.data_buffer[4]] := FALSE;
						Server.sStep := 30;
						
					11: //Bypass Lid Hold Down
						
						LogMessageData.AdditionalData := 'Bypass Lid Hold-down, Shuttle ID: ';
						brsmemcpy(ADR(shuttleID_int), ADR(Server.data_buffer[4]), 4);
						shuttleID := INT_TO_STRING(shuttleID_int);
						brsstrcat(ADR(LogMessageData.AdditionalData), ADR(shuttleID));
						
						gBypassLHD[Server.data_buffer[4]] := TRUE;
						Server.sStep := 30;
					
					ELSE
						//Add error message (TODO)
						Server.sStep := 30;
					
					
				END_CASE
				
				//Log message if it is not heartbeat
				IF Server.data_buffer[2] <> 0 THEN
					LogMessageData.AdditionalDataSize := LEN(LogMessageData.AdditionalData);
					brsmemcpy(ADR(LogQueueData), ADR(LogMessageData), SIZEOF(LogMessageData));
					FifoPush(ADR(gLogQueue), LogQueueData);
				END_IF
				
				//Add ACK message
				brsmemset(ADR(Server.data_buffer), 0, SIZEOF(Server.data_buffer));
				Server.data_buffer[0] := 0;
				Server.data_buffer[1] := gMessageSequenceNumberServer;
				
				gMessageSequenceNumberServer := Server.data_buffer[1] + 1;
			
			ELSIF Server.TcpRecv_0.status = 0 AND (Server.data_buffer[1] = (gMessageSequenceNumberServer - 1)) THEN  (* Data received and Message count is previous sequence number *)
				//Send ACK
				brsmemset(ADR(Server.data_buffer), 0, SIZEOF(Server.data_buffer));
				Server.data_buffer[0] := 0;
				Server.data_buffer[1] := gMessageSequenceNumberServer - 1;
				gMessageSequenceNumberServer := Server.data_buffer[1] + 1;
				Server.sStep := 30;
		
			ELSIF Server.TcpRecv_0.status = 0 AND NOT(Server.data_buffer[1] = 0 OR Server.data_buffer[1] = (gMessageSequenceNumberServer)) THEN  (* Data received and Message count mismatch*)
				//Add NAK message. Will need to implement specific error codes 
				TempSeqNum := Server.data_buffer[1];
				brsmemset(ADR(Server.data_buffer), 0, SIZEOF(Server.data_buffer));
				Server.data_buffer[0] := 1;
				Server.data_buffer[1] := TempSeqNum;
				Server.data_buffer[2] := LEN('Sequence Number Mismatch');
				brsmemcpy(ADR(Server.data_buffer[3]), ADR('Sequence Number Mismatch'),LEN('Sequence Number Mismatch'));
				Server.sStep := 30;
				
			ELSIF Server.TcpRecv_0.status = tcpERR_NO_DATA THEN  (* No data received - wait *)
			   Server.recv_timeout := Server.recv_timeout + 1;
			  IF Server.recv_timeout > 17500 THEN //Wait 35s
	 		    Server.sStep := 40;
				Server.recv_timeout := 0;
			  END_IF
			ELSIF Server.TcpRecv_0.status = ERR_FUB_BUSY THEN  (* TcpRecv not finished -> redo *)					
			  (* Busy *)
			ELSIF Server.TcpRecv_0.status = tcpERR_NOT_CONNECTED THEN
				Server.sStep := 40;
			ELSE  (* Goto Error Step *)
				Server.sStep := 100;
			END_IF
		
			
		
		30:	(* Send Data back to Client *)	
			prevStep := Server.sStep;
			Server.TcpSend_0.enable := 1;				
			Server.TcpSend_0.ident := Server.TcpServer_0.identclnt;  (* Client Ident from AsTCP.TCP_Server *)	
			Server.TcpSend_0.pData := ADR(Server.data_buffer[0]);  (* Which data to send *)
			IF Server.data_buffer[0] = 0 THEN
				Server.TcpSend_0.datalen := 8;  (* Lenght of data to send *)	
			ELSE
				Server.TcpSend_0.datalen := 12 + Server.data_buffer[2];
			END_IF
			Server.TcpSend_0.flags := 0;
			Server.TcpSend_0;  (* Call the Function*)
				
			IF Server.TcpSend_0.status = 0 THEN  (* Data sent *)
				Server.sStep := 20;		
				
			ELSIF Server.TcpSend_0.status = ERR_FUB_BUSY THEN  (* TcpSend not finished -> redo *)						
			  (* Busy *)
			ELSIF (Server.TcpSend_0.status = tcpERR_NOT_CONNECTED) THEN
				Server.sStep := 40;
			ELSE  (* Goto Error Step *)
				Server.sStep := 100;
			END_IF
			
		40:
			prevStep := Server.sStep;
			Server.TcpClose_0.enable := 1;
			Server.TcpClose_0.ident := Server.TcpServer_0.identclnt;
			Server.TcpClose_0.how := 0; // tcpSHUT_RD OR tcpSHUT_WR;
			Server.TcpClose_0;
			
			IF Server.TcpClose_0.status = 0 THEN
   				Server.sStep := 50;
			ELSIF Server.TcpClose_0.status = ERR_FUB_BUSY THEN  (* TcpClose not finished -> redo *)	
			  (* Busy *)
			ELSE  (* Goto Error Step *)
				Server.sStep := 100;
			END_IF	
		50: 
			prevStep := Server.sStep;
			Server.TcpClose_0.enable := 1;
			Server.TcpClose_0.ident := Server.TcpOpen_0.ident;
			Server.TcpClose_0.how := 0; //tcpSHUT_RD OR tcpSHUT_WR;
			Server.TcpClose_0;
			
			IF Server.TcpClose_0.status = 0 THEN
   				Server.sStep := 0;
			ELSIF Server.TcpClose_0.status = ERR_FUB_BUSY THEN  (* TcpClose not finished -> redo *)	
			  (* Busy *)
			ELSE  (* Goto Error Step *)
				Server.sStep := 100;
			END_IF
						
	    
		100: (* Error State. Wait 10s, reopen port and try again *)
			
			TON_Reopen.IN := TRUE;
			IF TON_Reopen.Q THEN
				Server.sStep := prevStep;
				TON_Reopen.IN := FALSE;
			END_IF
		

	END_CASE
		
	TON_Reopen();
	TON_MessageTimeout();



END_PROGRAM

