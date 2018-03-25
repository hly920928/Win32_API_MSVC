#pragma once
#include "stdafx.h"
#include "Everything.h" 
#define MAX_RQRS_LEN 0x1000
#define MAX_MESSAGE_LEN MAX_RQRS_LEN
struct MESSAGE {
	LONG32 msgLen;
	BYTE record[MAX_RQRS_LEN];
};
 struct REQUEST{	
	LONG32 rqLen;	
	BYTE record[MAX_RQRS_LEN];
 };
struct RESPONSE {
	 LONG32 rsLen;
	 BYTE record[MAX_RQRS_LEN];
  };
#define RQ_SIZE sizeof (REQUEST)
#define RQ_HEADER_LEN RQ_SIZE-MAX_RQRS_LEN
#define RS_SIZE sizeof (RESPONSE)
#define RS_HEADER_LEN RS_SIZE-MAX_RQRS_LEN
 struct MS_MESSAGE {
	DWORD32 msStatus;
	DWORD32 msUtilization;
	CHAR msName[MAX_PATH];
};
#define MSM_SIZE sizeof (MS_MESSAGE)
#define CS_TIMEOUT 5000

#define MAX_CLIENTS  4 
#define MAX_SERVER_TH 4
#define MAX_CLIENTS_CP 128 

#define SERVER_PIPE "\\\\.\\PIPE\\SERVER"
#define CLIENT_PIPE "\\\\.\\PIPE\\SERVER"
#define MS_SRVNAME "\\\\.\\MAILSLOT\\CLS_MAILSLOT"
#define MS_CLTNAME "\\\\.\\MAILSLOT\\CLS_MAILSLOT"

#define MX_NAME  "ClientServerMutex"
#define SM_NAME  "ClientServerSemaphore"

#define SERVER_BROADCAST "SrvrBcst.exe"

#define CS_INIT			1
#define CS_RQSTART		2
#define CS_RQCOMPLETE	3
#define CS_REPORT		4
#define CS_TERMTHD		5

 BOOL LocateServer(LPTSTR, DWORD);

#define SERVER_PORT 50000 