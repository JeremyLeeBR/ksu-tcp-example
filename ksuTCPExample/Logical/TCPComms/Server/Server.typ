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
	Server_typ : 	STRUCT  (*TCP Server Variables*)
		sStep : UINT; (*TCP Server Step Variable*)
		client_address : STRING[20]; (*Address of the client connection to the Server*)
		TcpOpen_0 : TcpOpen; (*AsTCP.TcpOpen FUB*)
		TcpServer_0 : TcpServer; (*AsTCP.TcpServer FUB*)
		TcpRecv_0 : TcpRecv; (*AsTCP.TcpRecv FUB*)
		TcpSend_0 : TcpSend; (*AsTCP.TcpSend FUB*)
		TcpClose_0 : TcpClose; (*AsTCP.TcpClose FUB*)
		TcpIoctl_0 : TcpIoctl; (*AsTCP.TcpIoctl*)
		data_buffer : ARRAY[0..100] OF UDINT; (*Data buffer ( receive and send )*)
		linger_opt : tcpLINGER_typ; (*AsTCP.tcpLINGER_typ*)
		recv_timeout : UDINT; (*receive timeout*)
	END_STRUCT;
END_TYPE
