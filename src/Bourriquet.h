/***************************************************************************
Bourriquet MorphOS
	2024-2025 Digitally
***************************************************************************/
#ifndef BOURRIQUET_H
#define BOURRIQUET_H

#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#include <libraries/cybergraphics.h>

#include "Stringsizes.h"
#include "Utilities.h"   
#include "Timer.h"      
#include "Themes.h"
#include "Lists.h"

struct Server;
struct DiskObject;
struct AppIcon;
struct MsgPort;
struct FileReqCache;
struct Locale;
struct Catalog;
struct NotifyRequest;
struct Process;

enum GlobalDispatcherJob
	{
  		ID_CLOSEALL=1000,
  		ID_RESTART,
  		ID_RESTART_FORCE,
  		ID_LOGIN
	};
/*** Structure globale ***/
struct Global
{
 	APTR SharedMemPool; // MEMF_SHARED réservoir de mémoire
  	Object *App;
  	Object *SplashWinObject;
  	Object *DashBoardWin;
	Object *ConnectionWinObject;
  	struct DiskObject *HideIcon;
  	struct AppIcon *AppIcon;
  	struct MsgPort *AppPort;
  	struct Locale *Locale;
  	struct Catalog *Catalog;
  	struct Screen *workbenchScreen;
  	LONG Weights[12];
  	int currentAppIcon;
  	int gmtOffset;           // Le décalage actuel à GMT en min (+/-)
  	BOOL SingleTask;
  	BOOL AppIconQuiet;
  	BOOL InStartupPhase;
  	BOOL NoCatalogTranslation;
  	BOOL DefIconsAvailable;
  	BOOL Terminating;
  	struct DateStamp StartDate;
  	struct Interrupt *lowMemHandler;        // Gestion de basse mémoire pour vider tous les index 
  	struct MsgPort *threadPort;
  	struct MsgPort *methodStack;
  	char ProgDir[SIZE_PATH];
  	char ProgName[SIZE_FILE];
  	struct Timers timerData;  
  	struct MinList idleThreads;
  	struct MinList workingThreads;
  	struct MinList normalBusyList;
  	ULONG numberOfThreads;
  	ULONG threadCounter;
  	struct Process  *mainThread;
  	struct HashTable *imageCacheHashTable;
  	BOOL NoImageWarning;  
  	struct codeset *systemCodeset;
  	struct codeset *localCodeset;
  	struct codesetList *codesetsList;
  	struct Theme theme;
  	char ThemesDir[SIZE_PATH];
  	struct SignalSemaphore * globalSemaphore; 
  	BOOL LowMemSituation;
  	struct ServerList *ServerMet;
	struct ShareList *ShareFiles;
  	struct MA_ClassData *MA;
  	struct ER_ClassData *ER;
  	char *ER_Message[MAXERR];
  	int ER_NumErr;
  	struct SignalSemaphore *configSemaphore;      // un sémaphore pour empêcher les modifications simultanées de la configuration
  	int activeConnections;
	BOOL ActiveConn;
	//struct list list_servers;
	struct list list_hosts;
	struct list list_shared;
	struct list list_downloads;
	struct list list_research;
};

extern struct Global *G;

void PopUp(void);
BOOL StayInProg(void);
void MiniMainLoop(void);
void MicroMainLoop(void);

#endif 

