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
#ifndef TIMER_H
#define TIMER_H

#include "timeval.h"

enum Timer
{
 	TIMER_WRINDEX = 0,
  	TIMER_AUTOSAVE,
  	TIMER_READPANEUPDATE,
  	TIMER_READSTATUSUPDATE,
  	TIMER_PROCESSQUICKSEARCH,
  	TIMER_UPDATECHECK,
  	TIMER_SPAMFLUSHTRAININGDATA,
  	TIMER_DELETEZOMBIEFILES,
  	TIMER_CHECKBIRTHDAYS,
  	TIMER_PURGEIDLETHREADS,
  	TIMER_DSTSWITCH,
  	TIMER_NUM
};
// Nos propres structures Timer 
struct TRequest
{
 	struct TimeRequest *tr;             // pointeur sur timerequest
    struct TimeVal startTime;           // a quelle moment la requête a été démarrée
    struct TimeVal remainingTime;       // le temps restant si la requête étatit en pause
    BOOL isRunning;                     // si la requête est couramment active/en marche
    BOOL isPrepared;                    // si la requête est préparée pour faire feu
    BOOL isPaused;                      // si la requête est en ce moment en pause
    BOOL isAbsolute;                    // si la requête porte un TimeVal absolu plutôt que relatif
};

struct Timers
{
 	struct MsgPort  *port;
    struct TRequest timer[TIMER_NUM];
};

void PrepareTimer(const enum Timer tid, const ULONG seconds, const ULONG micros, BOOL absoluteTime);
void StartTimer(const enum Timer tid);
void StopTimer(const enum Timer tid);
void PauseTimer(const enum Timer tid);
void ResumeTimer(const enum Timer tid);
void RestartTimer(const enum Timer tid, const ULONG seconds, const ULONG micros, BOOL absoluteTime);
BOOL CreateTRequest(struct TRequest *timer, const int id);
void DeleteTRequest(struct TRequest *timer);
BOOL InitTimers(void);
void CleanupTimers(void);
BOOL ProcessTimerEvent(void);

#endif /* TIMER_H */

