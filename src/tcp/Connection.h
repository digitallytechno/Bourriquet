/***************************************************************************
 Bourriquet MorphOS
 2024-2025 digitally
 
***************************************************************************/
#ifndef CONNECTION_H
#define CONNECTION_H 1

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <time.h>
#include "ServerList.h"

#include "Stringsizes.h"

#define CS_FATALERROR	-5
#define CS_DISCONNECTED	-4
#define CS_SERVERDEAD	-3
#define	CS_ERROR		-2
#define CS_SERVERFULL	-1
#define	CS_NOTCONNECTED	0
#define	CS_CONNECTING	1
#define	CS_CONNECTED	2
#define	CS_WAITFORLOGIN	3
#define CS_WAITFORPROXYLISTENING 4 
#define CS_RETRYCONNECTTIME  30 

#define CONNECTION_NULL   0
#define CONNECTION_GET_IP 1
#define CONNECTION_GET_SOCKFD 2
#define CONNECTION_GET_CONNECTED 3
#define CONNECTION_IN_HANDLE    4
#define CONNECTION_ACTIVE 5
#define CONNECTION_ASK_FILE_STATUS 6
#define CONNECTION_SLOT_ASKED 7
#define CONNECTION_SLOT_OPEN 8
#define CONNECTION_UPLOAD_STARTED 9
#define MAX_RETRY 9
#define MAX_HOSTS 4096
#define TIMEOUT 3
#define PARTSIZE 9728000
#define MAXBLOCKSIZE 184320
#define MAXPARTSEND 10248

#endif /* CONNECTION_H */
