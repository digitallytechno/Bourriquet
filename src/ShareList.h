/***************************************************************************
 Bourriquet MorphOS
 2024-2025 digitally
 
***************************************************************************/
#ifndef SHARELIST_H
#define SHARELIST_H 1

#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/types.h>
#include <proto/exec.h>

// forward declarations
struct SignalSemaphore;
struct Share;

struct ShareList
{
  struct MinList list;
  struct SignalSemaphore *lockSemaphore;
  ULONG count;
};

struct ShareNode
{
  struct MinNode node;      
  struct Share *share;      
  ULONG sflags;         
  int position;       
  int index;    
};

void InitShareList(struct ShareList *shlist);
void ClearShareList(struct ShareList *shlist);
struct ShareList *CreateShareList(void);
void DeleteShareList(struct ShareList *shlist);
struct ShareNode *ScanShareList(const struct ShareList *shlist, const ULONG maskFlags, const ULONG wantedFlags, const BOOL allFlags);
struct ShareNode *CreateShareNode(struct Share *Share, const ULONG flags);
void AddShareNode(struct ShareList *shlist, struct ShareNode *shnode);
void DeleteShareNode(struct ShareNode *shnode);
ULONG CountShareNodes(const struct ShareList *shlist, const ULONG flags);

#define IterateShareList(list, type, node)           for((node) = (type)GetHead((struct List *)(list)); (node) != NULL; (node) = (type)GetSucc((struct Node *)(node)))
#define SafeIterateShareList(list, type, node, succ) for((node) = (type)GetHead((struct List *)(list)); (node) != NULL && (((succ) = (type)GetSucc((struct Node *)(node))) != NULL || (succ) == NULL); (node) = (succ))

#if !defined(IsMinListEmpty)
#define IsMinListEmpty(x) \
    ( (struct MinList *)((x)->mlh_TailPred) == (struct MinList *)(x) )
#endif

// check if a Share list is empty
#define IsShareListEmpty(shlist) 	IsMinListEmpty(&(shlist)->list)
#define isAnyFlagSet(v,f) 	(((v) & (f)) != 0)    

// iterate through the list, the list must *NOT* be modified!
#define ForEachShareNode(shlist, shnode)             for(shnode = FirstShareNode(shlist); shnode != NULL; shnode = NextShareNode(shnode))

// navigate in the list
#define FirstShareNode(shlist)                      (struct ShareNode *)GetHead((struct List *)&((shlist)->list))
#define LastShareNode(shlist)                       (struct ShareNode *)GetTail((struct List *)&((shlist)->list))
#define NextShareNode(shnode)                       (struct ShareNode *)GetSucc((struct Node *)shnode)
#define PreviousShareNode(shnode)                   (struct ShareNode *)GetPred((struct Node *)shnode)

// lock and unlock a Share list via its semaphore
#define LockShareList(shlist)                       ObtainSemaphore((shlist)->lockSemaphore)
#define LockShareListShared(shlist)                 ObtainSemaphoreShared((shlist)->lockSemaphore)
#define UnlockShareList(shlist)                     ReleaseSemaphore((shlist)->lockSemaphore)

#endif /* SHARELIST */
