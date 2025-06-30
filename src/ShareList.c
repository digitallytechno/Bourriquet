/***************************************************************************
 Bourriquet MorphOS
 Copyright (C) 2024-2025 digitally
***************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>

#include "Extrasrc.h"
#include "Share.h"
#include "ShareList.h"
#include "Debug.h"

/// InitShareList
// initialize a transfer list
// NOTE: the embedded semaphore must NOT be used for such a list
void InitShareList(struct ShareList *shlist)
{
 	ENTER();
  	NewMinList(&shlist->list);
  	shlist->count = 0;
  	LEAVE();
}

///
/// ClearShareList
// remove all nodes from a transfer list
void ClearShareList(struct ShareList *shlist)
{
 	struct ShareNode *shnode;
  	struct ShareNode *succ;

  	ENTER();
  	SafeIterateShareList(&shlist->list, struct ShareNode *, shnode, succ)
  		{
    		DeleteShareNode(shnode);
  		}
  	InitShareList(shlist);
  	LEAVE();
}

///
/// CreateShareList
// create a new list for transfers
struct ShareList *CreateShareList(void)
{
 	struct ShareList *shlist;

  	ENTER();
  	// at first create the list itself
  	if((shlist = AllocSysObjectTags(ASOT_LIST, ASOLIST_Size, sizeof(*shlist), ASOLIST_Min,  TRUE, TAG_DONE)) != NULL)
  		{
    		// now create the arbitration semaphore
    		if((shlist->lockSemaphore = AllocSysObjectTags(ASOT_SEMAPHORE, TAG_DONE)) != NULL) shlist->count = 0;
        	else
    			{
      				// free the list again on failure
      				FreeSysObject(ASOT_LIST, shlist);
      				shlist = NULL;
    			}
  		}
  	RETURN(shlist);
  	return(shlist);
}

///
/// DeleteShareList
// remove all nodes from a list and free it completely
void DeleteShareList(struct ShareList *shlist)
{
 	ENTER();
  	if(shlist != NULL)
  		{
    		struct ShareNode *shnode;
    		struct ShareNode *next;

    		// lock the list just, just for safety reasons
    		LockShareList(shlist);
    		// remove and free all remaining nodes in the list
    		SafeIterateShareList(&shlist->list, struct ShareNode *, shnode, next)
    			{
      				DeleteShareNode(shnode);
    			}
    		NewMinList(&shlist->list);
    		// unlock the list again
    		UnlockShareList(shlist);
    		// free the semaphore
    		FreeSysObject(ASOT_SEMAPHORE, shlist->lockSemaphore);
    		shlist->lockSemaphore = NULL;
    		// free the list itself
    		FreeSysObject(ASOT_LIST, shlist);
 		}
  	LEAVE();
}

///
/// ScanShareList
// iterate over a transfer list and return TRUE if at least one node has
// either one or all of the given flags set.
// Optionally the node's index can be returned
struct ShareNode *ScanShareList(const struct ShareList *shlist, const ULONG maskFlags, const ULONG wantedFlags, const BOOL allFlags)
{
 	struct ShareNode *result = NULL;
  	struct ShareNode *shnode;

  	ENTER();
  	LockShareList(shlist);
  	ForEachShareNode(shlist, shnode)
  		{
    		// check if either at least one flag matches or if all flags match
    		if((allFlags == FALSE && isAnyFlagSet(shnode->sflags, maskFlags)) || (allFlags == TRUE  && (shnode->sflags & maskFlags) == wantedFlags))
    			{
    				result = shnode;
    				break;
    			}
  		}
  	UnlockShareList(shlist);
  	RETURN(result);
  	return result;
}

///
/// CreateShareNode
// create a new transfer node, a given Share pointer will be memdup()'ed
struct ShareNode *CreateShareNode(struct Share *share, const ULONG flags)
{
 	struct ShareNode *shnode;

  	ENTER();
  	if((shnode = AllocSysObjectTags(ASOT_NODE, ASONODE_Size, sizeof(*shnode), ASONODE_Min, TRUE, TAG_DONE)) != NULL)
		{
    		// clear the structure, ASOT() does not do that for us
    		memset(shnode, 0, sizeof(*shnode));
    		shnode->sflags = flags;
    		shnode->share = share;
  		}
  	RETURN(shnode);
  	return(shnode);
}

///
/// AddShareNode
// add a transfer node to an existing list
// if locking of the list is needed this must be done by the calling function
void AddShareNode(struct ShareList *shlist, struct ShareNode *shnode)
{
	ENTER();
  	if(shlist != NULL && shnode != NULL && shnode->share != NULL)
  		{
    		AddTail((struct List *)&shlist->list, (struct Node *)&shnode->node);
    		// and increase the counter
    		shlist->count++;
  		}
  	LEAVE();
}

///
/// DeleteShareNode
// free a transfer node that does not belong to a list
void DeleteShareNode(struct ShareNode *shnode)
{
 	ENTER();
  	if(shnode->share != NULL) 
  		{
    		free(shnode->share);
  		}
  	FreeSysObject(ASOT_NODE, shnode);
  	LEAVE();
}

///
/// CountShareNodes
// count the number of nodes with specific flags set
ULONG CountShareNodes(const struct ShareList *shlist, const ULONG flags)
{
	ULONG count = 0;
  	struct ShareNode *shnode;

  	ENTER();
  	LockShareList(shlist);
  	ForEachShareNode(shlist, shnode)
  		{
    		if(isAnyFlagSet(shnode->sflags, flags)) count++;
  		}
  	UnlockShareList(shlist);
  	RETURN(count);
  	return(count);
}
