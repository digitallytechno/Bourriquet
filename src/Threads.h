/***************************************************************************
Bourriquet MorphOS
 Copyright (C) 2024-2025 digitally
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
***************************************************************************/
#ifndef THREADS_H
#define THREADS_H

#include "SDI_compiler.h"

#define TAG_STRING (1 << 30)

enum ThreadAction
{ 
 	TA_Startup,
    TA_Shutdown,
    TA_StartSingleConnection,
	TA_StartMultiConnection,
	 TA_EndConnection
 };

#define TT_Priority                                0xf001 // priorité du thread
#define TT_Connection_OneServer                 (TAG_USER + 1)
#define TT_Connection_MultiServer     			(TAG_USER + 2)

// Fonctions du système initialisation/nettoyage des threads 
BOOL InitThreads(void);
void CleanupThreads(void);
void HandleThreads(BOOL handleAll);
void AbortWorkingThreads(void);
void PurgeIdleThreads(const BOOL purgeAll);
#define DoAction(obj, action, ...) ({ ULONG _tags[] = { SDI_VACAST(__VA_ARGS__) }; DoActionA(obj, action, (struct TagItem *)_tags); })
APTR DoActionA(Object *obj, const enum ThreadAction action, struct TagItem *tags);
BOOL IsMainThread(void);
APTR CurrentThread(void);
const char *CurrentThreadName(void);
BOOL SleepThread(void);
void AbortThread(APTR thread, BOOL targetVanished);
void WakeupThread(APTR thread);
ULONG ThreadConnectionSignal(void);
ULONG ThreadAbortSignal(void);
ULONG ThreadWakeupSignal(void);
LONG ThreadTimerSignal(void);
BOOL ThreadWasAborted(void);
const char *ThreadName(void);
BOOL InitThreadTimer(void);
void CleanupThreadTimer(void);
void StartThreadTimer(ULONG seconds, ULONG micros);
void StopThreadTimer(void);

#endif /* THREADS_H */

