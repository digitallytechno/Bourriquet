/***************************************************************************
Bourriquet MorphOS
	2024-2025 Digitally
 
***************************************************************************/
#ifndef SERVER_H
#define SERVER_H

#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/types.h>
#include <proto/exec.h>

#include "Lists.h"
#include "Stringsizes.h"

//struct SignalSemaphore;
//struct Server;

struct Server
{
	struct node node;
	short RefCounter; 
  	Object *imageObject;
  	int status;
    char IP[SIZE_DEFAULT];
    int Port;
    char Name[SIZE_DEFAULT];
    char Description[SIZE_DEFAULT];
    int Ping;
    ULONG Users;
    ULONG Files;
    int Prefs;
    BOOL Failed;
    BOOL Static;
    ULONG maxusers;
    ULONG Offerfiles;
    ULONG lowidusers;
    ULONG hardfiles;
    ULONG softfiles;
    char Version[SIZE_DEFAULT];
    ULONG lowlimit_users;
    ULONG maxlimit_users;
	int Type;
};

#endif /* SERVER_H */
