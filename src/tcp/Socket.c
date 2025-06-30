/***************************************************************************
 Bourriquet MorphOS
 2024-2025 digitally
 
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/filio.h>
#include <netdb.h>
#include <hardware/byteswap.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <netdb.h>

#include <assert.h>

#include <proto/alib.h>

#include "Extrasrc.h"

#include "Bourriquet.h"
#include "Debug.h"
#include "Global.h"
#include "Locale.h"
#include "tcp/ConnectionProtocole.h"
#include "tcp/Parser.h"
#include "tcp/Host.h"
#include "tcp/Socket.h"
#include "tcp/Connection.h"

struct timerequest *tr = NULL;
struct timeval currentval;
extern ULONG serverIP;
extern int serverPort;
extern struct list dl_list;
extern int connected;
ULONG eventmask;
extern LONG insockfd;
extern char userName[255];
extern unsigned char userHash[16];
extern ULONG myIp;
extern int userPort;

void delete_timer(struct timerequest *tr)
{
   struct MsgPort *tp;

   if(tr)
      {
         tp = tr->tr_node.io_Message.mn_ReplyPort;
         if(tp) DeletePort(tp);
         CloseDevice((struct IORequest *)tr);
         DeleteExtIO((struct IORequest *)tr);
      }
}

struct timerequest *create_timer(ULONG unit)
{
   struct MsgPort *timerport;
   struct timerequest *TimerIO = NULL;

   if(!(timerport = CreateMsgPort())) return(NULL);
   if(!(TimerIO = (struct timerequest *)CreateIORequest(timerport, sizeof(struct timerequest))))
      {
         DeleteMsgPort(timerport);
         return(NULL);
      }

   if(OpenDevice("timer.device", unit, &TimerIO->tr_node, 0) != 0)
      {
         delete_timer(TimerIO);
         return(NULL);
      }

   return(TimerIO);
}

void startTimer(struct timerequest *tr)
{
   tr->tr_time.tv_secs   = 0;
   tr->tr_time.tv_micro  = 4000;
   tr->tr_node.io_Command = TR_ADDREQUEST;
   SendIO((struct IORequest *)tr);
}

unsigned long timerclocks(void)
{
   tr->tr_node.io_Command = TR_GETSYSTIME;
   DoIO((struct IORequest *)tr);
   return(tr->tr_time.tv_secs);
}

void timerwaits(int nb_sec)
{
   unsigned long firstClock;

   firstClock = timerclocks();
   while(timerclocks() - firstClock < nb_sec) {}
}

unsigned long ClockUs(void)
{
   tr->tr_node.io_Command = TR_GETSYSTIME;
   DoIO((struct IORequest *)tr);
   return(tr->tr_time.tv_micro);
}

unsigned char *newID(void)
{
#define ID_LEN 16

   	unsigned int seed;
   	unsigned int i;
   	static unsigned char header[ID_LEN - 1];

	ENTER();
   	*(int *)header = ((int)time(0));
   	seed = header[2] * header[3];
   	srand(seed);
   	i = rand();
   	memcpy(header + 4, (char *)&i, 4);
   	i = rand();
   	memcpy(header + 8, (char *)&i, 4);
   	i = rand();
   	memcpy(header + 12, (char *)&i, 4);
   	//header[5]=14;    // emule signature
   	//header[14]=111;  // emule signature
	RETURN(header);
   	return(header);
}

struct sockaddr_in get_socket_address(char *ip, int port)
{
    struct hostent *he = NULL;
    long sockettype;
    struct sockaddr_in sin;

	assert(ip);
    if(!(he = gethostbyname(ip))) return(sin);
	else 
		{
    		sin.sin_family = AF_INET;
    		sockettype = SOCK_STREAM;
    		sin.sin_len = sizeof(sin);
    		memcpy(&sin.sin_addr, he->h_addr, he->h_length);
    		sin.sin_port = port;
    		D(DBF_NET, "Get Socket address : %s, Port :%ld", ip, (long)port);
    		return(sin);
		}
}

int get_connected(long sockfd, struct sockaddr *sa)
{
	int RetCode;
	
    assert(sockfd >= 0);
    assert(sa);
    RetCode = connect(sockfd, sa, sa->sa_len); 
	if(RetCode == -1) return(0);
    else 
		{
			D(DBF_NET, "Socket %ld connecté", sockfd);
    		return(1);
		}
}

int testRecv(long sock)
{
    fd_set in_set, out_set;
    struct timeval tm;
    int res = 0;
    fd_set nullfds;
   
    if(sock > 100000 || sock < 0) return(FALSE);
    FD_ZERO(&nullfds); // crash on mos ?
    tm.tv_secs = 0;
    tm.tv_micro = 22;
    FD_ZERO(&in_set);
    FD_ZERO(&out_set);
    FD_SET(sock, &in_set);
    FD_SET(sock, &out_set);
    res = select(sock + 1, &in_set, &out_set, &nullfds, &tm);
    if(res <= 0) 
		{
			D(DBF_NET, "select recv erreur : %ld", Errno());
			return(FALSE);
		}
    res = FD_ISSET(sock, &in_set);
    FD_CLR(sock, &in_set);
    return(res);
}

int testSend(long sock)
{
    fd_set out_set, in_set;
    struct timeval tm;
    int res = 1;
    fd_set nullfds;

    if(sock > 100000 || sock < 0) return(FALSE);
    FD_ZERO(&nullfds); // crash on mos ?
    tm.tv_secs = 0;
    tm.tv_micro = 22;
    FD_ZERO(&out_set);
    FD_ZERO(&in_set);
    FD_SET(sock, &out_set);
    FD_SET(sock, &in_set);
    res = select(sock + 1, &in_set, &out_set, &nullfds, &tm);
    if(res <= 0)
		{
	    	D(DBF_NET, "select send erreur : %ld", Errno());
	    	return(FALSE);
		}
    res = FD_ISSET(sock, &out_set);
    FD_CLR(sock, &out_set);
    return(res);
}

int waitSend(long sockfd)
{
   	int x = 0;
	
   	while(!testSend(sockfd))
      	{
         	x++;
         	if(x == TIMEOUT) return(0);
      	}
   return(1);
}

void waitRecv(long sockfd)
{
   	int x = 0;
   	while(!testRecv(sockfd))
      	{
         	x++;
         	if(x == TIMEOUT) 
				{
					D(DBF_NET, "recv timeout!");
				}
      }
}

/*********************************************************/
int get_response(long sockfd, char *buf, int max)
{
   int len = 0, le = 0;
   	int retry = 0;

   	while(len < max)
      	{
         	le = recv(sockfd, buf + len, 1, 0);
			// D(DBF_NET, "len : %ld le : %ld buf : 0x%x", len, le, buf[len]);
         	if(le == 0 || strstr(buf, "\r\n\r\n"))
            	{
               		buf[len] = 0;
               		retry++;
               		if(retry > 5) return(len);
               		Delay(1);
            	}
         	if(le < 0)
            	{
               		retry++;
               		if(retry > 50) return(len);
               		Delay(5);
               		le = 0;
            	}
         	if(le>0) 
				{
					retry = 0;
         			len += le;
				}
      	}
   	buf[max] = 0;
   	return(len);
}

void send_msg(long sockfd, unsigned char *msg, int len)
{
    long resp;
	
    resp = send(sockfd, msg, (long)len, 0);
    if(resp != (long)len) D(DBF_NET, "send: error %ld", Errno());
    else D(DBF_NET, "sending message to : %ld", sockfd);
}

void sendPacket(long sockfd, unsigned char *buf, int buflen)
{
    char *buf2;

    buf2 = AllocVec(65000, MEMF_CLEAR);
    if(buf2)
		{
	    	buf2[0] = 0xe3;
	    	*(unsigned int *)&buf2[1] = SWAPLONG(buflen);
	    	memcpy(buf2 + 5, buf, buflen);
	    	if(waitSend(sockfd)) send_msg(sockfd, buf2, buflen + 5);
		    else D(DBF_NET, "cmd not sent, timeout !!");
		    FreeVec(buf2);
        }
    else D(DBF_NET, "Pas assez de mémoire");
}

int establish_connection(struct Host *host)
{
    LONG sockfd;
    struct sockaddr_in sin;
    struct sockaddr *sa; // = NULL;
    long n;
    long value;

    D(DBF_NET, "fonction establish_connection()");
    if(host == NULL) return(0);
    if(host->retry > MAX_RETRY) return(0);
    else host->retry += 1;
    D(DBF_NET, "refaire %ld ... état %ld", (long)host->retry, (long)host->state);
    switch(host->state)
        {
	    	case CONNECTION_NULL: // 0
				if((sockfd = get_sockfd()) < 0) return(0);
	        	host->state = CONNECTION_GET_SOCKFD;
	        	host->sockfd = sockfd;
	        	value = FD_CONNECT | FD_ACCEPT | FD_OOB | FD_READ | FD_WRITE | FD_CLOSE | FD_ERROR;
	        	setsockopt(host->sockfd, SOL_SOCKET, SO_EVENTMASK, &value, sizeof(LONG));
				D(DBF_NET, "établir une connexion de départ (CONNECTION_NULL) ... %s", host->ip);
	        	break;
	    	case CONNECTION_GET_SOCKFD: // 2
	        	sin = get_socket_address(host->ip, host->port);
	        	if(!sin.sin_len) return(0);
	        	host->state = CONNECTION_GET_IP;
	        	host->sa = malloc(sizeof(struct sockaddr));
	        	if(host->sa != NULL)
	            	{
						memset(host->sa, 0, sizeof(struct sockaddr));
		        		sa = (struct sockaddr *)&sin;
		        		host->sa->sa_len = sa->sa_len;
		        		host->sa->sa_family = sa->sa_family;
		        		memcpy(host->sa->sa_data, sa->sa_data, 14);
		        		n = 1;
		        		IoctlSocket(host->sockfd, FIONBIO, (char*)&n);    //non bloquant
		        		IoctlSocket(host->sockfd, FIOASYNC, (char*)&n);   // asynchrone
	           	 	}
				D(DBF_NET, "établir une connexion (obtenir CONNECTION_GET_SOCKFD) ... %s", host->ip);
	        	break;
	    	case CONNECTION_GET_IP: // 1
	            if(get_connected(host->sockfd, host->sa) == 0)
	                {
			    		if(Errno() != 36) D(DBF_NET, "Errno != 36");
					    else
							{
				    			D(DBF_NET, "Errno == 36");
				    			//testSend(host->sockfd);
				    			//testRecv(host->sockfd);
				    			Delay(30);
				    			if(get_connected(host->sockfd, host->sa) == 0) D(DBF_NET, "Errno == %ld", (long)Errno());
				    			else D(DBF_NET, "OK CONNECTE !!!");
							}
	                }
	        	host->state = CONNECTION_GET_CONNECTED;
				D(DBF_NET, "établir une connexion (obtenir CONNECTION_GET_IP) ... %s", host->ip);
	        	return(1);
	        	break;
	    }
    return(0);
}

int write_edk_tags(unsigned char *buf, int tag)
{
   int offset = 0;

   switch(tag)
      {
         case CT_NAME:
            buf[offset] = 0x02;
            buf[offset + 1] = 0x01;
            buf[offset + 2] = 0x00;
            buf[offset + 3] = 0x01;
            offset += 4;
            *(unsigned short *)&buf[offset] = SWAPWORD(strlen(userName));
            offset += 2;
            memcpy(buf + offset, userName, strlen(userName));
            offset += strlen(userName);
            break;
         case CT_PORT:
            buf[offset] = 0x03;
            buf[offset + 1] = 0x01;
            buf[offset + 2] = 0x00;
            buf[offset + 3] = 0x0f;
            offset += 4;
            *(unsigned int *)&buf[offset] = SWAPLONG(userPort);
            offset += 4;
            break;
         case CT_VERSION:
            buf[offset] = 0x03;
            buf[offset + 1] = 0x01;
            buf[offset + 2] = 0x00;
            buf[offset + 3] = 0x11;
            offset += 4;
            buf[offset] = 0x3C;
            buf[offset + 1] = 0x00;
            buf[offset + 2] = 0x00;
            buf[offset + 3] = 0x00;
            offset += 4;
            break;
      }
   return(offset);
}

int handshake(long sockfd, int type)
{
    char *buf;
    int offset = 0;

    assert(sockfd >= 0);
	buf = AllocVec(65000, MEMF_CLEAR);
    if(buf)
        {
	    	if(type == 1)  // connect to a serveur
	        	{
					D(DBF_NET, "connexion au serveur ...");
	            	buf[0] = 0x01;
	            	offset = 1;
	            	memcpy(buf + offset, userHash, 16);
	            	offset += 16;
	            	*(unsigned int *)&buf[offset] = myIp;
	            	offset += 4;
	            	*(unsigned short *)&buf[offset] = SWAPWORD(userPort);
	            	offset += 2;
	            	buf[offset] = 0x03;
	            	buf[offset + 1] = 0x00;
	            	buf[offset + 2] = 0x00;
	            	buf[offset + 3] = 0x00;
	            	offset += 4;
	            	offset += write_edk_tags(buf + offset, 0x01); // write the name of the user in buf
	            	offset += write_edk_tags(buf + offset, 0x11); // write the version number
	            	offset += write_edk_tags(buf + offset, 0x0f); // specify the port of Edk client (4662)
					D(DBF_NET, "ma carte d'identité pour le serveur : %ld", sockfd);
	            	sendPacket(sockfd, buf, offset);
	        	}
	    	else  //connect to a host
	        	{
					D(DBF_NET, "connexion à un hôte ....");
	            	buf[0] = 0x01;
	            	buf[1] = 0x10;
	            	offset = 2;
	            	memcpy(buf + offset, userHash, 16);
	            	offset += 16;
	            	*(unsigned int *)&buf[offset] = myIp;
	            	offset += 4;
	            	*(unsigned short *)&buf[offset] = SWAPWORD(userPort);
	            	offset += 2;
	            	buf[offset] = 0x03;
	            	buf[offset + 1] = 0x00;
	            	buf[offset + 2] = 0x00;
	            	buf[offset + 3] = 0x00;
	            	offset += 4;
	            	offset += write_edk_tags(buf + offset, 0x01); // write the name of the user in buf
	            	offset += write_edk_tags(buf + offset, 0x11); // write the version number
	            	offset += write_edk_tags(buf + offset, 0x0f); // specify the port of Edk client (4662)
	            	*(unsigned int *)&buf[offset] = serverIP;
	            	offset += 4;
	            	*(unsigned short *)&buf[offset] = SWAPWORD(serverPort);
	            	offset += 2;
					D(DBF_NET, "ma carte d'identité pour l'hôte : %ld", sockfd);
	            	sendPacket(sockfd, buf, offset);
	        	}
	    	FreeVec(buf);
	    	return(1);
        }
    else
		{
	    	D(DBF_NET, "Pas assez de mémoire");
	    	return(0);
		}
}

int acceptHandler(long sockfd)
{
    struct sockaddr_in sa;
    long s;
    struct in_addr ia;
    long value;
    int n;
    long j;
    struct Host *host = NULL;

    j = sizeof(struct sockaddr_in);
    if((s = accept(sockfd, (struct sockaddr *)&sa, &j)) < 0) return(FALSE);
	else 
		{
    		D(DBF_NET, "FD_ACCEPT sockfd... %ld... s %ld", (long)sockfd, s);
    		ia.s_addr = sa.sin_addr.s_addr;
			if((host = calloc(sizeof(struct Host), 1)) != NULL)
        		{
					host->ip = (char *)mystrdup(Inet_NtoA(ia.s_addr));
					D(DBF_NET, "host->ip %s", host->ip);
	    			host->port = sa.sin_port;
	    			host->state = CONNECTION_IN_HANDLE;
	    			host->retry = 0;
	    			host->sockfd = s;
	    			host->hosttype = 2;
	    			host->connection = 1; // incoming connection
					host->nom = (char *)mystrdup("? noname ?");
					D(DBF_NET, "host->nom %s", host->nom);
	    			host->queueRank = 0;
					list_insert_tail(&G->list_hosts, &host->node);
				}		
	    	n = 1;
	    	IoctlSocket(s, FIONBIO, (char*)&n);    //non bloquant
	    	IoctlSocket(s, FIOASYNC, (char*)&n);   // asynchrone
	    	value = FD_CONNECT | FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE | FD_ERROR;
	    	setsockopt(s, SOL_SOCKET, SO_EVENTMASK, &value, sizeof(LONG));
			//if(!connected) 
			//	{								
				 	readHandler(host);
			//		closeHandler(host);
			//	}
			D(DBF_NET, "we ascape FD_ACCEPT");
        	return(s);  
      	}
   	return(0);
}

int connectHandler(struct Host *host)
{	
   	D(DBF_NET, "FD_CONNECT");
	if(handshake(host->sockfd, host->hosttype)) host->state = CONNECTION_IN_HANDLE;
   	return(TRUE);
}

void readHandler(struct Host *host)
{
   	int resp = 0;
   	int msg_len = 0;
   	unsigned char *buf = NULL;
	
   	D(DBF_NET, "FD_READ : %ld", host->sockfd);
   	if(testRecv(host->sockfd))
   		{
         	buf = AllocVec(65000, MEMF_CLEAR);
         	if(buf)
         		{					
					while((resp = get_response(host->sockfd, buf, 1)) > 0)
						{
							if(buf[0] == 0xe3)
               					{								 
                    				resp = get_response(host->sockfd, buf, 4);
                     				msg_len = SWAPLONG(*(unsigned int *)&buf[0]);
                     				waitRecv(host->sockfd);                           									
                     				D(DBF_NET, "msg len : %u", msg_len);
									if(msg_len < 65000 && msg_len > 0)
										{   
											resp = get_response(host->sockfd, buf, msg_len);
											D(DBF_NET, "msglen : %ld Buf : %s resp : %ld",msg_len, buf, resp);
											printf("%s\n", buf);
											if(buf[0] == 0x6b) D(DBF_NET, "problem : msg_len :%ld msg : %x %x %x %x %x %x %x %x", msg_len , buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
											else
												{                          
                      								parse_host_reply(host, buf);
													host->lastCmdTime = timerclocks();
													if(resp == msg_len) return;
												}
										}
									else D(DBF_NET, "msg too long to be retrieved msglen : %u", msg_len);
									D(DBF_NET, "message received !");
									Delay(30);
               					}   
            			}
			       	FreeVec(buf), buf = NULL;
       			}
			else D(DBF_NET, "NOMORERAM!");
   		}
	if(host->hosttype == 2 && host->connection == 0 && host->state == CONNECTION_SLOT_OPEN && host->nbPartsRecv >= 17)
   		{
			unsigned char buf2[256];
			//struct DownloadList *dl = (struct DownloadList *)list_first(&dl_list);

      		D(DBF_NET, "host->hosttype == 2 && host->connection == 0 && host->state == CONNECTION_SLOT_OPEN");
      		/*while(strncmp(node->ID, dl->ID, 16) != 0 && dl != NULL) 
				{
					dl = (struct DownloadList *)node_next(&dl->node); 
					D(DBF_NET, "And there FileID : %s!", dl->ID);
				}
      		D(DBF_NET, "!!!!!!!!!!!!!!!!!!!!!!!!! reqsize: %i nbParts:%i s: %li e: %li", dl->gapEnd[0] - dl->gapStart[0], node->nbPartsRecv, dl->gapStart[0], dl->gapEnd[0]);
      		if(strncmp(node->ID, dl->ID, 16) == 0 && dl != NULL)
      			{
         			sprintf(buf2,"d %i 0", dl->nb);
         			parse_cmd(node, buf2);
					node->nbPartsRecv=0;
					D(DBF_NET, "Asked new block !");
      			}*/
   		}
}

void writeHandler(struct Host *node)
{
   	/*static unsigned char buf[65000];
   	char fileName[255];
   	BPTR fp;
   	int gapSize = 0;
   	int offset = 0;*/

   	/*D(DBF_NET, "FD_WRITE : %ld", node->sockfd);
   	D(DBF_NET, "node->hostType == %d && node->connectionType == %d", node->hosttype, node->connection);
   	if(node->hosttype == 2 && node->connection == 0)  // client + outgoing connection
      	{
         	D(DBF_NET, "node->hostType == 2 && node->connectionType == 0");
         	if(node->state == CONNECTION_ACTIVE)  D(DBF_NET, "connection_active");
      	}*/
	/*if(node->hosttype == 2 && node->connection == 0 && node->state == CONNECTION_SLOT_OPEN && node->nbPartsRecv >= 17)
   		{
			unsigned char buf2[256];
			struct DownloadList *dl = (struct DownloadList *)list_first(&dl_list);

      		D(DBF_NET, "node->hosttype == 2 && node->connection == 0 && node->state == CONNECTION_SLOT_OPEN");
      		while(strncmp(node->ID, dl->ID, 16) != 0 && dl != NULL) 
				{
					dl = (struct DownloadList *)node_next(&dl->node); 
					D(DBF_NET, "And there FileID : %s!", dl->ID);
				}
      		D(DBF_NET, "!!!!!!!!!!!!!!!!!!!!!!!!! reqsize: %i nbParts:%i s: %li e: %li", dl->gapEnd[0] - dl->gapStart[0], node->nbPartsRecv, dl->gapStart[0], dl->gapEnd[0]);
      		if(strncmp(node->ID, dl->ID, 16) == 0 && dl != NULL)
      			{
         			sprintf(buf2,"d %i 0", dl->nb);
         			parse_cmd(node, buf2);
					node->nbPartsRecv=0;
					D(DBF_NET, "Asked new block !");
      			}
   		}
   	if(node->hosttype == 2 && node->connection == 1)  // client + incoming connection
      	{
         	D(DBF_NET, "p->hostType == 2 && p->connectionType == 1");
         	if(node->state == CONNECTION_UPLOAD_STARTED && node->nbGaps)
         		{
            		struct SharedList *sl = (struct SharedList *)list_first(&sh_list);
					
					D(DBF_NET, "node->state == CONNECTION_UPLOAD_STARTED && node->nbGaps");
					while(strncmp(node->ID, sl->FileID, 16) != 0 && sl != NULL) 
						{
							sl = (struct SharedList *)node_next(&sl->node); 
							D(DBF_NET, "And there FileID : %s!", dl->ID);
						}			
					if(strncmp(node->ID, sl->FileID, 16) == 0 && sl != NULL)
						{	
            				sprintf(fileName, "%s%s", C->SharedDir, sl->FileName);
            				fp = Open(fileName, MODE_OLDFILE);
            				if(fp)
            					{
         	   						gapSize = node->gapEnd[0] - node->gapStart[0];
              						if(gapSize > MAXPARTSEND) gapSize = MAXPARTSEND;
               						buf[0] = 0x46;
               						offset = 1;
               						memcpy(buf + offset, hash_hex2bin(sl->FileID), 16);
               						offset+=16;
               						*(unsigned int *)&buf[offset] = SWAPLONG(node->gapStart[0]);
               						offset += 4;
               						*(unsigned int *)&buf[offset] = SWAPLONG(node->gapStart[0] + gapSize);
               						offset += 4;
               						Seek(fp, node->gapStart[0], OFFSET_BEGINNING);
                					// fseek(fp, node->gapStart[0], SEEK_SET);
               						FRead(fp, buf + offset, 1, gapSize);
                					// fread(buf + offset, 1, gapSize, fp);
               						offset += gapSize;
               						Close(fp);
               						sendPacket(node->sockfd, buf, offset);
									D(DBF_NET, "Start : %ld , End : %ld size :%ld",node->gapStart[0], node->gapEnd[0],gapSize);
 					              	uploadGapHandle(node, node->gapStart[0], node->gapStart[0] + gapSize);
									Delay(100);
									if(node->nbGaps == 0)
										{
											D(DBF_NET, "file completed");
											node->state = CONNECTION_NULL;
											closeHandler(node);
										}
								}
            			}
         		}
      	}*/
 }
  
int closeHandler(struct Host *host)
{
	D(DBF_NET, "Closing socket : %ld for IP : %s hosttype : %ld", host->sockfd, host->ip, (long)host->hosttype);
   	D(DBF_NET, "FD_CLOSE");
   	CloseSocket(host->sockfd);   // Disable to see if we still loose hosts in hostlist
	D(DBF_NET, "Closed socket : %ld for IP : %s", host->sockfd, host->ip);
	host->sockfd= 0;
   	host->state = CONNECTION_NULL;
   	host->retry = 0;
    if(host->connection == 1)
   		{
     		if(host->hosttype == 2) D(DBF_NET, "supprimer");
   		}
	else host->lastCmdTime = timerclocks();
	if(host->hosttype == 1) // only do this for server
		{
			D(DBF_NET, "Closing connection to server");		
      		return(0);
		}
	else
		{
			D(DBF_NET, "Or here ?");
			return(1);
		}
}

/*********************************************************************************************************/
int errorHandler(struct Host *host)
{
   	char buffer[SIZE_DEFAULT];

	D(DBF_NET, "Error on socket : %ld for IP : %s  nodetype : %ld", host->sockfd, host->ip, (long)host->hosttype);
   	getErrno(Errno());
    D(DBF_NET, "FD_ERROR");
    CloseSocket(host->sockfd);  // SixK . Disable to see if we still loose hosts in host list
	D(DBF_NET, "Error on socket : %ld for IP : %s", host->sockfd, host->ip);
	host->sockfd= 0;
    host->state = CONNECTION_NULL;
	if(host->hosttype == 1) // only do this for server
		{
			D(DBF_NET, "Error with server");
         	return(0);
		}
	else return(1);
}

/********************************************************************************************************/
BOOL AsynchSocketEvents(struct Host *host)
{
   	ULONG mysock;
   	BOOL events = TRUE;

	D(DBF_NET, "AsynchSocketEvents");
   	while((mysock = GetSocketEvents((ULONG *)&eventmask)) != -1)
      	{		
			struct Host *hnode = (struct Host *)list_first(&G->list_hosts);						
			
			D(DBF_NET, "mysock : %ld eventmask : 0x%x", mysock, eventmask);
			if(mysock > 0)
				{		
					ULONG count = 0;
					
					D(DBF_NET, "mysock ... %ld", mysock);		
					while(hnode->sockfd != mysock && hnode != NULL) 
						{
							hnode = (struct Host *)node_next(&hnode->node);							
						}
					if(hnode == NULL) 
						{
							D(DBF_NET, "PB should never happen !");										
				   			return(TRUE); 		
						}
					D(DBF_NET, "sortie boucle mysock ... %ld", mysock);
				}
			if(eventmask & FD_ACCEPT)
            	{
					events = acceptHandler(insockfd);	
               		return(events);
            	}
			if(eventmask & FD_CONNECT)
	            {
    	           	events = connectHandler(hnode);          // Connect handler, performs handshake and establishes pending connections
    	           	return(events);
    	        }
			if(eventmask & FD_OOB)
				{
					D(DBF_NET, "WARNING !!! OOB not handle");
				}
         	if(eventmask & FD_READ)
	            {
	               	readHandler(hnode); // Read handler, acts upon incoming data
	                return(TRUE);
	            }
         	if(eventmask & FD_WRITE)
	            {
    	           	writeHandler(hnode); // Write handler, send data waiting to be sent
    	           	return(TRUE);
    	        }
			if(eventmask & FD_ERROR)
	            {
	               	events = errorHandler(hnode);
	               	return(events);
	            }
        	if(eventmask & FD_CLOSE)
	            {
					if(hnode->sockfd == 0) return(TRUE); // Sorry, we try to avoid a crash here when an error occure. We need to fix it later. SixK
	               	events = closeHandler(hnode);   // Close/Error handler, removes connections
	               	return(events);
	            }     
			  	         	
      }
  	return(TRUE);
}

void getErrno(int myErrno)
{
   switch(myErrno)
      {
         case EPERM           :     D(DBF_NET, "          1               /* Operation not permitted */   "); break;
         case ENOENT          :     D(DBF_NET, "          2               /* No such file or directory */   "); break;
         case ESRCH           :     D(DBF_NET, "          3               /* No such process */   "); break;
         case EINTR           :     D(DBF_NET, "          4               /* Interrupted system call */   "); break;
         case EIO             :     D(DBF_NET, "          5               /* Input/output error */   "); break;
         case ENXIO           :     D(DBF_NET, "          6               /* Device not configured */   "); break;
         case E2BIG           :     D(DBF_NET, "          7               /* Argument list too long */   "); break;
         case ENOEXEC         :     D(DBF_NET, "          8               /* Exec format error */   "); break;
         case EBADF           :     D(DBF_NET, "          9               /* Bad file descriptor */   "); break;
         case ECHILD          :     D(DBF_NET, "          10              /* No child processes */   "); break;
         case EDEADLK         :     D(DBF_NET, "          11              /* Resource deadlock avoided */   "); break;
         case ENOMEM          :     D(DBF_NET, "          12              /* Cannot allocate memory */   "); break;
         case EACCES          :     D(DBF_NET, "          13              /* Permission denied */   "); break;
         case EFAULT          :     D(DBF_NET, "          14              /* Bad address */   "); break;
         case ENOTBLK         :     D(DBF_NET, "          15              /* Block device required */   "); break;
         case EBUSY           :     D(DBF_NET, "          16              /* Device busy */   "); break;
         case EEXIST          :     D(DBF_NET, "          17              /* File exists */   "); break;
         case EXDEV           :     D(DBF_NET, "          18              /* Cross-device link */   "); break;
         case ENODEV          :     D(DBF_NET, "          19              /* Operation not supported by device */   "); break;
         case ENOTDIR         :     D(DBF_NET, "          20              /* Not a directory */   "); break;
         case EISDIR          :     D(DBF_NET, "          21              /* Is a directory */   "); break;
         case EINVAL          :     D(DBF_NET, "          22              /* Invalid argument */   "); break;
         case ENFILE          :     D(DBF_NET, "          23              /* Too many open files in system */   "); break;
         case EMFILE          :     D(DBF_NET, "          24              /* Too many open files */   "); break;
         case ENOTTY          :     D(DBF_NET, "          25              /* Inappropriate ioctl for device */   "); break;
         case ETXTBSY         :     D(DBF_NET, "          26              /* Text file busy */   "); break;
         case EFBIG           :     D(DBF_NET, "          27              /* File too large */   "); break;
         case ENOSPC          :     D(DBF_NET, "          28              /* No space left on device */   "); break;
         case ESPIPE          :     D(DBF_NET, "          29              /* Illegal seek */   "); break;
         case EROFS           :     D(DBF_NET, "          30              /* Read-only file system */   "); break;
         case EMLINK          :     D(DBF_NET, "          31              /* Too many links */   "); break;
         case EPIPE           :     D(DBF_NET, "          32              /* Broken pipe */   "); break;
         case EDOM            :     D(DBF_NET, "          33              /* Numerical argument out of domain */   "); break;
         case ERANGE          :     D(DBF_NET, "          34              /* Result too large */   "); break;
         case EAGAIN          :     D(DBF_NET, "          35              /* Resource temporarily unavailable */   ");
                                    D(DBF_NET, "        EAGAIN            /* Operation would block */   "); break;
         case EINPROGRESS     :     D(DBF_NET, "          36              /* Operation now in progress */   "); break;
         case EALREADY        :     D(DBF_NET, "          37              /* Operation already in progress */   "); break;
         case ENOTSOCK        :     D(DBF_NET, "          38              /* Socket operation on non-socket */   "); break;
         case EDESTADDRREQ    :     D(DBF_NET, "          39              /* Destination address required */   "); break;
         case EMSGSIZE        :     D(DBF_NET, "          40              /* Message too long */   "); break;
         case EPROTOTYPE      :     D(DBF_NET, "          41              /* Protocol wrong type for socket */   "); break;
         case ENOPROTOOPT     :     D(DBF_NET, "          42              /* Protocol not available */   "); break;
         case EPROTONOSUPPORT :     D(DBF_NET, "          43              /* Protocol not supported */   "); break;
         case ESOCKTNOSUPPORT :     D(DBF_NET, "          44              /* Socket type not supported */   "); break;
         case EOPNOTSUPP      :     D(DBF_NET, "          45              /* Operation not supported */   "); break;
         case EPFNOSUPPORT    :     D(DBF_NET, "          46              /* Protocol family not supported */   "); break;
         case EAFNOSUPPORT    :     D(DBF_NET, "          47              /* Address family not supported by protocol family */   "); break;
         case EADDRINUSE      :     D(DBF_NET, "          48              /* Address already in use */   "); break;
         case EADDRNOTAVAIL   :     D(DBF_NET, "          49              /* Can't assign requested address */   "); break;
         case ENETDOWN        :     D(DBF_NET, "          50              /* Network is down */   "); break;
         case ENETUNREACH     :     D(DBF_NET, "          51              /* Network is unreachable */   "); break;
         case ENETRESET       :     D(DBF_NET, "          52              /* Network dropped connection on reset */   "); break;
         case ECONNABORTED    :     D(DBF_NET, "          53              /* Software caused connection abort */   "); break;
         case ECONNRESET      :     D(DBF_NET, "          54              /* Connection reset by peer */   "); break;
         case ENOBUFS         :     D(DBF_NET, "          55              /* No buffer space available */   "); break;
         case EISCONN         :     D(DBF_NET, "          56              /* Socket is already connected */   "); break;
         case ENOTCONN        :     D(DBF_NET, "          57              /* Socket is not connected */   "); break;
         case ESHUTDOWN       :     D(DBF_NET, "          58              /* Can't send after socket shutdown */   "); break;
         case ETOOMANYREFS    :     D(DBF_NET, "          59              /* Too many references:  can't splice */   "); break;
         case ETIMEDOUT       :     D(DBF_NET, "          60              /* Operation timed out */   "); break;
         case ECONNREFUSED    :     D(DBF_NET, "          61              /* Connection refused */   "); break;
         case ELOOP           :     D(DBF_NET, "          62              /* Too many levels of symbolic links */   "); break;
         case ENAMETOOLONG    :     D(DBF_NET, "          63              /* File name too long */   "); break;
         case EHOSTDOWN       :     D(DBF_NET, "          64              /* Host is down */   "); break;
         case EHOSTUNREACH    :     D(DBF_NET, "          65              /* No route to host */   "); break;
         case ENOTEMPTY       :     D(DBF_NET, "          66              /* Directory not empty */   "); break;
         case EPROCLIM        :     D(DBF_NET, "          67              /* Too many processes */   "); break;
         case EUSERS          :     D(DBF_NET, "          68              /* Too many users */   "); break;
         case EDQUOT          :     D(DBF_NET, "          69              /* Disc quota exceeded */   "); break;
         case ESTALE          :     D(DBF_NET, "          70              /* Stale NFS file handle */   "); break;
         case EREMOTE         :     D(DBF_NET, "          71              /* Too many levels of remote in path */   "); break;
         case EBADRPC         :     D(DBF_NET, "          72              /* RPC struct is bad */   "); break;
         case ERPCMISMATCH    :     D(DBF_NET, "          73              /* RPC version wrong */   "); break;
         case EPROGUNAVAIL    :     D(DBF_NET, "          74              /* RPC prog. not avail */   "); break;
         case EPROGMISMATCH   :     D(DBF_NET, "          75              /* Program version wrong */   "); break;
         case EPROCUNAVAIL    :     D(DBF_NET, "          76              /* Bad procedure for program */   "); break;
         case ENOLCK          :     D(DBF_NET, "          77              /* No locks available */   "); break;
         case ENOSYS          :     D(DBF_NET, "          78              /* Function not implemented */   "); break;
         case EFTYPE          :     D(DBF_NET, "          79              /* Inappropriate file type or format */   "); break;
      }
}
