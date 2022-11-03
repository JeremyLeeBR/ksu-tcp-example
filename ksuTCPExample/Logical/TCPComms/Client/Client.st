(********************************************************************
 * COPYRIGHT -- Bernecker + Rainer
 ********************************************************************
 * Program: Client
 * File: Client.st
 * Author: Bernecker + Rainer
 * Created: February 21, 2008
 ********************************************************************
 * Implementation of program Client
 ********************************************************************)
PROGRAM _INIT
	

	
	//blank out queue just in case it initializes with non-zero info
	FifoReset(ADR(gSendQueue));
	TON_Reopen.PT := t#10s;
	
	ServerIP := '127.0.0.1';
	//ServerIP := '192.168.0.102';
	ServerPort := 9999;
	
	gMessageToEZ3.Move_Command := 'Auto_Run';
	gMessageToEZ3.Battery_Subscribe := 1;
	
END_PROGRAM


PROGRAM _CYCLIC
		
	IF CreateTestMessage1 THEN
		CreateTestMessage1 := FALSE;
		
		//brsmemset(ADR(gMessageToEZ3), 0, SIZEOF(gMessageToEZ3));
		
		
		brsmemset(ADR(message_tmp), 0, SIZEOF(message_tmp));
		brsmemset(ADR(JSON_Stringify_0),0,SIZEOF(JSON_Stringify_0));;
				
		REPEAT
			JSON_Stringify_0(pStName := ADR('gMessageToEZ3'));
			brsstrcat(ADR(message_tmp),ADR(JSON_Stringify_0.Output));
			UNTIL JSON_Stringify_0.Status <> 0
		END_REPEAT;
		
		FifoPush(ADR(gSendQueue), message_tmp);
	END_IF

	

	CASE Client.sStep OF
 	
		0: (* Open Ethernet Interface *)
			prevStep := Client.sStep;
			Client.TcpOpen_0.enable := 1;	
			Client.TcpOpen_0.pIfAddr := 0;  (* Which Interface to open *)
			Client.TcpOpen_0.port := 12001;  (* Port on client side to use *)
			Client.TcpOpen_0.options := 0;	
			Client.TcpOpen_0;  (* Call the Function *)
						
			IF Client.TcpOpen_0.status = 0 THEN  (* TcpOpen successfull *)
				Client.sStep := 5;											
			ELSIF Client.TcpOpen_0.status = ERR_FUB_BUSY THEN  (* TcpOpen not finished -> redo *)				
				(* Busy *)
			ELSE  (* Goto Error Step *)
				prevStep := Client.sStep;
				Client.sStep := 100;
			END_IF
		
		5: 	
			prevStep := Client.sStep;
			Client.linger_opt.lLinger := 0; (* linger Time = 0 *)
			Client.linger_opt.lOnOff := 0; (* linger Option OFF *)
				 
			Client.TcpIoctl_0.enable := 1;
			Client.TcpIoctl_0.ident := Client.TcpOpen_0.ident; (* Connection Ident from AsTP.TCP_Open *)
			Client.TcpIoctl_0.ioctl := tcpSO_LINGER_SET; (* Set Linger Options *)
			Client.TcpIoctl_0.pData := ADR(Client.linger_opt);
			Client.TcpIoctl_0.datalen := SIZEOF(Client.linger_opt);
			Client.TcpIoctl_0;	    
		
			IF Client.TcpIoctl_0.status = 0 THEN  (* TcpIoctl successfull *)
				Client.sStep := 10;											
			ELSIF Client.TcpIoctl_0.status = ERR_FUB_BUSY THEN  (* TcpIoctl not finished -> redo *)				
				(* Busy *)
			ELSE  (* Goto Error Step *)
				Client.sStep := 100;
			END_IF
		
		
		10: (* Connect to the other Station *)		
			prevStep := Client.sStep;
			Client.TcpClient_0.enable := 1;
			Client.TcpClient_0.ident := Client.TcpOpen_0.ident;  (* Connection Ident from AsTCP.TCP_Open *)
			Client.TcpClient_0.portserv := ServerPort;  (* Port on server side to use *)
			Client.TcpClient_0.pServer := ADR(ServerIP);  (* Server Address *)
			Client.TcpClient_0;  (* Call the Function*)
			
			
			
			IF Client.TcpClient_0.status = 0 THEN  (* Open ok -> Send Data *)
				Client.sStep := 15;	
			ELSIF Client.TcpClient_0.status = ERR_FUB_BUSY THEN  (* TcpClient not finished -> redo *)	
				(* Busy *)
			ELSIF Client.TcpClient_0.status = tcpERR_INVALID THEN  (* Port error -> Close actual connection, and reopen a new one in 10s *)
				Client.sStep := 40;
				ReconnectWait := TRUE;
			ELSE  (* Goto Error Step *)
				Client.sStep := 100;
			END_IF
			
		15:
			IF gSendQueue.NextAvailableIndex > 0 THEN
				// copy the top item of the queue and then pop it
				// our functions can't return non-basic types, so I can't pop right into the send data - JDL
				brsmemset(ADR(SendData), 0, SIZEOF(SendData));
				brsmemcpy(ADR(SendData), ADR(gSendQueue.Queue[0]), SIZEOF(SendData));
				
				Client.sStep := 20;
			END_IF
		
		20: (* Send Data to the Server *)
			prevStep := Client.sStep;

					
			Client.TcpSend_0.enable := 1;				
			Client.TcpSend_0.ident := Client.TcpOpen_0.ident;  (* Connection Ident from AsTCP.TCP_Open *)
			Client.TcpSend_0.pData := ADR(SendData);  (* Which data to send *)
			Client.TcpSend_0.datalen := brsstrlen(ADR(SendData));  (* Lenght of data to send *)											
			Client.TcpSend_0.flags := 0;
			Client.TcpSend_0;  (* Call the Function*)
				
			IF Client.TcpSend_0.status = 0 THEN  (* Data was sent sucessfully -> receive data *)
				FifoPop(ADR(gSendQueue));
				Client.sStep := 30;
			ELSIF Client.TcpSend_0.status = ERR_FUB_BUSY THEN  (* TcpSend not finished -> redo *)	
				(* Busy *)
			ELSIF (Client.TcpSend_0.status = tcpERR_SENTLEN) OR (Client.TcpSend_0.status = tcpERR_NOT_CONNECTED) THEN (* Connection Lost *)
				Client.sStep := 40;
			ELSE  (* Goto Error Step *)
				Client.sStep := 100;
			END_IF				
					
		30: (* Receive Data from the Server *)
			prevStep := Client.sStep;
			Client.TcpRecv_0.enable := 1;
			Client.TcpRecv_0.ident := Client.TcpOpen_0.ident;  (* Connection Ident from AsTCP.TCP_Open *)
			Client.TcpRecv_0.pData	:= ADR(LibAsTCP1_ST.receive_data);  (* Where to store the incoming data *)
			Client.TcpRecv_0.datamax := SIZEOF(LibAsTCP1_ST.receive_data);  (* Lenght of data buffer *)
			Client.TcpRecv_0.flags := 0;
			Client.TcpRecv_0;  (* Call the Function*)
		
			IF Client.TcpRecv_0.status = 0 THEN  (* Data was received sucessfully -> Send next packet *)
				Client.sStep := 15;
				Client.recv_timeout := 0;
			ELSIF Client.TcpRecv_0.status = 0 THEN  (* Data was received sucessfully, but Message number not matched. Resend packet *)
				Client.sStep := 15;
				Client.recv_timeout := 0;
			ELSIF Client.TcpRecv_0.status = tcpERR_NO_DATA THEN  (* No data received - wait *)
				Client.recv_timeout := Client.recv_timeout + 1;
				IF Client.recv_timeout > 100 THEN //5s
				//	Client.sStep := 40;
					Client.sStep := 15;
					ReconnectWait := TRUE;
					Client.recv_timeout := 0;
				END_IF
				(* No Data received *)
			ELSIF Client.TcpRecv_0.status = ERR_FUB_BUSY THEN  (* TcpRecv not finished -> redo *)		
				(* Busy *)
			ELSIF Client.TcpRecv_0.status = tcpERR_NOT_CONNECTED THEN (* Connection Lost *)			
				Client.sStep := 40;
			ELSE  (* Goto Error Step *)
				Client.sStep := 100;
			END_IF
		
		40: (* Close connection *)
			prevStep := Client.sStep;
			Client.TcpClose_0.enable := 1;
			Client.TcpClose_0.ident := Client.TcpOpen_0.ident;  (* Connection Ident from AsTCP.TCP_Open *)
			Client.TcpClose_0.how := 0;
			Client.TcpClose_0;  (* Call the Function*)
	
			IF Client.TcpClose_0.status = 0 AND NOT ReconnectWait THEN  (* Close sucessfull -> Reopen the interface *)
				Client.sStep := 0;
			ELSIF Client.TcpClose_0.status = 0 AND ReconnectWait THEN  (* Close sucessfull -> Reopen the interface w/ 10s delay *)
				Client.sStep := 50;
				ReconnectWait := FALSE;
			ELSIF Client.TcpClose_0.status = ERR_FUB_BUSY THEN  (* TcpClose not finished -> redo *)	
				(* Busy *)
			ELSE  (* Goto Error Step *)
				Client.sStep := 100;
			END_IF
			
		50: (*Wait to reopen connection*)
			TON_Reopen.IN := TRUE;
			IF TON_Reopen.Q THEN
				Client.sStep := 0;
				TON_Reopen.IN := FALSE;
			END_IF
		
		100: (* Error State. Wait 10s, try again *)
			TON_Reopen.IN := TRUE;
			IF TON_Reopen.Q THEN
				Client.sStep := prevStep;
				TON_Reopen.IN := FALSE;
			END_IF
			
			
				
			
	END_CASE		
	
	TON_Reopen();


END_PROGRAM


