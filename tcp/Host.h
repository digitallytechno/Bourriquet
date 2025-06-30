/***************************************************************************
Bourriquet MorphOS
	2024-2025 Digitally
 
***************************************************************************/
#ifndef HOST_H
#define HOST_H

#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/types.h>
#include <proto/exec.h>

#include "Lists.h"
#include "Stringsizes.h"

struct Host
{
	struct node node;
	char *ip;
   	char *nom;
   	int port;
   	int serverIP;
   	int serverPort;
   	int state;
   	int retry;
   	int hosttype;
   	int type;
   	int connection;
   	long sockfd;
   	int queueRank;
   	unsigned long lastCmdTime;
   	int nbPartsRecv;			  
   	struct sockaddr *sa;
   	long gapStart[SIZE_DEFAULT];
   	long gapEnd[SIZE_DEFAULT];
   	char ID[SIZE_DEFAULT];
   	int nbGaps;
	int alreadytrytoconnect;
};

#endif /* HOST_H */
