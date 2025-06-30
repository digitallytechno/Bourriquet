/***************************************************************************
 Bourriquet 
 	digitally
 ***************************************************************************/
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#include "Bourriquet.h"
#include "Stringsizes.h"
#include "Extrasrc.h"
#include "mui/SplashWindow.h"
#include "mui/BourriquetApplication.h"
#include "Busy.h"
#include "MethodStack.h"
#include "Threads.h"
#include "Debug.h"

/// BusyBegin
// Démarrer une action d'attente
struct BusyNode *BusyBegin(ULONG type)
{
    struct BusyNode *busy = NULL;

    ENTER();
	W(DBF_ALWAYS, "BusyBegin");
    if(IsMainThread() == TRUE)
      	{
			W(DBF_ALWAYS, "ismainthread()");
        	if((busy = AllocSysObjectTags(ASOT_NODE, ASONODE_Size, sizeof(*busy), ASONODE_Min, TRUE, TAG_DONE)) != NULL)
          		{
           	 		memset(busy, 0, sizeof(*busy));
            		busy->type = type;
            		busy->progressCurrent = (type == BUSY_PROGRESS_ABORT) ? 0 : -1;
            		busy->progressMax = 0;
            		AddTail((struct List *)&G->normalBusyList, (struct Node *)busy);
          		}
      	}
	else
		{
			W(DBF_ALWAYS, "not ismainthread()", busy->type);
            busy = (struct BusyNode *)PushMethodOnStackWait(G->App, 2, MUIM_BourriquetApplication_BusyBegin, type);
		}
    RETURN(busy);
    return(busy);
}

static BOOL BusyShow(const struct BusyNode *busy)
{
    BOOL goOn = TRUE;

    ENTER();
    if(G->InStartupPhase == TRUE) DoMethod(G->SplashWinObject, MUIM_SplashWindow_ProgressChange, busy);
    //else if(G->MA != NULL) goOn = DoMethod(G->MA->GUI.IB_INFOBAR, MUIM_InfoBar_ShowBusyBar, busy);
    RETURN(goOn);
    return(goOn);
}

void BusyText(struct BusyNode *busy, const char *text, const char *param)
{
    ENTER();

    if(busy != NULL)
      	{
        	if(IsMainThread() == TRUE)
          		{
            		snprintf(busy->infoText, sizeof(busy->infoText), text, param);
            		BusyShow(busy);
          		}
			else PushMethodOnStack(G->App, 4, MUIM_BourriquetApplication_BusyText, busy, text, param);
      	}
    LEAVE();
}

BOOL BusyProgress(struct BusyNode *busy, int progress, int max)
{
    BOOL goOn = TRUE;

    ENTER();
    if(busy != NULL)
      	{
        	if(busy->type == BUSY_PROGRESS || busy->type == BUSY_PROGRESS_ABORT)
          		{
            		if(IsMainThread() == TRUE)
              			{
                			busy->progressCurrent = (busy->type == BUSY_PROGRESS_ABORT) ? progress : -1;
                			busy->progressMax = max;
                			goOn = BusyShow(busy);
              			}
					else
              			{
							if(busy->type == BUSY_PROGRESS_ABORT) goOn = PushMethodOnStackWait(G->App, 3, MUIM_BourriquetApplication_BusyProgress, progress, max);
							else PushMethodOnStack(G->App, 4, MUIM_BourriquetApplication_BusyProgress, busy, progress, max);
			  			}
          		}
        	else W(DBF_ALWAYS, "mauvais appel de fonction pour le type %ld Busy", busy->type);
      	}
    RETURN(goOn);
    return(goOn);
}

void BusyEnd(struct BusyNode *busy)
{
    ENTER();
    if(busy != NULL)
      	{
       	 	if(IsMainThread() == TRUE)
          		{
            		struct BusyNode *last = NULL;

            		Remove((struct Node *)busy);
					if(last == NULL) last = (struct BusyNode *)GetTail((struct List *)&G->normalBusyList);
            		BusyShow(last);
            		FreeSysObject(ASOT_NODE, busy);
          		}
			else PushMethodOnStack(G->App, 2, MUIM_BourriquetApplication_BusyEnd, busy);
      	}
    LEAVE();
}

void BusyCleanup(void)
{
    struct BusyNode *busy;
    struct BusyNode *next;

    ENTER();
	SafeIterateList(&G->normalBusyList, struct BusyNode *, busy, next)
    	{
        	W(DBF_STARTUP, " liberation en attendant une action busy normale %08lx, type %ld, text '%s'", busy, busy->type, busy->infoText);
        	FreeSysObject(ASOT_NODE, busy);
	  	}
	NewMinList(&G->normalBusyList);
    LEAVE();
}

///
