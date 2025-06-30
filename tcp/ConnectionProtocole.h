/***************************************************************************
Bourriquet MorphOS
2024-2025 digitally
 
***************************************************************************/
#ifndef CONNECTIONPROTOCOLE_H
#define CONNECTIONPROTOCOLE_H

#include "ServerMet.h"
#include "Lists.h"

struct DownloadList
{
   struct node node;
   char *Name;
   long Size;
   long Received;
   long Completed;
   int  Speed;
   char *Progress;
   int  Sources;
   char *Priority;
   char *State;
   char *Remaining_Time;
   char *LastDate;
   int nb;
   char *ID;
   long    gapStart[64];
   long    gapEnd[64];
   int     nbGaps;
   char tmpFileName[30];
   char *Hashlist;
   int  nbHash;
};

struct ResearchList
{
   struct  node node;
   int     status;
   char    *Name;
   int     Size;
   char    *Type;
   int     Sources;
   char    *FileID;
   char    *PartFileName;
   int     TransferedSize;
   long    GapStart[64];
   long    GapEnd[64];
   int     NbGaps;
   char    *IP;
   int     ClientPort;
   char    *Codec;
   long    gapStart[64];
   long    gapEnd[64];
   int     nbGaps;
};

extern struct timerequest *tr;

void host_cleanup(void);
long get_sockfd(void);
int goBind(long, unsigned short);
int goListen(long);
int ed2k_login(char *, int, char *);
//void pollEdkEvents(struct Host *);
long connect_client(char *, int, char *);
int IP_OK(char *);

#endif /* CONNECTIONPROTOCOLE_H */
