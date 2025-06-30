/***************************************************************************
Bourriquet MorphOS
 Copyright (C) 2022 digitally
***************************************************************************/
#include <proto/exec.h>
#include <proto/timer.h>
#include <mui/NList_mcc.h>
#include "Extrasrc.h"
#include "timeval.h"
#include "Bourriquet.h"

//#include "Config.h"
#include "Locale.h"
#include "MUIObjects.h"
#include "Threads.h"
#include "Timer.h"

#include "mui/ClassesExtra.h"
#include "mui/BourriquetApplication.h"

#include "Debug.h"

/*** Timer processing function ***/
/// PrepareTRequest
// prepare a single time request
static void PrepareTRequest(struct TRequest *timer, const ULONG seconds, const ULONG micros, BOOL absoluteTime)
{
 	ENTER();
    if(seconds > 0 || micros > 0)
    	{
        	if(timer->isRunning == FALSE && timer->isPrepared == FALSE)
          		{
           	 		timer->tr->Request.io_Command  = TR_ADDREQUEST;
            		timer->tr->Time.Seconds        = seconds;
            		timer->tr->Time.Microseconds   = micros;
            		timer->remainingTime.Seconds = seconds;
            		timer->remainingTime.Microseconds = micros;
            		timer->isPrepared = TRUE;
            		timer->isAbsolute = absoluteTime;
          		}
    	}
    LEAVE();
}

void PrepareTimer(const enum Timer tid, const ULONG seconds, const ULONG micros, BOOL absoluteTime)
{
    ENTER();
    PrepareTRequest(&G->timerData.timer[tid], seconds, micros, absoluteTime);
    LEAVE();
}

static void StartTRequest(struct TRequest *timer)
{
    ENTER();
    if(timer->tr != NULL)
    	{
        	if(timer->isRunning == FALSE && timer->isPrepared == TRUE)
          		{
#if defined(DEBUG)
	  				char dateStrStarted[64];
	  				char dateStrFinish[64];
	  				struct TimeVal tvTmp;
	  				struct TimeVal tvTimer;
#endif
            		if(timer->isAbsolute == TRUE)
              			{
                			struct TimeVal tsNow;

                			GetSysTime(TIMEVAL(&tsNow));
                			SubTime(TIMEVAL(&timer->tr->Time), TIMEVAL(&tsNow));
              			}
    				// fire the timer by doing a SendIO()
        			SendIO(&timer->tr->Request);
      				// remember the timer start time
      	  			GetSysTime(TIMEVAL(&timer->startTime));
     				// signal that our timer is running
        			timer->isRunning = TRUE;
        			timer->isPrepared = FALSE;
     			}
    	}
   LEAVE();
}

///
/// StartTimer
//  Start a delay depending on the time specified
void StartTimer(const enum Timer tid)
{
    ENTER();
    StartTRequest(&G->timerData.timer[tid]);
    LEAVE();
}

///
/// StopTRequest
// stop a time request
static void StopTRequest(struct TRequest *timer)
{
    ENTER();
  	// check if we have a already issued ioreq running
    if(timer->isRunning == TRUE)
      	{
        	struct IORequest *ioreq = &timer->tr->Request;

        	if(ioreq->io_Command != 0)
          		{
            		if(CheckIO(ioreq) == NULL) AbortIO(ioreq);
            		WaitIO(ioreq);
      				// there is no time left for this request
            		timer->remainingTime.Seconds = 0;
            		timer->remainingTime.Microseconds = 0;
      				// make sure the timer is signalled to be NOT running
            		timer->isRunning = FALSE;
            		timer->isPrepared = FALSE;
              	}
      	}
    LEAVE();
}

///
/// StopTimer
//  Stop a currently running TimerIO request
//  Please note that this function may NOT be used in the eventloop after having received
//  a timer with GetMsg because CheckIO and friends are not defined to work there correctly.
void StopTimer(const enum Timer tid)
{
    ENTER();
    StopTRequest(&G->timerData.timer[tid]);
    LEAVE();
}

///
/// PauseTimer
//  Pause a currently running Time request
void PauseTimer(const enum Timer tid)
{
    struct TRequest *timer = &G->timerData.timer[tid];

    ENTER();
  	// check if we have a already issued ioreq running
    if(timer->isRunning == TRUE && timer->isPaused == FALSE)
      	{
        	struct IORequest *ioreq = &timer->tr->Request;

        	if(ioreq->io_Command != 0)
          		{
            		struct TimeVal stopTime;

            		if(CheckIO(ioreq) == NULL) AbortIO(ioreq);
            		WaitIO(ioreq);
      				// calculate the remaining time
            		GetSysTime(TIMEVAL(&stopTime));
            		SubTime(TIMEVAL(&stopTime), TIMEVAL(&timer->startTime));
            		SubTime(TIMEVAL(&timer->remainingTime), TIMEVAL(&stopTime));
      				// make sure the timer is signalled to be NOT running
            		timer->isRunning = FALSE;
            		timer->isPaused = TRUE;
          		}
        	else E(DBF_TIMER, "timer[%ld]: est invalide et ne peut pas être en pause", tid);
      	}
    else W(DBF_TIMER, "timer[%ld]: déjà en pause", tid);
    LEAVE();
}

///
/// ResumeTimer
//  Resume a timer with the remaining time
void ResumeTimer(const enum Timer tid)
{
    struct TRequest *timer = &G->timerData.timer[tid];

    ENTER();
    if(timer->isRunning == FALSE && timer->isPaused == TRUE)
      	{
        	struct TimeRequest *timeReq = timer->tr;

    		// issue a new timerequest with the previously calculated remaining time
        	timeReq->Request.io_Command  = TR_ADDREQUEST;
        	timeReq->Time.Seconds = timer->remainingTime.Seconds;
        	timeReq->Time.Microseconds = timer->remainingTime.Microseconds;
    		// fire the timer by doing a SendIO()
        	SendIO((struct IORequest *)timeReq);
    		// remember the new start time
        	GetSysTime(TIMEVAL(&timer->startTime));
    		// signal that our timer is running
        	timer->isRunning = TRUE;
        	timer->isPaused = FALSE;
      	}
    else W(DBF_TIMER, "timer[%ld]: déjà en fonctionnement ou non en pause", tid);
    LEAVE();
}

///
/// RestartTimer
//  restarts a particular timer. In fact it makes sure that the timer in question
//  is first stopped via AbortIO() and then issues a new one. Please note that
//  this function may NOT be called from the eventloop because CheckIO and friends
//  are not defined to work there.
void RestartTimer(const enum Timer tid, const ULONG seconds, const ULONG micros, BOOL absoluteTime)
{
    ENTER();
    StopTimer(tid);
    PrepareTimer(tid, seconds, micros, absoluteTime);
    StartTimer(tid);
  LEAVE();
}
///

/*** Timer management functions ***/
/// CreateTRequest
// create a new time request, will be cloned from the first global request
BOOL CreateTRequest(struct TRequest *timer, UNUSED const int id)
{
    BOOL success = FALSE;

    ENTER();
    if(G->timerData.timer[0].tr != NULL)
      	{
    		// clone the first global request
        	timer->tr = AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_Size, sizeof(struct TimeRequest), ASOIOR_Duplicate, (IPTR)G->timerData.timer[0].tr, TAG_DONE);
      	}
    else
      	{
    		// create a new request
        	timer->tr = AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_Size, sizeof(struct TimeRequest), ASOIOR_ReplyPort, (IPTR)G->timerData.port, TAG_DONE);
      	}
    if(timer->tr != NULL)
      	{
        	timer->isRunning = FALSE;
        	timer->isPrepared = FALSE;
        	success = TRUE;
      }
    RETURN(success);
    return(success);
}

///
/// DeleteTRequest
// delete a cloned time request
void DeleteTRequest(struct TRequest *timer)
{
    ENTER();
    if(timer->tr != NULL)
      	{
        	StopTRequest(timer);
        	FreeSysObject(ASOT_IOREQUEST, timer->tr);
        	timer->tr = NULL;
      	}
    LEAVE();
}

///
/// InitTimers
//  Initializes timer resources
BOOL InitTimers(void)
{
    BOOL result = FALSE;

    ENTER();
  	// create message port
    if((G->timerData.port = AllocSysObjectTags(ASOT_PORT, TAG_DONE)) != NULL)
      	{
    		// create the TimerIOs now
    		// we use AllocSysObjectTags to give the OS a better chance to
    		// free the data in case YAM crashes (only available on OS4)
        	if(CreateTRequest(&G->timerData.timer[0], 0) == TRUE)
          		{
      				// then open the device
            		if(OpenDevice(TIMERNAME, UNIT_MICROHZ, &G->timerData.timer[0].tr->Request, 0L) == 0)
              			{
        					// needed to get GetSysTime() working
                			if((TimerBase = (APTR)G->timerData.timer[0].tr->Request.io_Device)) 
                  				{
                    				enum Timer tid;

          							// create our other TimerIOs now
                    				for(tid = TIMER_WRINDEX + 1; tid < TIMER_NUM; tid++)
                      					{
                        					if(CreateTRequest(&G->timerData.timer[tid], tid) == FALSE) break;
                      					}
                  				}
              			}
          		}
      	}
    result = (BOOL)(G->timerData.timer[TIMER_NUM - 1].tr != NULL);
    RETURN(result);
    return(result);
}

///
/// CleanupTimers
//  Frees timer resources
void CleanupTimers(void)
{
    ENTER();
  	// first we abort & delete the IORequests
    if(G->timerData.timer[0].tr != NULL)
      	{
        	enum Timer tid;

    		// first make sure every TimerIO is stoppped
        	for(tid = TIMER_WRINDEX; tid < TIMER_NUM; tid++) StopTimer(tid);
    		// then close the device
        	if(G->timerData.timer[0].tr->Request.io_Device != NULL)
          		{
     				// forget the TimerBase to avoid crashes when quitting YAM
            		TimerBase = NULL;
      				// drop the OS4 Interface of the TimerBase
            		//DROPINTERFACE(ITimer);
            		CloseDevice(&G->timerData.timer[0].tr->Request);
          		}
    		// and then we delete the IO requests
        	for(tid = TIMER_WRINDEX + 1; tid < TIMER_NUM; tid++) DeleteTRequest(&G->timerData.timer[tid]);
        	DeleteTRequest(&G->timerData.timer[0]);
      	}
	// remove the MsgPort now.
    if(G->timerData.port != NULL)
      	{
        	FreeSysObject(ASOT_PORT, G->timerData.port);
        	G->timerData.port = NULL;
      	}
    LEAVE();
}

///
/// TimerDispatcher
//  Dispatcher for timer class
//  WARNING: Do NOT use StartTimer() directly in this function as it is
//  called within the timer eventloop which is undefined!
//  Do a PrepareTimer() instead here and a StartTimer() in the
//  the parent eventloop at the end of the file here.
static void TimerDispatcher(const enum Timer tid)
{
  	// prepare some debug information
	ENTER();
  	// now dispatch between the differnent timerIOs
    /*switch(tid)
      {
	case TIMER_NUM:
           break;
      }*/
    LEAVE();
}

///
/// ProcessTimerEvent
// function to check the status of all our timers via the msgport
// process on them and then restart those fired accordingly.
BOOL ProcessTimerEvent(void)
{
    BOOL processedAny = FALSE;
    struct TimeRequest *timeReq;

    ENTER();
  	// check if we have a waiting message
    while((timeReq = (struct TimeRequest *)GetMsg(G->timerData.port)) != NULL)
      	{
        	BOOL processedThis = FALSE;
        	enum Timer tid;

        	D(DBF_TIMER, "gestion de l'evenement timer %08lx", timeReq);
        	for(tid=0; tid < TIMER_NUM; tid++)
          		{
            		struct TRequest *timer = &G->timerData.timer[tid];

            		D(DBF_TIMER, "verification de l'evenement timer  %08lx vs %08lx tid %ld", timeReq, timer->tr, tid);
            		if(timeReq == timer->tr)
              			{
        					// set the timer to be not running and not be prepared for
        					// another shot. Our dispatcher have to do the rest then
                			timer->isRunning = FALSE;
                			timer->isPrepared = FALSE;
        					// call the dispatcher with signalling which timer
        					// this request caused
                			TimerDispatcher(tid);
        					// signal that we processed something
                			processedAny = TRUE;
                			processedThis = TRUE;
        					// break out of the for() loop
                			break;
              			}
         	 	}
    		// no ReplyMsg() needed
      	}
  	// make sure that we are starting the timer again after the GetMsg loop
    if(processedAny == TRUE)
      	{
    		// here we just check for the timers that TimerDispatcher really
    		// prepares and not all of them in a loop
        	if(G->timerData.timer[TIMER_AUTOSAVE].isPrepared == TRUE) StartTimer(TIMER_AUTOSAVE);
      	}
    else W(DBF_TIMER, "l'événement timer reçu mais aucun timer n'a été traité !!!");
    RETURN(processedAny);
    return(processedAny);
}
