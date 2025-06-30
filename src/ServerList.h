/***************************************************************************
 Bourriquet MorphOS
 2024-2025 digitally
 
***************************************************************************/
#ifndef SERVERLIST_H
#define SERVERLIST_H 1

#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/types.h>
#include <proto/exec.h>

#include "Lists.h"

// forward declarations
struct SignalSemaphore;
struct Server;
struct ServerList;

struct ServerList
{
  	struct MinList list;
  	struct SignalSemaphore *lockSemaphore;
  	ULONG count;
};

struct ServerNode
{
  	struct MinNode node;      
  	struct Server *server;        
  	ULONG sflags;            
  	int position;            
  	int index;          
};

void InitServerList(struct ServerList *slist);
void ClearServerList(struct ServerList *slist);
struct ServerList *CreateServerList(void);
void DeleteServerList(struct ServerList *slist);
struct ServerNode *ScanServerList(const struct ServerList *slist, const ULONG maskFlags, const ULONG wantedFlags, const BOOL allFlags);
struct ServerNode *CreateServerNode(struct Server *server, const ULONG flags);
void AddServerNode(struct ServerList *slist, struct ServerNode *snode);
void DeleteServerNode(struct ServerNode *snode);
ULONG CountServerNodes(const struct ServerList *slist, const ULONG flags);

#define IterateList(list, type, node)           for((node) = (type)GetHead((struct List *)(list)); (node) != NULL; (node) = (type)GetSucc((struct Node *)(node)))
#define SafeIterateList(list, type, node, succ) for((node) = (type)GetHead((struct List *)(list)); (node) != NULL && (((succ) = (type)GetSucc((struct Node *)(node))) != NULL || (succ) == NULL); (node) = (succ))

#if !defined(IsMinListEmpty)
#define IsMinListEmpty(x) \
    ( (struct MinList *)((x)->mlh_TailPred) == (struct MinList *)(x) )
#endif

// check if a server list is empty
#define IsServerListEmpty(slist)                    IsMinListEmpty(&(slist)->list)
#define isAnyFlagSet(v,f)     (((v) & (f)) != 0)    // return TRUE if one of the flags in f is set in v

// iterate through the list, the list must *NOT* be modified!
#define ForEachServerNode(slist, snode)             for(snode = FirstServerNode(slist); snode != NULL; snode = NextServerNode(snode))

// navigate in the list
#define FirstServerNode(slist)                      (struct ServerNode *)GetHead((struct List *)&((slist)->list))
#define LastServerNode(slist)                       (struct ServerNode *)GetTail((struct List *)&((slist)->list))
#define NextServerNode(snode)                       (struct ServerNode *)GetSucc((struct Node *)snode)
#define PreviousServerNode(snode)                   (struct ServerNode *)GetPred((struct Node *)snode)

// lock and unlock a server list via its semaphore
#define LockServerList(slist)                       ObtainSemaphore((slist)->lockSemaphore)
#define LockServerListShared(slist)                 ObtainSemaphoreShared((slist)->lockSemaphore)
#define UnlockServerList(slist)                     ReleaseSemaphore((slist)->lockSemaphore)

#endif /* SERVERLIST */
