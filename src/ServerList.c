/***************************************************************************
 Bourriquet MorphOS
  2024-2025 digitally
***************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>

#include "Extrasrc.h"
#include "Server.h"
#include "ServerList.h"
#include "Debug.h"

/// InitServerList
// initialize a transfer list
// NOTE: the embedded semaphore must NOT be used for such a list
void InitServerList(struct ServerList *slist)
{
 	ENTER();
  	NewMinList(&slist->list);
  	slist->count = 0;
  	LEAVE();
}

///
/// ClearServerList
// remove all nodes from a transfer list
void ClearServerList(struct ServerList *slist)
{
 	struct ServerNode *snode;
  	struct ServerNode *succ;

  	ENTER();
  	SafeIterateList(&slist->list, struct ServerNode *, snode, succ)
  		{
    		DeleteServerNode(snode);
  		}
  	InitServerList(slist);
  	LEAVE();
}

///
/// CreateServerList
// create a new list for transfers
struct ServerList *CreateServerList(void)
{
 	struct ServerList *slist;

  	ENTER();
  	// at first create the list itself
  	if((slist = AllocSysObjectTags(ASOT_LIST, ASOLIST_Size, sizeof(*slist), ASOLIST_Min,  TRUE, TAG_DONE)) != NULL)
  		{
    		// now create the arbitration semaphore
    		if((slist->lockSemaphore = AllocSysObjectTags(ASOT_SEMAPHORE, TAG_DONE)) != NULL) slist->count = 0;
        	else
    			{
      				// free the list again on failure
      				FreeSysObject(ASOT_LIST, slist);
      				slist = NULL;
    			}
  		}
  	RETURN(slist);
  	return(slist);
}

///
/// DeleteServerList
// remove all nodes from a list and free it completely
void DeleteServerList(struct ServerList *slist)
{
 	ENTER();
  	if(slist != NULL)
  		{
    		struct ServerNode *snode;
    		struct ServerNode *next;

    		// lock the list just, just for safety reasons
    		LockServerList(slist);
    		// remove and free all remaining nodes in the list
    		SafeIterateList(&slist->list, struct ServerNode *, snode, next)
    			{
      				DeleteServerNode(snode);
    			}
    		NewMinList(&slist->list);
    		// unlock the list again
    		UnlockServerList(slist);
    		// free the semaphore
    		FreeSysObject(ASOT_SEMAPHORE, slist->lockSemaphore);
    		slist->lockSemaphore = NULL;
    		// free the list itself
    		FreeSysObject(ASOT_LIST, slist);
 		}
  	LEAVE();
}

///
/// ScanServerList
// iterate over a transfer list and return TRUE if at least one node has
// either one or all of the given flags set.
// Optionally the node's index can be returned
struct ServerNode *ScanServerList(const struct ServerList *slist, const ULONG maskFlags, const ULONG wantedFlags, const BOOL allFlags)
{
 	struct ServerNode *result = NULL;
  	struct ServerNode *snode;

  	ENTER();
  	LockServerList(slist);
  	ForEachServerNode(slist, snode)
  		{
    		// check if either at least one flag matches or if all flags match
    		if((allFlags == FALSE && isAnyFlagSet(snode->sflags, maskFlags)) || (allFlags == TRUE  && (snode->sflags & maskFlags) == wantedFlags))
    			{
    				result = snode;
    				break;
    			}
  		}
  	UnlockServerList(slist);
  	RETURN(result);
  	return result;
}

///
/// CreateServerNode
// create a new transfer node, a given Server pointer will be memdup()'ed
struct ServerNode *CreateServerNode(struct Server *server, const ULONG flags)
{
 	struct ServerNode *snode;

  	ENTER();
  	if((snode = AllocSysObjectTags(ASOT_NODE, ASONODE_Size, sizeof(*snode), ASONODE_Min, TRUE, TAG_DONE)) != NULL)
		{
    		// clear the structure, ASOT() does not do that for us
    		memset(snode, 0, sizeof(*snode));
    		snode->sflags = flags;
    		snode->server = server;
			if(server != NULL) server->RefCounter++;
  		}
  	RETURN(snode);
  	return(snode);
}

///
/// AddServerNode
// add a transfer node to an existing list
// if locking of the list is needed this must be done by the calling function
void AddServerNode(struct ServerList *slist, struct ServerNode *snode)
{
	ENTER();
  	if(slist != NULL && snode != NULL && snode->server != NULL)
  		{
    		AddTail((struct List *)&slist->list, (struct Node *)&snode->node);
    		// and increase the counter
    		slist->count++;
  		}
  	LEAVE();
}

///
/// DeleteServerNode
// free a transfer node that does not belong to a list
void DeleteServerNode(struct ServerNode *snode)
{
 	ENTER();
  	if(snode->server != NULL) 
  		{
			snode->server->RefCounter--;
    		free(snode->server);
  		}
  	FreeSysObject(ASOT_NODE, snode);
  	LEAVE();
}

///
/// CountServerNodes
// count the number of nodes with specific flags set
ULONG CountServerNodes(const struct ServerList *slist, const ULONG flags)
{
	ULONG count = 0;
  	struct ServerNode *snode;

  	ENTER();
  	LockServerList(slist);
  	ForEachServerNode(slist, snode)
  		{
    		if(isAnyFlagSet(snode->sflags, flags)) count++;
  		}
  	UnlockServerList(slist);
  	RETURN(count);
  	return(count);
}
