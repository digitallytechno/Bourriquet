/***************************************************************************
Bourriquet MorphOS
2024-2025 digitally
***************************************************************************/
#include <string.h>

#define __NOLIBBASE__
#define __NOGLOBALIFACE__
#include <proto/bsdsocket.h>
#undef __NOGLOBALIFACE__
#undef __NOLIBBASE__

#include <clib/alib_protos.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/timer.h>
#include <proto/utility.h>
#include <net/socketbasetags.h>
#include <hardware/byteswap.h>
#include <sys/filio.h>
#include <netdb.h>
#include <assert.h>

#include "Extrasrc.h"
#include "Bourriquet.h"
#include "Error.h"
#include "Stringsizes.h"
#include "DashBoard.h"

#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/TheBar_mcc.h>
#include "mui/ClassesExtra.h"
#include "mui/BourriquetApplication.h"
#include "tcp/Connection.h"
#include "tcp/ConnectionProtocole.h"
#include "tcp/Socket.h"
#include "tcp/Host.h"

#include "Server.h"
#include "Locale.h"
#include "MethodStack.h"
#include "MUIObjects.h"
#include "Requesters.h"
#include "Threads.h"
#include "Config.h"
#include "Debug.h"
#include "tcp/Parser.h"

#define FD_ACCEPT 0x01	/* there is a connection to accept() */
#define FD_CONNECT 0x02	/* connect() completed */
#define FD_OOB	0x04	/* socket has out-of-band data */
#define FD_READ 0x08	/* socket is readable */
#define FD_WRITE 0x10	/* socket is writeable */
#define FD_ERROR 0x20	/* asynchronous error on socket */
#define FD_CLOSE 0x40	/* connection closed (graceful or not) */
#define SO_EVENTMASK 0x2001
#define SOL_SOCKET 0xffff	/* options for socket level */
#define RECEIVEF_SIGNAL  (1<<0) // wakeup a waiting thread after the transfer

extern struct timerequest *tr;
int sharedListSent = 0;
int downloadFilesReasked = 0;
int connected;
LONG serverSockfd;
LONG max_sockfd;
unsigned char userHash[16];
ULONG myIp;
int userPort;
ULONG serverIP;
int serverPort;
ULONG eventmask;
LONG insockfd;
char userName[255];
BYTE  sig;
ULONG ed2k_sigmask;

struct Library *SocketBase;
/**************************************************************************************/
void host_cleanup(void)
{
   	struct Host *entry;

   	while((entry = (struct Host *)list_remove_tail(&G->list_hosts)))
      	{
			if(entry->sa) free(entry->sa);
         	if(entry->ip) free(entry->ip); 
         	if(entry->nom) free(entry->nom); 
	 		free(entry);
    	}
}

BOOL ConnectionIsOnline(void)
{
 	BOOL isonline = FALSE;
		
  	ENTER();
 	if((SocketBase = OpenLibrary("bsdsocket.library", 4L)) != NULL)  isonline = TRUE;
    else D(DBF_NET, "Trouvé la pile TCP/IP en position %s", isonline ? "ONLINE" : "OFFLINE");		
	RETURN(isonline);
  	return(isonline);
}

/***************************************************************************/
long get_sockfd(void)
{
   	long sockfd;

   	assert(SocketBase);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) 
		{
			D(DBF_NET, "Incapable de créer un socket");
			return(-1);
		}
	else D(DBF_NET, "socket crée sockfd %ld", sockfd);
   	return(sockfd);
}

/***************************************************************************/
int ed2k_login(char *ip, int port, char *name)
{
   	char HostName[255];
   	int RetCode;
   	struct hostent *hostEntry;
   	long n, value = 0;
	const int reuseaddr = 1;
   	struct sockaddr_in sa;
	int error = CS_NOTCONNECTED;
    struct Host *host;
	
	memcpy(userHash, newID(), 16);
	D(DBF_NET, "userHash %ld", userHash);
	if(C->UserName != NULL) strcpy(userName, C->UserName);
	else strcpy(userName, "bourriquet");
	if(C->ClientPort != NULL) userPort = C->ClientPort;
	else userPort = 4662;		
	sig = AllocSignal(-1);
	if(sig)
		{
			ed2k_sigmask = (1L << sig);
   			SocketBaseTags(SBTM_SETVAL(SBTC_SIGEVENTMASK), ed2k_sigmask, SBTM_SETVAL(SBTC_DTABLESIZE), 4096, TAG_DONE);		
		}			
	else return(error);
   	RetCode = gethostname(HostName, sizeof(HostName));  
   	if(RetCode < 0) return(error);
	D(DBF_NET, "hostname %s", HostName);
	hostEntry = gethostbyname(HostName);
	if(hostEntry == 0) return(error);
	myIp = *((ULONG *)hostEntry->h_addr_list[0]); 
	D(DBF_NET, "myIp %s", Inet_NtoA(myIp));
    assert(SocketBase);
   	if((insockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		{
			D(DBF_NET, "socket invalide");
         	return(error);
      	}
   	assert(SocketBase);
	D(DBF_NET, "socket: %ld... conn->userPort : %ld", insockfd, userPort);
   	setsockopt(insockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
   	setsockopt(insockfd, SOL_SOCKET, SO_REUSEPORT, &reuseaddr, sizeof(reuseaddr)); 
   	sa.sin_family = AF_INET;
   	sa.sin_addr.s_addr = htonl(INADDR_ANY);
   	sa.sin_port = htons(userPort);
	D(DBF_NET, "bind sur socket: %ld ... Port :%ld", insockfd, (long)userPort);
	if(bind(insockfd, (struct sockaddr *)&sa, sizeof(sa)) == -1)
		{
			D(DBF_NET, "bind invalide");
         	return(error);
      	}
   	assert(SocketBase);
	D(DBF_NET, "listen sur socket: %ld ... Port :%ld", insockfd, (long)userPort);
   	if(listen(insockfd, 35) != 0)
		{
			D(DBF_NET, "listen invalide");
         	return(error);
      	}
   	assert(SocketBase);
   	n = 1;
	D(DBF_NET, "IoctlSocket");
   	IoctlSocket(insockfd, FIONBIO, (char *)&n);
	IoctlSocket(insockfd, FIOASYNC, (char*)&n);   
   	value = FD_CONNECT | FD_ACCEPT | FD_OOB | FD_READ | FD_WRITE | FD_CLOSE | FD_ERROR;
   	D(DBF_NET, "setsockopt");
	setsockopt(insockfd, SOL_SOCKET, SO_EVENTMASK, &value, sizeof(LONG));
   	assert(SocketBase);
	if((host = calloc(sizeof(struct Host), 1)) != NULL)
		{
			host->ip = (char *)mystrdup(ip);
			D(DBF_NET, "host->ip %s", host->ip);
         	host->nom = (char *)mystrdup(name);
			D(DBF_NET, "host->nom %s", host->nom);
			host->port = port;
         	host->serverIP = (unsigned long)inet_addr(ip);
         	host->serverPort = port;
         	host->state = CONNECTION_NULL;
         	host->retry = 0;
         	host->hosttype = 1;
         	host->connection = 0;
			list_insert_tail(&G->list_hosts, &host->node);
			return(CS_CONNECTING);
		}
	else return(error);
   	return(error);
}
/***************************************************************************/
void pollEdkEvents(struct Host *node)
{
   	long sockfd = sockfd;
   	char cmd[256];

	//D(DBF_NET, "polledkevents");
   	if(node->port == 0 && node->sockfd == 0) return;
   	if(node->state < CONNECTION_IN_HANDLE && !node->connection)
      	{
			//D(DBF_NET, "node->state < CONNECTION_IN_HANDLE && !node->connection");
         	if((node->state == CONNECTION_NULL && node->lastCmdTime == 0)  // check if first connection
             		|| (node->state == CONNECTION_NULL && abs(timerclocks() - node->lastCmdTime) > 1740 && node->lastCmdTime != 0) // check if we have waited for 29 Minutes
             		|| (node->state > CONNECTION_NULL)) // check if we already have started the connection
         		{
					D(DBF_NET, "essayons de nous connecter à %s.... %ld", node->ip, (long)node->port);
					if(establish_connection(node))        //1 to connect to a server
            			{
               				if(node->hosttype == 1) 
								{
									sockfd = node->sockfd; //get_sockfd_element(hosts);
									downloadFilesReasked = 0;
									sharedListSent=0;
									D(DBF_NET, "nous sommes ici");
								}
               				node->lastCmdTime = timerclocks();
               				if(!connected) 
								{
									downloadFilesReasked = 0;
									connected = 1; 
									serverSockfd = sockfd;
									D(DBF_NET, "connecté!!!");
								}
							else D(DBF_NET, "en connexion");
            			}
            		if(node->hosttype == 1) timerwaits(2);
         		}
			if(abs(timerclocks() - node->lastCmdTime) > 1800 && abs(timerclocks() - node->lastCmdTime) < 2300) // We reinit connection each 29 minutes
				{
					node->state = CONNECTION_NULL;
		      		node->retry = 0;
					node->lastCmdTime = timerclocks();
				}
//			D(DBF_NET, "exit pollEdkEvents");
         	return;
      	}
   	else
      	{
			// printf("we are here : %ld %ld %ld ht:%ld\n",downloadFilesReasked,node->connection,abs(timerclocks() - node->lastCmdTime),node->hosttype);		
			if(node->hosttype == 1 && !node->connection) // we delay a bit before sending our shared files list
         		{								
					//D(DBF_NET, "node->hostype == 1 && !node->connection");
					// This Doesn't works as it should... Disable...SixK
					if(abs(timerclocks() - node->lastCmdTime) > 10 && node->sockfd == serverSockfd && downloadFilesReasked == 0)
						{
							/*struct DownloadList *dl = (struct DownloadList *)list_first(&dl_list);

							while(dl != NULL)
								{
									sprintf(cmd, "m %ld",(long)dl->nb);
									parse_cmd(0, cmd);
									D(DBF_NET, "dl->nb : %d",dl->nb);
									dl = (struct DownloadList *)node_next(&dl->node); 
								}*/
							downloadFilesReasked = 1;
							//D(DBF_NET, "exit");
						}				
        		}
      	}
   //if(testRecv(node->sockfd))
    //  {
    //  }
//maybe we should test if it's an incoming or outgoing connection . SixK
   	if(node->hosttype == 2 && node->connection == 0 && node->state >= CONNECTION_ACTIVE && node->state < CONNECTION_SLOT_ASKED && abs(timerclocks() - node->lastCmdTime) > 40)
      	{
         	node->state = CONNECTION_NULL;
         	node->retry = 0;
         	//conn->nbPartsRecv = 0;
         	D(DBF_NET, "!!!!!!!!!!!!!  ATTENTION!!! connexion fermé!!!");
         	CloseSocket(node->sockfd);
         	D(DBF_NET, "Delai imparti %lu %lu %lu pour la connexion", timerclocks() - node->lastCmdTime, timerclocks(), node->lastCmdTime);
      	}
}

///
/// ConnectionServer
BOOL ConnectionServer(struct Server *server)
{
 	int success = CS_NOTCONNECTED;
  	struct MA_GUIData *gui = &G->MA->GUI;
	
	ENTER();
	if(ConnectionIsOnline() == TRUE)
		{
			success = ed2k_login(server->IP, server->Port, server->Name);
			if(success == CS_CONNECTING) 
				{
					D(DBF_NET, "CS_CONNECTING");
					tr = (struct timerequest *)create_timer(UNIT_MICROHZ);
    				if(tr)
    					{
        					struct timerequest *tr2 = NULL;
											
            				tr2 = (struct timerequest *)create_timer(UNIT_MICROHZ);
            				if(tr2)
            					{
            						ULONG mytimersignal, sigs;
            						int truc = 0;
									struct Host *host;
																									
                					mytimersignal = (1L << tr2->tr_node.io_Message.mn_ReplyPort->mp_SigBit);
									host = (struct Host *)list_first(&G->list_hosts);
									while(G->ActiveConn != FALSE)
										{
											if(host == NULL)	host = (struct Host *)list_first(&G->list_hosts);	
											if(!truc)
												{																						
													startTimer(tr2);
													truc = 1;																			
												}															
											sigs = Wait(sigs | ed2k_sigmask | mytimersignal);
											if(sigs & ed2k_sigmask) G->ActiveConn = AsynchSocketEvents(host);
											if(sigs & mytimersignal || sigs & ed2k_sigmask)
												{
													if(sigs & mytimersignal) truc = 0;
													if(host != NULL) 
														{
															pollEdkEvents(host);	
															host = (struct Host *)node_next(&host->node);																			
														} 	                                                     
												}																
										}
									D(DBF_NET, "au revoir");
									delete_timer(tr2), tr2 = NULL;
								}
							delete_timer(tr), tr = NULL;
						}
				}
			if(SocketBase) 
				{
					CloseLibrary((struct Library *)SocketBase); 
					SocketBase = NULL; 
				}
			DoMethod(gui->TO_TOOLBAR, MUIM_Group_InitChange);
			DoMethod(gui->TO_TOOLBAR, MUIM_TheBar_SetAttr, 1, MUIA_TheBar_Attr_Hide, TRUE);
			DoMethod(gui->TO_TOOLBAR, MUIM_TheBar_SetAttr, 0, MUIA_TheBar_Attr_Hide, FALSE);
			DoMethod(gui->TO_TOOLBAR, MUIM_Group_ExitChange);
		}
	/*if(success == 0)	
		{
			tr = (struct timerequest *)create_timer(UNIT_MICROHZ);
    		if(tr)
    			{
        			struct timerequest *tr2 = NULL;
											
            		tr2 = (struct timerequest *)create_timer(UNIT_MICROHZ);
            		if(tr2)
            			{
            				ULONG mytimersignal, sigs;
            				int truc = 0;
							struct Host *host;
																									
                			mytimersignal = (1L << tr2->tr_node.io_Message.mn_ReplyPort->mp_SigBit);
							host = (struct Host *)list_first(&G->list_hosts);
							while(G->ActiveConn != FALSE)
								{
									if(host == NULL)	host = (struct Host *)list_first(&G->list_hosts);	
									if(!truc)
										{																						
											startTimer(tr2);
											truc = 1;																			
										}															
									sigs = Wait(sigs | ed2k_sigmask | mytimersignal);
									if(sigs & ed2k_sigmask) G->ActiveConn = AsynchSocketEvents(host);
									if(sigs & mytimersignal || sigs & ed2k_sigmask)
										{
											if(sigs & mytimersignal) truc = 0;
											if(host != NULL) 
												{
													pollEdkEvents(host);	
													host = (struct Host *)node_next(&host->node);																			
												}                                                      
										}																
								}
							D(DBF_NET, "au revoir");
							delete_timer(tr2), tr2 = NULL;
						}
					delete_timer(tr), tr = NULL;
				}
			DoMethod(gui->TO_TOOLBAR, MUIM_Group_InitChange);
			DoMethod(gui->TO_TOOLBAR, MUIM_TheBar_SetAttr, 1, MUIA_TheBar_Attr_Hide, TRUE);
			DoMethod(gui->TO_TOOLBAR, MUIM_TheBar_SetAttr, 0, MUIA_TheBar_Attr_Hide, FALSE);
			DoMethod(gui->TO_TOOLBAR, MUIM_Group_ExitChange);
			G->ActiveConn = FALSE;
  		}*/
	host_cleanup();
  	RETURN(success);
  	return(success);
}

///
