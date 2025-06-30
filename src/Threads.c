/***************************************************************************
Bourriquet MorphOS
 Copyright (C) 2022 digitally
  
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
#include <string.h>
#include <stdlib.h>
#include <clib/alib_protos.h>
#include <dos/dostags.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

#include "Bourriquet.h"
#include "SDI_stdarg.h"
#include "Extrasrc.h"
#include "Locale.h"
#include "MethodStack.h"
#include "Threads.h"
#include "mui/ClassesExtra.h"
#include "tcp/ConnectionProtocole.h"
#include "Debug.h"

#define MIN_THREADS  2

struct ThreadMessage;

struct Thread
{
    struct MinNode node;     // pour faire de ceçi un noeud exec complet
    struct Process *process; // le pointeur du processus retourné par CreateNewProc()
    struct MsgPort *commandPort;
    struct MsgPort *timerPort;
    struct TimeRequest *timerRequest;
    LONG priority;           // la priorité du thread
    LONG abortSignal;        // un signal alloué pour annuler le thread
    LONG wakeupSignal;       // un signal alloué pour réveiller un thread dormant
	LONG connectionSignal;
    char name[SIZE_DEFAULT]; // le nom du thread
    BOOL working;            // sommes nous en fonctionnement ?
    BOOL aborted;            // avons nous été annulé?
    BOOL timerRunning;       // est ce que la minuterie fonctionne ?
};

//  un noeud de thread qui sera inseré à 
// notre liste global de sous-threads.
struct ThreadNode
{ 
    struct MinNode node;
    struct Thread *thread;
};

struct ThreadMessage
{
    struct Message msg;             // pour rendre ThreadMessage en un message Exec complet
    enum ThreadAction action;       // l'action que le thread doit effectuer
    LONG result;                    // lorsque le thread est terminé, le résultat est stocké içi
    LONG priority;                  // la priorité de tâche auquel le thread doit être mis
    struct TagItem *actionTags;     // les paramétres pour l'action
    struct ThreadNode *threadNode;  // liaison au noeud du thread
    struct Thread *thread;          // liaison au thread lui-même
    Object *object;                 // un objet pour lequel MUIM_ThreadFinished sera appelé
};

// Nous utilisons un message global pour le/la démarrage/fermeture, 
// parce que le démarrage/arrêt n'est pas sujet à des situations de mémoire
// basse où aucun autre objet de message peut être alloué dynamiquement.
// Ceci nous mettrait dans une impasse
static struct ThreadMessage startupMessage;
  
/// FreeThreadTags
// Libére une liste de tags précédemment clonées, en respectant les chaînes dupliquées
static void FreeThreadTags(struct TagItem *tags)
{
  	struct TagItem *tstate = (struct TagItem *)tags;
    struct TagItem *tag;

    ENTER();
    while((tag = NextTagItem((APTR)&tstate)) != NULL)
    	{
    		// libére les balises de chaînes possible 
       		if(isFlagSet(tag->ti_Tag, TAG_STRING)) free((void *)tag->ti_Data);
    	}
    FreeSysObject(ASOT_TAGLIST, tags);
    LEAVE();
}

///
/// CloneThreadTags
// clone les éléments de balise donnés, duplique les chaînes possibles
static struct TagItem *CloneThreadTags(const struct TagItem *tags)
{
	struct TagItem *tstate;
    struct TagItem *clone;
    ULONG numTags = 0;

    ENTER();
    tstate = (struct TagItem *)tags;
  	// compte une balise additionnelle pour la terminaison TAG_DONE
    numTags = 1;
    while(NextTagItem((APTR)&tstate) != NULL) numTags++;
    if((clone = AllocSysObjectTags(ASOT_TAGLIST, ASOTAGS_NumEntries, numTags, TAG_DONE)) != NULL)
    	{
       		struct TagItem *tag;
       		struct TagItem *ctag = clone;

       		tstate = (struct TagItem *)tags;
       		while((tag = NextTagItem((APTR)&tstate)) != NULL)
      	  		{
    				// la balise reste la même
					ctag->ti_Tag = tag->ti_Tag;
					ctag->ti_Data = tag->ti_Data;
			  		// Est ce une balise de chaîne spécial ?
					if(isFlagSet(tag->ti_Tag, TAG_STRING) && tag->ti_Data != (IPTR)NULL)
						{
							// duplique la chaine
							if((ctag->ti_Data = (IPTR)strdup((STRPTR)tag->ti_Data)) == (IPTR)NULL)
						  		{
				  					// pas de mémoire pour la copie de chaîne
				  					// termine la liste à l'élément courant et le libére
									ctag->ti_Tag = TAG_DONE;
									FreeThreadTags(clone);
									clone = NULL;
									break;
						  		}
					  	}
           			ctag++;
       			}
    		// vérifie si le clone a été crée avec succés
       		if(clone != NULL) ctag->ti_Tag = TAG_DONE;
    	}
    RETURN(clone);
    return(clone);
}

///
/// DoThreadMessage
// exécuter l'action demandée
static LONG DoThreadMessage(struct ThreadMessage *msg)
{
 	LONG result = -1;

    ENTER();
    D(DBF_THREAD, "thread '%s' agit %ld", msg->thread->name, msg->action);
    switch(msg->action)
    	{
       		case TA_Startup:
       		case TA_Shutdown:
       			break;
       		case TA_StartSingleConnection:
				D(DBF_NET, "start single connection");
      			result = ConnectionServer((struct Server *)GetTagData(TT_Connection_OneServer, (IPTR)NULL, msg->actionTags), msg->actionTags);
          		break;
			case TA_StartMultiConnection:
				printf("start multi connection\n");
				//result = ConnectionServer((struct Server *)GetTagData(TT_Connection_MultiServer, (IPTR)NULL, msg->actionTags), msg->actionTags);
				break;
		 	case TA_EndConnection:
				D(DBF_THREAD, "TA_EndConnection");
				break;
      	}
    D(DBF_THREAD, "thread '%s' action accomplie %ld, resultat %ld", msg->thread->name, msg->action, result);
    RETURN(result);
    return(result);
}

///
/// ThreadEntry
// point d'entrée pour un nouveau thread
static SAVEDS void ThreadEntry(void)
{
 	struct Process *proc;
    BOOL done = FALSE;
    struct ThreadMessage *msg;

    proc = (struct Process *)FindTask(NULL);
  	// attendre l'arrivée du message de démarrage au port de message du thread
    WaitPort(&proc->pr_MsgPort);
    if((msg = (struct ThreadMessage *)GetMsg(&proc->pr_MsgPort)) != NULL && msg->action == TA_Startup)
    	{
        	struct Thread *thread = msg->thread;
        	BOOL initOk = FALSE;

    		// nous devons pas utiliser des fonctions I/O avant de recevoir le message de démarrage, 
    		// par conséquent la macro ENTER() est appelée içi
        	ENTER();
			D(DBF_THREAD, "thread %08lx '%s' a reçu un message de démarrage", thread, thread->name);
        	if((thread->abortSignal = AllocSignal(-1)) != -1)
          		{
					D(DBF_THREAD, "thread->abortSignal");
            		if((thread->wakeupSignal = AllocSignal(-1)) != -1)
              			{
							D(DBF_THREAD, "thread->wakeupSignal");
        					// alloue un port de message séparé pour ne pas interférer avec les fonctions I/O standards
        					// qui utilisent proc->pr_MsgPort
                			if((thread->commandPort = AllocSysObjectTags(ASOT_PORT, TAG_DONE)) != NULL)
                  				{
									// se souvenir du pointeur de thread dans le champ tc_UserData de la tâche
									// cela sera utilisé chaque fois que la thread courant doit être obtenu
									D(DBF_THREAD, "thread->commandPort");
									proc->pr_Task.tc_UserData = thread;
									msg->result = TRUE;
									initOk = TRUE;
									// change notre priorité initial de 1 à 0
									SetTaskPri((struct Task *)proc, 0);
                  				}
                			else
                 	 			{
									E(DBF_THREAD, "ERREUR: thread '%s' n'a pas attribué de port de commande", thread->name);
                    				FreeSignal(thread->wakeupSignal);
                    				FreeSignal(thread->abortSignal);
                    				msg->result = FALSE;
                  				}
              			}
            		else
              			{
							E(DBF_THREAD, "ERREUR: thread '%s' n'a pas alloué le signal de réveil", thread->name);
                			FreeSignal(thread->abortSignal);
                			msg->result = FALSE;
              			}
          		}
        	else
          		{
					E(DBF_THREAD, "ERREUR: thread '%s' êchec d'allocation du signal d'abandon", thread->name);
            		msg->result = FALSE;
          		}
    		// retourne le message à l'expéditeur
        	ReplyMsg((struct Message *)msg);
        	if(initOk == TRUE)
          		{
            		do
            			{
							// Attendre que les messages arrivent au port de message de commande
							WaitPort(thread->commandPort);
							// traite tous les messages en attente
							while((msg = (struct ThreadMessage *)GetMsg(thread->commandPort)) != NULL)
								{
                   					D(DBF_THREAD, "eu 1 message %08lx, action %ld", msg, msg->action);
                   					switch(msg->action)
                   						{
                       						case TA_Shutdown:
                       							{
													D(DBF_THREAD, "thread '%s' a reçu un message d'arrêt", thread->name);
            										// libére tous les ressources allouées et sort de la boucle
                           							if(thread->commandPort != NULL)
                           								{
															FreeSysObject(ASOT_PORT, thread->commandPort);
															thread->commandPort = NULL;
														}
													if(thread->abortSignal != -1)
													  	{
															FreeSignal(thread->abortSignal);
															thread->abortSignal = -1;
														}
													if(thread->wakeupSignal != -1)
													  	{
															FreeSignal(thread->wakeupSignal);
															thread->wakeupSignal = -1;
														}
													msg->result = TRUE;
													done = TRUE;
												}
												break;
											default:
												{
								  					// efface le signal d'abandon avant l'éxecution de l'action désiré
												  	thread->aborted = FALSE;
												  	SetSignal(0UL, (1UL << thread->abortSignal) | (1UL << thread->wakeupSignal));
												  	msg->result = DoThreadMessage(msg);
												}
											  	break;
										}
          							// retourne le message à l'expéditeur
                   					ReplyMsg((struct Message *)msg);
          							// vérifie si nous devons sortir complétement
                   					if(done == TRUE) break;
               					}
            			}
          			while(done == FALSE);
        		}
      		LEAVE();
   		}
}

///
/// SleepThread
// mettre le thread courant en veille
BOOL SleepThread(void)
{
	ULONG abortMask;
	ULONG wakeupMask;
	ULONG signals;
	BOOL notAborted;

	ENTER();
	abortMask = (1UL << ThreadAbortSignal());
	wakeupMask = (1UL << ThreadWakeupSignal());
	D(DBF_THREAD, "thread '%s' en attente des signaux %08lx", ThreadName(), abortMask|wakeupMask);
	signals = Wait(abortMask|wakeupMask);
	D(DBF_THREAD, "thread '%s' eu des signaux %08lx", ThreadName(), signals);
	notAborted = isFlagClear(signals, abortMask);
	RETURN(notAborted);
	return(notAborted);
}

///
/// AbortThread
// signal au thread d'annuler l'action en cours
void AbortThread(APTR thread, BOOL waitForTermination)
{
	struct Process *proc;
    ULONG sig;

    ENTER();
    if(thread == NULL || thread == G->mainThread)
    	{
        	D(DBF_THREAD, "ATTENTION : abandon du fil principal !!!");
        	proc = G->mainThread;
        	sig = SIGBREAKB_CTRL_C;
        	waitForTermination = FALSE;
      	}
    else
     	{
        	struct Thread *_thread = thread;

        	D(DBF_THREAD, "ATTENTION : fil d'execution %s !!! ", _thread->name);
        	_thread->aborted = TRUE;
        	proc = _thread->process;
        	sig = _thread->abortSignal;
      	}
    Signal((struct Task *)proc, 1UL << sig);
    if(waitForTermination == TRUE)
     	{
        	struct Thread *_thread = thread;

    		// maintenant en attente jusquà ce que le thread a fini son travail
        	do
          		{
		    		MicroMainLoop();
            		Delay(10);
          		}
        	while(_thread->working == TRUE);
      	}
    LEAVE();
}

///
/// WakeupThread
// signale au thread de continuer son travail
void WakeupThread(APTR thread)
	{
    	struct Process *proc;
    	ULONG sig;

    	ENTER();
    	if(thread == NULL || thread == G->mainThread)
      		{
				D(DBF_THREAD, "réveil du fil principal");
        		proc = G->mainThread;
        		sig = SIGBREAKB_CTRL_E;
      		}
    	else
      		{
        		struct Thread *_thread = thread;

				D(DBF_THREAD, "réveil du fil %s", _thread->name);
        		proc = _thread->process;
        		sig = _thread->wakeupSignal;
      		}
    	Signal((struct Task *)proc, 1UL << sig);
   	 	LEAVE();
	}

///
/// AbortWorkingThreads
// signale à tous les threads toujours en fonctionnement d'annuler l'action en cours
void AbortWorkingThreads(void)
	{
    	struct Node *node;

    	ENTER();
		D(DBF_THREAD, "abortworkingthread");
  		// toujours prendre le premier noeud dans la liste, parce que plus tard les appels à HandleThreads()
  		// modifierons la liste, sinon l'itération ne donnera pas le résultat souhaité 
    	while((node = GetHead((struct List *)&G->workingThreads)) != NULL)
      		{
        		struct ThreadNode *threadNode = (struct ThreadNode *)node;
        		struct Thread *thread = threadNode->thread;
    			
				// annule le thread en fonctionnement et attend jusqu'à ce qu'il termine son travail 
				D(DBF_THREAD, "fil %s", thread->name);
        		AbortThread(thread, TRUE);
      		}
    	LEAVE();
	}

///
/// ShutdownThread
// termine un thread unique
static void ShutdownThread(struct ThreadNode *threadNode)
	{
    	struct Thread *thread = threadNode->thread;

    	ENTER();
  		// prépare le message d'arrêt
    	memset(&startupMessage, 0, sizeof(startupMessage));
    	startupMessage.msg.mn_ReplyPort = G->threadPort;
    	startupMessage.msg.mn_Length = sizeof(startupMessage);
    	startupMessage.action = TA_Shutdown;
    	startupMessage.threadNode = threadNode;
    	startupMessage.thread = thread;
  		// envoie le message d'arrêt et attend une réponse
    	D(DBF_THREAD, "envoi du message d'arret du fil %s", thread->name);
    	PutMsg(thread->commandPort, (struct Message *)&startupMessage);
    	Remove((struct Node *)WaitPort(G->threadPort));
    	D(DBF_THREAD, "thread '%s' est mort maintenant", thread->name);
    	G->numberOfThreads--;
    	LEAVE();
	}

///
/// CreateThread
// lance une fonction donnée dans un thread nouvellement créé sous un nom donné
// qui est lié dans une liste interne
static struct Thread *CreateThread(void)
	{
    	struct Thread *result = NULL;
    	struct ThreadNode *threadNode;

    	ENTER();
    	if((threadNode = AllocSysObjectTags(ASOT_NODE, ASONODE_Size, sizeof(*threadNode), ASONODE_Min, TRUE, TAG_DONE)) != NULL)
      		{
        		struct Thread *thread;

        		if((thread = AllocSysObjectTags(ASOT_NODE, ASONODE_Size, sizeof(*thread), ASONODE_Min, TRUE, TAG_DONE)) != NULL)
          			{
            			threadNode->thread = thread;
            			memset(thread, 0, sizeof(*thread));
            			thread->abortSignal = -1;
            			thread->wakeupSignal = -1;
            			snprintf(thread->name, sizeof(thread->name), "thread Bourriquet [%d]", (int)G->threadCounter+1);
            			if((thread->process = CreateNewProcTags(NP_Entry,       ThreadEntry, //  fonction d'entrée
            										                                        NP_StackSize,   SIZE_STACK,  // taille de la pile
            										                                        NP_Name,        thread->name,
            										                                        NP_Priority,    1,
            										                                        NP_Input,       Input(),
            										                                        NP_CloseInput,  FALSE,
            										                                        NP_Output,      Output(),
            										                                        NP_CloseOutput, FALSE,
            										                                        NP_CodeType,    MACHINE_PPC,
            										                                        TAG_DONE)) != NULL)
              				{
                				struct Message *msg;

        						// prépare le message de démarrage
                				memset(&startupMessage, 0, sizeof(startupMessage));
                				startupMessage.msg.mn_ReplyPort = G->threadPort;
                				startupMessage.msg.mn_Length = sizeof(startupMessage);
                				startupMessage.action = TA_Startup;
                				startupMessage.threadNode = threadNode;
                				startupMessage.thread = thread;
        						// envoie le message de démarrage
								D(DBF_THREAD, "thread 0x%08lx '%s' démarré, envoi du message de démarrage", thread, thread->name);
                				PutMsg(&thread->process->pr_MsgPort, (struct Message *)&startupMessage);
						        // maintenant en attente d'une réponse 
                				do
                  					{
										D(DBF_THREAD, "en attente de réponse au message de démarrage");
                    					msg = WaitPort(G->threadPort);
										D(DBF_THREAD, "eu 1 message %08lx, expecté %08lx", msg, &startupMessage);
                 					 	// soyons sûr d'avoir le retour de notre message de démarrage 
                    					if(&startupMessage == (struct ThreadMessage *)msg)
                      						{
            									// WaitPort() ne supprime pas le message, nous devons le faire nous-même
                        						Remove((struct Node *)msg);
            									// vérifie si la fonction du thread a retourné quelque chose de valide 
                        						if(startupMessage.result != 0)
                          							{
														// incrémente le compteur de thread
														G->numberOfThreads++;
														G->threadCounter++;
														AddTail((struct List *)&G->idleThreads, (struct Node *)threadNode);
														result = thread;
													 	// nous avons eu en retour le message, nous sortons d'içi
														break;
													}
                        						else
                          							{
                            							E(DBF_THREAD, "ERREUR: thread 0x%08lx '%s' impossible à initialiser", thread, thread->name);
                            							ShutdownThread(threadNode);
                          							}
                      						}
                    					else HandleThreads(FALSE);
                  					}
                				while(msg != NULL);
              				}
            			if(result == NULL) FreeSysObject(ASOT_NODE, thread);
          			}
        		if(result == NULL) FreeSysObject(ASOT_NODE, threadNode);
      		}
    	RETURN(result);
    	return(result);
	}

///
/// InitThreads
// initialise le système de thread 
BOOL InitThreads(void)
{
 	BOOL result = FALSE;

    ENTER();
  	// initialise les listes de threads
    NewMinList(&G->idleThreads);
    NewMinList(&G->workingThreads);
    G->mainThread = (struct Process *)FindTask(NULL);
    if((G->threadPort = AllocSysObjectTags(ASOT_PORT, TAG_DONE)) != NULL)
    	{
        	ULONG i;

        	result = TRUE;
    		// mettre en place une réserve de threads inactifs
        	for(i = 0; i < MIN_THREADS; i++)
          		{
            		if(CreateThread() == NULL) break;
          		}
      	}
    RETURN(result);
    return(result);
}

///
/// CleanupThreads
// nettoie tout le système de thread - aannule eventuellement les threads actifs 
// et les attend pour finir proprement
void CleanupThreads(void)
{
 	ENTER();
    if(G->threadPort != NULL)
  		{
    		BOOL tryAgain = TRUE;

    		// envoit un signal d'abandon à chaque fil inactif encore existant 
    		AbortWorkingThreads();
		    // envoit un message d'arrêt à chaque fil inactif encore existant 
		    PurgeIdleThreads(TRUE);
		    // gestion des transitions possibles des fils working->idle 
    		MicroMainLoop();
		    do
			    {
      				// Vérifier en premier si il y a encore des fils en fonctionnement
      				if(IsMinListEmpty(&G->workingThreads) == FALSE)
      					{
       				 			// il y a des fils toujours en fonctionnement, demande à l'utilisateur ce qu'il faut faire
        						if(MUI_Request(G->App, NULL, 0L, tr(MSG_THREAD_EXIT_WARNING_TITLE), tr(MSG_THREAD_EXIT_WARNING_BT), tr(MSG_THREAD_EXIT_WARNING)) != 0)
        							{
        								  // l'utilisateur veut continuer encore, gestion des transitions
        								  MicroMainLoop();
        							}
        						else
        							{
          								// ne pas attendre plus longtemps
          								tryAgain = FALSE;
        							}
      					}
					// envoit un message d'arrêt à chaque fil toujours existant
    			  	PurgeIdleThreads(TRUE);
    			}
    		while(tryAgain == TRUE && IsMinListEmpty(&G->workingThreads) == FALSE);
    		FreeSysObject(ASOT_PORT, G->threadPort);
    		G->threadPort = NULL;
  		}
    LEAVE();
}

///
/// HandleThreads
// traîte un message retourné par l'un des threads
void HandleThreads(BOOL handleAll)
	{
    	struct Message *msg;

    	ENTER();
    	while((msg = GetMsg(G->threadPort)) != NULL)
      		{
        		struct ThreadMessage *tmsg = (struct ThreadMessage *)msg;

				D(DBF_THREAD, "thread '%s' action %ld finie, résultat %ld", tmsg->thread->name, tmsg->action, tmsg->result);
    			// nous ne devons pas libérer le message de démarrage
        		if(tmsg->action != TA_Startup)
          			{
            			if(tmsg->object != NULL)
              				{
								D(DBF_THREAD, "envoi MUIM_ThreadFinished à l'objet %08lx", tmsg->object);
                				DoMethod(tmsg->object, MUIM_ThreadFinished, tmsg->action, tmsg->result, tmsg->actionTags);
              				}
      					// supprime le thread à partir de la liste de fonctionnement et le remettre dans la liste des inactifs
            			Remove((struct Node *)tmsg->threadNode);
            			tmsg->threadNode->thread->working = FALSE;
            			AddTail((struct List *)&G->idleThreads, (struct Node *)tmsg->threadNode);
      					// remet la priorité du thread à zéro
            			if(tmsg->thread->priority != 0) SetTaskPri((struct Task *)tmsg->thread->process, 0);
      					// libére les paramétres
            			FreeThreadTags(tmsg->actionTags);
      					// finalement élimine le message
            			FreeSysObject(ASOT_MESSAGE, tmsg);
          			}
    			// sort si on nous demande de gérer seulement un seul message
        		if(handleAll == FALSE) break;
      		}
    	LEAVE();
	}

///
/// PurgeIdleThreads
// termine tous les threads inactifs pour sauvegarder la mémoire
void PurgeIdleThreads(const BOOL purgeAll)
	{
    	struct Node *node;
    	ULONG limit;

    	ENTER();
  		// purge tous les threads inactifs ou sinon laisser le nombre minimum de threads vivants
    	if(purgeAll == TRUE) limit = 0;
    	else limit = MIN_THREADS;
    	while(G->numberOfThreads > limit && (node = RemHead((struct List *)&G->idleThreads)) != NULL)
      		{
        		struct ThreadNode *threadNode = (struct ThreadNode *)node;

        		ShutdownThread(threadNode);
        		FreeSysObject(ASOT_NODE, threadNode->thread);
        		FreeSysObject(ASOT_NODE, threadNode);
      		}
    	LEAVE();
	}

///
/// DoAction
APTR DoActionA(Object *obj, const enum ThreadAction action, struct TagItem *tags)
	{
    	APTR success = NULL;
    	struct Node *node;

    	ENTER();
  		// essaye d'obtenir un thread inactif
  		// s'il n'y en a plus, nous en créeons un nouveau
    	while((node = GetHead((struct List *)&G->idleThreads)) == NULL)
      		{
        		if(CreateThread() == NULL) break;
      		}
    	if(node != NULL)
      		{
        		struct ThreadNode *threadNode = (struct ThreadNode *)node;
        		struct Thread *thread = threadNode->thread;
        		struct ThreadMessage *msg;

				D(DBF_THREAD, "Trouver tache inactive '%s'", thread->name);
        		if((msg = AllocSysObjectTags(ASOT_MESSAGE, ASOMSG_Size, sizeof(*msg),  ASOMSG_ReplyPort, (IPTR)G->threadPort, TAG_DONE)) != NULL)
          			{
            			msg->actionTags = CloneThreadTags((const struct TagItem *)tags);
            			SHOWVALUE(DBF_THREAD, msg->actionTags);
            			if(msg->actionTags != NULL)
              				{
                				LONG pri;
        
								// configure le message d'action
                				msg->action = action;
                				msg->threadNode = threadNode;
                				msg->thread = thread;
                				msg->object = obj;
        						// augmente la priorité du threadsi c'est nécessaire
                				if((pri = GetTagData(TT_Priority, 0, msg->actionTags)) != 0)
                  					{
                    					SetTaskPri((struct Task *)thread->process, pri);
                    					thread->priority = pri;
                  					}
        						// envoie le message au thread
                				PutMsg(thread->commandPort, (struct Message *)msg);
        						// supprime le thread à partir de la liste des inactifs et le met dans la liste de fonctionnement 
								Remove((struct Node *)threadNode);
								threadNode->thread->working = TRUE;
								AddTail((struct List *)&G->workingThreads, (struct Node *)threadNode);
								// retourne le thread conducteur si réussi
								success = thread;
							}
            			else
              				{
                				FreeSysObject(ASOT_MESSAGE, msg);
                				E(DBF_THREAD, "ERREUR: impossible de cloner les balises d'action pour l'action %ld", action);
              				}
          			}
				else E(DBF_THREAD, "ERREUR : impossible d'allouer un message de commande pour l'action %ld", action);
      		}
		else E(DBF_THREAD, "ERREUR : impossible de créer un nouveau thread pour l'action %ld", action);
    	RETURN(success);
    	return(success);
	}

///
/// IsMainThread
// vérifie que nous sommes dans le contexte du thread principal
BOOL IsMainThread(void)
	{
    	BOOL isMainThread = FALSE;

    	ENTER();
    	if((struct Process *)FindTask(NULL) == G->mainThread) isMainThread = TRUE;
    	RETURN(isMainThread);
    	return(isMainThread);
	}

///
/// CurrentThread
// prend le thread courant 
APTR CurrentThread(void)
{
	APTR thread;

    ENTER();
    if(IsMainThread() == TRUE) 
		{
			thread = G->mainThread;
			D(DBF_THREAD, "G->mainThread");
		}
    else 
		{
			D(DBF_THREAD, "!G->mainThread");
			thread = ((struct Process *)FindTask(NULL))->pr_Task.tc_UserData;
		}
    RETURN(thread);
    return(thread);
}

///
/// CurrentThreadName
// prend le nom du thread courant
const char *CurrentThreadName(void)
{
	const char *name;

    ENTER();
    if(IsMainThread() == TRUE) name = "main";
    else
    	{
       		struct Thread *thread = ((struct Process *)FindTask(NULL))->pr_Task.tc_UserData;

       		name = thread->name;
    	}
    RETURN(name);
    return(name);
}

///
/// ThreadAbortSignal
// prend le signal d'abandon du thread courant, il s'agit de CTRL-C pour le thread principal
ULONG ThreadAbortSignal(void)
	{
    	ULONG sigBit;
    	struct Process *me;

    	ENTER();
    	me = (struct Process *)FindTask(NULL);
    	if(me == G->mainThread) sigBit = SIGBREAKB_CTRL_C;
    	else
      		{
        		struct Thread *thread = (struct Thread *)me->pr_Task.tc_UserData;

        		sigBit = thread->abortSignal;
      		}
    	RETURN(sigBit);
    	return(sigBit);
	}

///
/// ThreadWakeupSignal
// prend le signal de réveil du thread courant, il s'agit de CTRL-E pour le thread principal
ULONG ThreadWakeupSignal(void)
{
 	LONG sigBit;
    struct Process *me;

    ENTER();
    me = (struct Process *)FindTask(NULL);
    if(me == G->mainThread) sigBit = SIGBREAKB_CTRL_E;
    else
    	{		
       		struct Thread *thread = (struct Thread *)me->pr_Task.tc_UserData;

       		sigBit = thread->wakeupSignal;
    	}
    RETURN(sigBit);
    return(sigBit);
}

///
/// ThreadTimerSignal
// prend le signal de la minuterie du thread courant
LONG ThreadTimerSignal(void)
{
 	ULONG sigBit;
    struct Process *me;

    ENTER();
    me = (struct Process *)FindTask(NULL);
    if(me == G->mainThread) sigBit = G->timerData.port->mp_SigBit;
    else
    	{
        	struct Thread *thread = (struct Thread *)me->pr_Task.tc_UserData;

        	if(thread->timerRequest != NULL) sigBit = thread->timerPort->mp_SigBit;
        	else sigBit = -1;
      	}
    RETURN(sigBit);
    return(sigBit);
}

///
/// ThreadWasAborted
// retourne l'état d'abandon du thread courant 
BOOL ThreadWasAborted(void)
{
 	BOOL aborted;
    struct Process *me;

    ENTER();
    me = (struct Process *)FindTask(NULL);
    if(me == G->mainThread) aborted = FALSE;
    else
    	{
       		struct Thread *thread = (struct Thread *)me->pr_Task.tc_UserData;

       		aborted = thread->aborted;
    	}
    RETURN(aborted);
   	return(aborted);
}

///
/// ThreadName
// retourne le nom du thread courant 
const char *ThreadName(void)
	{
    	const char *name;

    	ENTER();
    	if(IsMainThread() == TRUE) name = "main";
    	else
      		{
        		struct Thread *thread = CurrentThread();

        		name = thread->name;
      		}
    	RETURN(name);
    	return(name);
	}

///
/// InitThreadTimer
// initalise une minuterie pour le thread conducteur
BOOL InitThreadTimer(void)
	{
    	BOOL success = FALSE;
    	struct Thread *thread = CurrentThread();

    	ENTER();
    	if(thread->timerRequest == NULL)
      		{
        		if((thread->timerPort = AllocSysObjectTags(ASOT_PORT, TAG_DONE)) != NULL)
          			{
            			if((thread->timerRequest = AllocSysObjectTags(ASOT_IOREQUEST,
              																						ASOIOR_Size, sizeof(*thread->timerRequest), ASOIOR_ReplyPort, (IPTR)thread->timerPort, TAG_DONE)) != NULL)
              				{
                				if(OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)thread->timerRequest, 0L) == 0)
                  					{
                    					thread->timerRunning = FALSE;
                    					success = TRUE;
                  					}
              				}
          			}
      		}
    	if(success == FALSE)
      		{
        		if(thread->timerRequest != NULL)
          			{
            			FreeSysObject(ASOT_IOREQUEST, thread->timerRequest);
            			thread->timerRequest = NULL;
          			}
        		if(thread->timerPort != NULL)
          			{
            			FreeSysObject(ASOT_PORT, thread->timerPort);
            			thread->timerPort = NULL;
          			}
      		}
    	RETURN(success);
    	return(success);
	}

///
/// CleanupThreadTimer
// supprime une minuterie de thread précédemment initialisée
void CleanupThreadTimer(void)
	{
    	struct Thread *thread = CurrentThread();

    	ENTER();
    	if(thread->timerRequest != NULL)
      		{
        		StopThreadTimer();
        		if(thread->timerRequest != NULL)
          			{
            			CloseDevice((struct IORequest *)thread->timerRequest);
            			FreeSysObject(ASOT_IOREQUEST, thread->timerRequest);
            			thread->timerRequest = NULL;
          			}
        		if(thread->timerPort != NULL)
          			{
            			FreeSysObject(ASOT_PORT, thread->timerPort);
            			thread->timerPort = NULL;
          			}
      		}
    	LEAVE();
	}

///
/// StartThreadTimer
void StartThreadTimer(ULONG seconds, ULONG micros)
	{
    	struct Thread *thread = CurrentThread();

    	ENTER();
    	if(thread->timerRequest != NULL && thread->timerRunning == FALSE)
      		{
        		if(seconds > 0 || micros > 0)
          			{
      					// émet un nouveau timerequest
            			thread->timerRequest->Request.io_Command = TR_ADDREQUEST;
            			thread->timerRequest->Time.Seconds       = seconds;
            			thread->timerRequest->Time.Microseconds  = micros;
            			SendIO((struct IORequest *)thread->timerRequest);
            			thread->timerRunning = TRUE;
          			}
      		}
    	LEAVE();
	}

///
/// StopThreadTimer
void StopThreadTimer(void)
	{
    	struct Thread *thread = CurrentThread();

    	ENTER();
    	if(thread->timerRequest != NULL && thread->timerRunning == TRUE)
      		{
        		if(CheckIO((struct IORequest *)thread->timerRequest) == NULL) AbortIO((struct IORequest *)thread->timerRequest);
        		WaitIO((struct IORequest *)thread->timerRequest);
        		thread->timerRunning = FALSE;
      		}
    	LEAVE();
	}

///
