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
#include "Share.h"
#include "ShareList.h"
#include "tcp/Host.h"
#include "tcp/ConnectionProtocole.h"
#include "tcp/Connection.h"
#include "tcp/Parser.h"

//extern struct list dl_list;
extern LONG serverSockfd;
extern LONG insockfd;
extern char userName[255];
extern unsigned char userHash[16];
extern ULONG myIp;
extern int userPort;

/**************************************************************************/
const unsigned char *hash_bin2hex(const unsigned char *hash)
{
   const unsigned char hexdigits[] = "0123456789abcdef";
   static unsigned char m_hash_str[33];
   int j;

   for(j = 0; j < 16; j++)
      {
         m_hash_str[(j << 1)] = hexdigits[(((hash[j]) & 0xf0) >> 4)];
         m_hash_str[(j << 1) + 1] = hexdigits[(((hash[j]) & 0x0f))];
      }
   return(m_hash_str);
}

/*************************************************************/
const unsigned char *hash_hex2bin(const char hash_str[35])
{
   char hexnum[3] = { 0x00, 0x00, 0x00 };
   static unsigned char m_hash[16];
   int i;

   for(i = 0; i < 16; i++)
      {
         strncpy(hexnum, hash_str + (i * 2), 2);
         m_hash[i] = (char)strtol(hexnum, NULL, 16);
      }
   return(m_hash);
}

/*************************************************************************/
static int LoginEDKTags(unsigned char *buf, struct Host *host)
{
   	long datalen;
   	int offset = 0;
	static char nom[SIZE_LARGE], filetype[SIZE_DEFAULT], partfilename[SIZE_LARGE];
	
   	switch(buf[offset++])
      	{
         	case 0x02:
            	switch(datalen = SWAPWORD(*(unsigned short *)&buf[offset]))
               		{
                  		case 1:
                     		offset += 2;
                     		switch(buf[offset++])
                        		{
                           			case FT_FILENAME:  // name
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			memcpy(nom, buf + offset, datalen);
                              			host->nom = (char *)mystrdup(nom);
                              			host->nom[datalen] = 0;
                              			offset += datalen;
                              			break;
                           			case FT_FILETYPE:  // filetype (audio, pro ...)
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			//memcpy(filetype, buf + offset, datalen);
                              			//host->Type = (char *)mystrdup(filetype);
                              			//host->Type[datalen] = 0;
                              			offset += datalen;
                              			break;
                           			case FT_FILEFORMAT:  // fileextension (mp3, ...)
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			offset += datalen;
                              			break;
                           			case FT_PARTFILENAME:      // tmp file name
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			//memcpy(partfilename, buf + offset, datalen);
                              			//host->PartFileName = (char *)mystrdup(partfilename);
                              			//host->PartFileName[datalen] = 0;
                              			offset += datalen;
                              			break;
                           			case 0x97:
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			offset += datalen;
                              			break;
                           			default:
                              			D(DBF_NET, "cas 0x02/1 j'ai oublié d'implémenter le tag: %x", buf[offset-1]);
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			offset += datalen;
                              			break;
                        		}
                     			break;
                  		case 2:
                     		offset += 2;
                     		break;
                  		default:
                    		//D(DBF_NET, "0x02--> oups, l'implementation du tag (%02x) n'est pas encore fait\n", buf[offset - 1]);
                    		offset += 2;
                    		offset += datalen;
		     				//second part of the tag
		    	 			datalen = SWAPWORD(*(unsigned short *) &buf[offset]);
		     				offset += 2;
                    		offset += datalen;
                    		break;
               		}
            	break;
         	case 0x03:
            	switch(datalen = SWAPWORD(*(unsigned short *)&buf[offset]))
               		{
                  		case 1:
                     		offset += 2;
                     		switch(buf[offset++])
                        		{
                           			case FT_FILESIZE:  // filesize
                              			//host->Size = SWAPLONG(*(unsigned int *)&buf[offset]);
                              			D(DBF_NET, "size %d", SWAPLONG(*(unsigned int *)&buf[offset]));
                              			offset += 4;
                              			break;
                           			case FT_LASTSEENCOMPLETE:  // last seen complete
                              			offset += 4;
                              			break;
                           			case FT_TRANSFERED:      // size already transfered
                              			//host->TransferedSize = SWAPLONG(*(unsigned int *)&buf[offset]);
                              			D(DBF_NET, "size transfered %d", SWAPLONG(*(unsigned int *)&buf[offset]));
                              			offset += 4;
                              			break;
                           			case FT_OLDDLPRIORITY:      // priority of the file, not interesting for the moment
                              			offset += 4;
                              			break;
                           			case FT_STATUS:  // Status of the file... not interesting for the moment
                              			offset += 4;
                              			break;
                           			case FT_SOURCES:  // nb sources ?
                              			//host->Sources = SWAPLONG(*(unsigned int *)&buf[offset]);
                              			D(DBF_NET, "sources %d", SWAPLONG(*(unsigned int *)&buf[offset]));
                              			offset += 4;
                              			break;
                           			case FT_PERMISSIONS:  // permissions bits, never seen this tag...
                              			offset += 4;
                              			break;
                           			case FT_COMPLETE_SOURCES:
                              			offset += 4;
                              			break;
									case 0x11: // Version
										D(DBF_NET, "Emule Version : %x %x %x %x %x %x %x %x", buf[offset],buf[offset+1],buf[offset+2],buf[offset+3],buf[offset+4],buf[offset+5],buf[offset+6],buf[offset+7]);
										break;
                           			default:
                              			D(DBF_NET, "cas 0x03/1 j'ai oublié d'implémenter ce tag: 0x%x", buf[offset-1]);
                              			offset += 4; // we skip 4 bytes cause we don't know the current tag
                              			break;
                        		}
                     		break;
                  		case 2:
                     		offset += 2;
                     		switch(buf[offset++])
                        		{
                           			case FT_GAPSTART:  //Gap Start
                              			//host->gapStart[buf[offset++] - 0x30] = SWAPLONG(*(unsigned int *)&buf[offset]);
                              			D(DBF_NET, "gapstart %d", SWAPLONG(*(unsigned int *)&buf[offset]));
                              			offset += 4;
                              			// 0x30 = 0 in ASCII, but I don't know how edk do when nb gaps is > to 9
                              			// I think, we will have to change something there later
                              			//host->nbGaps++;
                              			break;
                           			case FT_GAPEND:      //Gap End
                              			//host->gapEnd[buf[offset++] - 0x30] = SWAPLONG(*(unsigned int *)&buf[offset]);
                              			D(DBF_NET, "gapend %d", SWAPLONG(*(unsigned int *)&buf[offset]));
                              			offset += 4;
                              			break;
                        		}
                     		break;
                  		default:
                     		offset += 2;
                     		offset += datalen;        // tagname
                     		offset += 4;                      // value
                     		break;
               		}
            	break;
			default:
				D(DBF_NET, "probleme here we received Tag Class : 0x%x\n", buf[offset - 1]);
				break;
      	}
   	return(offset);
}

/*int ParseEDKTags_Download(unsigned char *buf, struct DownloadList *p)
{
   	LONG datalen;
   	int offset = 0;
   	static char nom[SIZE_LARGE], filetype[SIZE_DEFAULT], partfilename[SIZE_LARGE], codec[SIZE_DEFAULT];

   	switch(buf[offset++])
      	{
         	case 0x02:
            	switch(datalen = SWAPWORD(*(unsigned short *)&buf[offset]))
               		{
                  		case 1:
                     		offset += 2;
                     		switch(buf[offset++])
                        		{
                           			case FT_FILENAME:  // name
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			//memcpy(nom, buf + offset, datalen);
                              			//p->Name = (char *)mystrdup(nom);
                              			//p->Name[datalen] = 0;
                              			offset += datalen;
                              			break;
                           			case FT_FILETYPE:  // filetype (audio, pro ...)
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			//memcpy(filetype, buf + offset, datalen);
                              			//p->Type = (char *)mystrdup(filetype);
                              			//p->Type[datalen] = 0;
                              			offset += datalen;
                              			break;
                           			case FT_FILEFORMAT:  // fileextension (mp3, ...)
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			offset += datalen;
                              			break;
                           			case FT_PARTFILENAME:      // tmp file name
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
			                            //memcpy(partfilename, buf + offset, datalen);
			                            //p->tmpFileName = (char *)mystrdup(partfilename);
										//memcpy(p->tmpFileName, buf + offset, datalen);
			                            //p->tmpFileName[datalen] = 0;
                              			offset += datalen;
                              			break;
                           			default:
                              			D(DBF_NET, "cas 0x02/1 j'ai oublié d'implémenter le tag: %x", buf[offset-1]);
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			offset += datalen;
                              			break;
                        		}
                     		break;
                  		case 2:
                     		offset += 2;
                     		break;
                  		case 5:
                     		offset += 2;
                     		switch(buf[offset++])
                        		{
                           			case 0x63:  // codec
                              			offset += 4;
                              			datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			//memcpy(codec, buf + offset, datalen);
                              			//p->Codec = (char *)mystrdup(codec);
                              			offset += datalen;
                              			break;
                      	 		}
                    		break;
                  		default:
                     		D(DBF_NET, "0x02--> oups, l'implementation du tag (%02x) n'est pas encore fait\n", buf[offset - 1]);
                     		offset += 2;
                     		offset += datalen;
		     				//second part of the tag
		     				datalen = SWAPWORD(*(unsigned short *) &buf[offset]);
		     				offset += 2;
                     		offset += datalen;
                     		break;
               		}
            	break;
         	case 0x03:
            	switch(datalen = SWAPWORD(*(unsigned short *)&buf[offset]))
               		{
                  		case 1:
                     		offset += 2;
                     		switch(buf[offset++])
                        		{
                           			case FT_FILESIZE:  // filesize
                              			p->Size = SWAPLONG(*(unsigned int *)&buf[offset]);
                              			offset += 4;
                              			break;
                           			case FT_LASTSEENCOMPLETE:  // last seen complete
                              			offset += 4;
                              			break;
                           			case FT_TRANSFERED:      // size already transfered
                              			p->Received = SWAPLONG(*(unsigned int *)&buf[offset]);
                              			offset += 4;
                              			break;
                           			case FT_OLDDLPRIORITY:      // priority of the file, not interesting for the moment
                              			offset += 4;
                              			break;
                           			case FT_STATUS:  // Status of the file... not interesting for the moment
                              			offset += 4;
                              			break;
                           			case FT_SOURCES:  // nb sources ?
                              			p->Sources = SWAPLONG(*(unsigned int *)&buf[offset]);
                              			offset += 4;
                              			break;
                           			case FT_PERMISSIONS:  // permissions bits, never seen this tag...
                              			offset += 4;
                              			break;
                           			case FT_COMPLETE_SOURCES:
                              			offset += 4;
                              			break;
                           			default:
                              			D(DBF_NET, "cas 0x03/1 j'ai oublié d'implémenter ce tag: 0x%x\n", buf[offset-1]);
                              			offset += 4; // we skip 4 bytes cause we don't know the current tag
                              			break;
                        		}
                     		break;
                  		case 2:
                     		offset += 2;
                     		switch(buf[offset++])
                        		{							
                           			case FT_GAPSTART:  //Gap Start
										p->gapStart[buf[offset++] - 0x30] = SWAPLONG(*(unsigned int *)&buf[offset+1]);  // Should not have to add +1 to offset here !! Gcc bug ? SixK
                              			offset += 4;
                              			p->nbGaps++;
										D(DBF_NET, "p->start : %ld",p->gapStart[0]);
			                            break;
                           			case FT_GAPEND:      //Gap End										
                              			p->gapEnd[buf[offset++] - 0x30] = SWAPLONG(*(unsigned int *)&buf[offset+1]);
                              			offset += 4;
										D(DBF_NET, "p->End : %ld\n",p->gapEnd[0]);
 			                            break;
                           			default:
                              			D(DBF_NET, "cas 0x03/2 j'ai oublié d'implémenter ce tag: 0x%x\n", buf[offset-1]);
                              			offset += 4; // we skip 4 bytes cause we don't know the current tag
                              			break;
                        		}
                     		break;
                  		default:
                     		D(DBF_NET, "0x03--> oups, l'implementation du tag (%02x) n'est pas encore fait", buf[offset - 1]);
                      		// >2, we are in filetype tags, we don't handle this for the moment
                     		offset += 2;
                     		offset += datalen;        // tagname
                     		offset += 4;                      // value
                     		break;
               		}
            	break;
			default:
				D(DBF_NET, "probleme here we received Tag Class : 0x%x\n", buf[offset-1]);
				break;
      	}
   	return(offset);
}*/

/*static int ParseEDKTags(unsigned char *buf, struct ResearchList *p)
{
   long datalen;
   int offset = 0;
   static char nom[SIZE_LARGE], filetype[SIZE_DEFAULT], partfilename[SIZE_LARGE], codec[SIZE_DEFAULT];

   switch(buf[offset++])
      {
         case 0x02:
            switch(datalen = SWAPWORD(*(unsigned short *)&buf[offset]))
               {
                  case 1:
                     offset += 2;
                     switch(buf[offset++])
                        {
                           case FT_FILENAME:  // name
                              datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              offset += 2;
                              memcpy(nom, buf + offset, datalen);
                              p->Name = (char *)mystrdup(nom);
                              p->Name[datalen] = 0;
                              offset += datalen;
                              break;
                           case FT_FILETYPE:  // filetype (audio, pro ...)
                              datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              offset += 2;
                              memcpy(filetype, buf + offset, datalen);
                              p->Type = (char *)mystrdup(filetype);
                              p->Type[datalen] = 0;
                              offset += datalen;
                              break;
                           case FT_FILEFORMAT:  // fileextension (mp3, ...)
                              datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              offset += 2;
                              offset += datalen;
                              break;
                           case FT_PARTFILENAME:      // tmp file name
                              datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              offset += 2;
                              memcpy(partfilename, buf + offset, datalen);
                              p->PartFileName = (char *)mystrdup(partfilename);
                              p->PartFileName[datalen] = 0;
                              offset += datalen;
                              break;
                           default:
                              D(DBF_NET, "cas 0x02/1 j'ai oublié d'implémenter le tag: %x\n", buf[offset - 1]);
                              datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              offset += 2;
                              offset += datalen;
                              break;
                        }
                     break;
                  case 2:
                     offset += 2;
                     break;
                  case 5:
                     offset += 2;
                     switch(buf[offset++])
                        {
                           case 0x63:  // codec
                              offset += 4;
                              datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                              offset += 2;
                              memcpy(codec, buf + offset, datalen);
                              p->Codec = (char *)mystrdup(codec);
                              offset += datalen;
                              break;
                       }
                    break;
                  default:
                     //D(DBF_NET, "0x02--> oups, l'implementation du tag (%02x) n'est pas encore fait\n", buf[offset - 1]);
                     offset += 2;
                     offset += datalen;
		     //second part of the tag
		     datalen = SWAPWORD(*(unsigned short *) &buf[offset]);
		     offset += 2;
                     offset += datalen;
                     break;
               }
            break;
         case 0x03:
            switch(datalen = SWAPWORD(*(unsigned short *)&buf[offset]))
               {
                  case 1:
                     offset += 2;
                     switch(buf[offset++])
                        {
                           case FT_FILESIZE:  // filesize
                              p->Size = SWAPLONG(*(unsigned int *)&buf[offset]);
                              offset += 4;
                              break;
                           case FT_LASTSEENCOMPLETE:  // last seen complete
                              offset += 4;
                              break;
                           case FT_TRANSFERED:      // size already transfered
                              p->TransferedSize = SWAPLONG(*(unsigned int *)&buf[offset]);
                              offset += 4;
                              break;
                           case FT_OLDDLPRIORITY:      // priority of the file, not interesting for the moment
                              offset += 4;
                              break;
                           case FT_STATUS:  // Status of the file... not interesting for the moment
                              offset += 4;
                              break;
                           case FT_SOURCES:  // nb sources ?
                              p->Sources = SWAPLONG(*(unsigned int *)&buf[offset]);
                              offset += 4;
                              break;
                           case FT_PERMISSIONS:  // permissions bits, never seen this tag...
                              offset += 4;
                              break;
                           case FT_COMPLETE_SOURCES:
                              offset += 4;
                              break;
                           default:
                              D(DBF_NET, "cas 0x03/1 j'ai oublié d'implémenter ce tag: 0x%x\n", buf[offset - 1]);
                              offset += 4; // we skip 4 bytes cause we don't know the current tag
                              break;
                        }
                     break;
                  case 2:
                     offset += 2;
                     switch(buf[offset++])
                        {
                           case FT_GAPSTART:  //Gap Start
                              p->GapStart[buf[offset++] - 0x30] = SWAPLONG(*(unsigned int *)&buf[offset]);
                              offset += 4;
                              p->NbGaps++;
                              break;
                           case FT_GAPEND:      //Gap End
                              p->GapEnd[buf[offset++] - 0x30] = SWAPLONG(*(unsigned int *)&buf[offset]);
                              offset += 4;
                              break;
                           default:
                              D(DBF_NET, "cas 0x03/2 j'ai oublié d'implémenter ce tag: 0x%x\n", buf[offset-1]);
                              offset += 4; // we skip 4 bytes cause we don't know the current tag
                              break;
                        }
                     break;
                  default:
                     //printf("0x03--> oups, l'implementation du tag (%02x) n'est pas encore fait\n", buf[offset - 1]);
                      // >2, we are in filetype tags, we don't handle this for the moment
                     offset += 2;
                     offset += datalen;        // tagname
                     offset += 4;                      // value
                     break;
               }
            break;

				default:
					printf("probleme here we received Tag Class : 0x%x\n", buf[offset-1]);
				break;

      }
   return(offset);
}*/

/***************************************************************************/
void myseek(FILE *fp, int pos)
{
   	int filelen;
   	char *pad;
   	int gapleft;

	D(DBF_NET, "Entered MySeek");
   	filelen = fseek(fp, 0, SEEK_END);
   	filelen = ftell(fp);
   	if(pos > filelen)
      	{
         	gapleft = pos - filelen;
         	while(gapleft > PARTSIZE)
            	{
               		pad = malloc(PARTSIZE);
               		// setmem(pad, PARTSIZE ,0);
					memset(pad, 0, PARTSIZE);
               		fwrite(pad, PARTSIZE, 1 , fp );
               		free(pad);
               		gapleft -= PARTSIZE;
            	}
         	pad = malloc(gapleft);
         	// setmem(pad, gapleft ,0);
			memset(pad, 0, gapleft);
         	fwrite(pad, gapleft, 1, fp);
         	free(pad);
      	}
   	fseek(fp, pos, SEEK_SET); //go to the position from the beginning of the file
	D(DBF_NET, "Leaving MySeek");
}

/***************************************************************************/
void gapHandle(struct DownloadList *dl, long start, long end)
{
   	int x = 0;
   	int oldGapNb;
   	int stop = 0;

	D(DBF_NET, "Entered gapHandle");
   	while(x < dl->nbGaps && !stop)
      	{
         	if(start == dl->gapStart[x])
            	{					
               		if(end < dl->gapEnd[x])
               			{						
                     		//on comble un trou au début du Gap
                     		dl->gapStart[x] = end;
                     		stop = 1;
               			}
               		else if(end == dl->gapEnd[x])
               			{						
                     		//on comble un Gap entier, on ecrase le gap courant par les gaps suivants
                     		for(; x < dl->nbGaps; x++)
                        		{
                           			dl->gapStart[x] = dl->gapStart[x + 1];
                           			dl->gapEnd[x] = dl->gapEnd[x + 1];
                        		}
                     		dl->nbGaps--;
                     		stop = 1;
               			}
            	}
         	else if(start > dl->gapStart[x])
            	{
               		if(start > dl->gapEnd[x])
                  		{
                       		//on passe au Gap suivant
                  		}
               		else if(end < dl->gapEnd[x])
               	   		{
                     		//on crée 2 nouveaux gaps à l'intérieur du gap courant, on décale les gaps suivant
                     		oldGapNb = x;							
                     		for(x = dl->nbGaps; oldGapNb < x; x--)
                     			{		
                     				dl->gapStart[x] = dl->gapStart[x-1];
                        			dl->gapEnd[x] = dl->gapEnd[x-1];
                     			}
							dl->gapStart[oldGapNb + 1] = end;
                     		dl->gapEnd[oldGapNb] = start;
    	                 	dl->nbGaps++;
                     		stop = 1;
                  		}
               		else if(end == dl->gapEnd[x])
                  		{							
                     		//on comble une fin de Gap
                     		dl->gapEnd[x] = start;
                     		stop = 1;
                  		}
            	}
         	x++;
      	}
   	for(x = 0; x < dl->nbGaps; x++) D(DBF_NET, "nextstart: %i, nextend: %i", dl->gapStart[x], dl->gapEnd[x]);
   	D(DBF_NET, "End of gap handle nbGaps:%i", dl->nbGaps);
	D(DBF_NET, "Leaving gapHandle");
}

void uploadGapHandle(struct Host *dl, long start, long end)
{
   	int x = 0;
   	int oldGapNb;
   	int stop = 0;

	D(DBF_NET, "Entered gapHandle");
   	while(x < dl->nbGaps && !stop)
      	{
         	if(start == dl->gapStart[x])
            	{					
               		if(end < dl->gapEnd[x])
               			{						
                     		//on comble un trou au début du Gap
                     		dl->gapStart[x] = end;
                     		stop = 1;
               			}
               		else if(end == dl->gapEnd[x])
               			{						
                     		//on comble un Gap entier, on ecrase le gap courant par les gaps suivants
                     		for(; x < dl->nbGaps; x++)
                        		{
                           			dl->gapStart[x] = dl->gapStart[x + 1];
                           			dl->gapEnd[x] = dl->gapEnd[x + 1];
                        		}
                     		dl->nbGaps--;
                     		stop = 1;
               			}
            	}
         	else if(start > dl->gapStart[x])
            	{
               		if(start > dl->gapEnd[x])
                  		{
                       		//on passe au Gap suivant
                  		}
               		else if(end < dl->gapEnd[x])
                  		{
                     		//on crée 2 nouveaux gaps à l'intérieur du gap courant, on décale les gaps suivant
                     		oldGapNb = x;							
                     		for(x = dl->nbGaps; oldGapNb < x; x--)
                     			{		
                     				dl->gapStart[x] = dl->gapStart[x-1];
                        			dl->gapEnd[x] = dl->gapEnd[x-1];
                     			}
							dl->gapStart[oldGapNb + 1] = end;
                     		dl->gapEnd[oldGapNb] = start;
                     		dl->nbGaps++;
                     		stop = 1;
                  		}
               		else if(end == dl->gapEnd[x])
                  		{							
                     		//on comble une fin de Gap
                     		dl->gapEnd[x] = start;
                     		stop = 1;
                  		}
            	}
         	x++;
      	}
   	for(x = 0; x < dl->nbGaps; x++) D(DBF_NET, "nextstart: %i, nextend: %i", dl->gapStart[x], dl->gapEnd[x]);
   	D(DBF_NET, "End of gap handle nbGaps:%i", dl->nbGaps);
	D(DBF_NET, "Leaving gapHandle");
}

/********************************************************************************************/
void parse_host_reply(struct Host *host, unsigned char *buf)
{
   	unsigned char buf2[256];
   	unsigned char pload[32000];
   	int offset;
   	int nb_match;
   	int nb_tags;
   	unsigned char fileID[16];
   	char clientIP[16];
   	int current_match;
   	int i;
   	char buffer[SIZE_LARGE];

   	switch(buf[0])
      	{
         	case 0x00:  // Doesn't exists in edonkey protocole
            	D(DBF_NET, "hum a problem ? Doesn't exists in edonkey protocole");
            	break;
         	case OP_LOGINREQUEST:  // Someone want to login
				{			
					struct Host *serverNode = (struct Host *)list_first(&G->list_hosts);
					
            		D(DBF_NET, "0x01 ...  OP_LOGINREQUEST someone want to login");
            		if(host->hosttype == 2) LoginEDKTags(buf + 28, host);
            		else 
						{
							host->nom = (char *)mystrdup("server");
							D(DBF_NET, "op_loginrequest host->nom %s", host->nom);
						}
					buf2[0] = 0x4c;
            		offset = 1;
            		memcpy(buf2 + offset, userHash, 16);
            		offset += 16;
            		*(unsigned int *)&buf2[offset] = myIp;
            		offset += 4;
            		*(unsigned short *)&buf2[offset] = SWAPWORD(userPort);
            		offset += 2;
            		buf2[offset] = 0x03;
            		buf2[offset + 1] = 0x00;
            		buf2[offset + 2] = 0x00;
            		buf2[offset + 3] = 0x00;
            		offset += 4;
            		offset += write_edk_tags(buf2 + offset, 0x01, NULL); // write the name of the user in buf
            		offset += write_edk_tags(buf2 + offset, 0x11, NULL); // write the version number
            		offset += write_edk_tags(buf2 + offset, 0x0f, NULL); // specify the port of Edk client (4662)
            		*(unsigned int *)&buf2[offset] = serverNode->serverIP;
            		offset += 4;
            		*(unsigned short *)&buf2[offset] = SWAPWORD(serverNode->serverPort);
					D(DBF_NET, "op_loginrequest serverPort : %ld", serverNode->serverPort);
            		offset += 2;
            		sendPacket(host->sockfd, buf2, offset);
            		host->state = CONNECTION_ACTIVE;
					D(DBF_NET, "OP_LOGINREQUEST user is now connected");
				}	
            	break;
         	case OP_GETSERVERLIST:
            	D(DBF_NET, "0x14 ... OP_GETSERVERLIST");
            	break;
			case OP_SERVERLIST: 
         		{ 
   	      			int len = strlen(buf); 
            		int nbserv = 0; 
            		//APTR lv = G->MA->GUI.LV_SERVER; 
            		
					D(DBF_NET, "0x32 ... OP_SERVERLIST"); 
            		offset = 1; 
            		nbserv = buf[offset]; 
            		offset += 1; 
            		i = 0; 
            		/*while(i < nbserv) 
            			{ 
      	      				struct ServerList *node = malloc(sizeof(struct ServerList)); 
              			 	if(node) 
               					{ 
         	      					memset(node, 0, sizeof(struct ServerList)); 
            	  	 				host->status = 0; 
                  					sprintf(host->IP, "%d.%d.%d.%d", buf(offset), buf(offset + 1), buf(offset + 2), buf(offset + 3)); 
                  					offset += 4; 
                  					host->Port = SWAPWORD(*(unsigned short *)&buf[offset]); 
                  					offset += 2; 
                  					DoMethod(lv, MUIM_NList_InsertSingle, node, MUIV_NList_Insert_Bottom); 
               					} 
               				i++; 
            			} */
         			if(nbserv > 0) D(DBF_NET, "nombre de serveurs rajoutés %d", nbserv); 
         		} 
         		break;
         	case OP_SERVERIDENT:
            	{
               		int len = strlen(buf);

               		D(DBF_NET, "0x41 ... OP_SERVERIDENT");
               		D(DBF_NET, "0x41 longueur buffer %d %s", len, buf);
            	}
            	break;
         	case OP_SEARCHRESULT:
            	D(DBF_NET, "0x33 ... OP_SEARCHRESULT");
				/*if(host->sockfd != serverSockfd)
					{
						D(DBF_NET, "Only server please ! ");
						break;
					}*/
            	offset = 1;
            	nb_match = SWAPLONG(*(unsigned int *)&buf[offset]);
            	offset += 4;
            	/*if(nb_match == 0) RS_Status(nb_match + 1, nb_match + 1);
            	else
               		{
                  		for(current_match = 0; current_match < nb_match; current_match++)
                     		{
                        		struct ResearchList *rl;

                        		rl = malloc(sizeof(struct ResearchList));
                        		if(rl)
                           			{
                              			memset(rl, 0, sizeof(struct ResearchList));
                              			rl->NbGaps = 1;
                              			rl->status = 8;
                              			memcpy(fileID, buf + offset, 16);
                              			rl->FileID = (char *)mystrdup(hash_bin2hex(fileID));
                              			offset += 16;
                              			sprintf(clientIP, "%i.%i.%i.%i", buf[offset], buf[offset + 1], buf[offset + 2], buf[offset + 3]);
                              			rl->IP = (char *)mystrdup(clientIP);
                              			offset += 4;
                              			rl->ClientPort = SWAPWORD(*(unsigned short *)&buf[offset]);
                              			offset += 2;
                              			nb_tags = SWAPLONG(*(unsigned int *)&buf[offset]);
                              			offset += 4;
                              			for(i = 0; i < nb_tags; i++)
                                 			{
                                    			offset += ParseEDKTags(buf + offset, rl);
                                 			}
    		                          	if(rl->Sources != 0)
                                 			{
                                    			static char str[SIZE_LARGE];

                                    			sprintf(str, "%s", IdentifyFile(rl->Name));
                                    			if(!strcmp(str, G->keytype))
                                       				{
                                          				rl->Type = (char *)mystrdup(str);
                                          				list_insert_tail(&rs_list, &rl->node);
                                       				}
                                    			else if(!strcmp(G->keytype, GetStr(MSG_Research_Type_Any)))
                                       				{
                                          				rl->Type = (char *)mystrdup(str);
                                          				list_insert_tail(&rs_list, &rl->node);
                                       				}
                                 			}
										D(DBF_NET, "ID : %s, Size : %u\n",rl->FileID, rl->Size));
  			                        }
                        		RS_Status(nb_match, current_match);
                     		}
               		}
            	if(G->RS) RS_LoadResearchFiles();*/
            	break;
         	case OP_SERVERSTATUS:
				{
               		int nb_users;
               		int nb_files;
               		static char buffer[SIZE_LARGE];

               		D(DBF_NET, "0x34 ... OP_SERVERSTATUS");
					if(host->sockfd != serverSockfd)
						{
							D(DBF_NET, "Only server please ! ");
							break;
						}
               		nb_users = SWAPLONG(*(unsigned int *)&buf[1]);
               		//sprintf(buffer, GetStr(MSG_MA_Users_Connected), nb_users);
					D(DBF_NET, "0x34 ... OP_SERVERSTATUS nb_users %ld", nb_users);
               		//set(G->MA->GUI.TX_MAIN_USERS, MUIA_Text_Contents, buffer);
               		nb_files = SWAPLONG(*(unsigned int *)&buf[5]);
					D(DBF_NET, "0x34 ... OP_SERVERSTATUS nb_users %ld", nb_files);
               		//sprintf(buffer, GetStr(MSG_MA_Files_Connected), nb_files);
               		//set(G->MA->GUI.TX_MAIN_FILES, MUIA_Text_Contents, buffer);
            	}
           	 	break;
         	case OP_SERVERMESSAGE:				
            	D(DBF_NET, "0x38 ... OP_SERVERMESSAGE");
				if(host->sockfd != serverSockfd)
					{
						D(DBF_NET, "Only server please ! ");
						break;
					}
				D(DBF_NET, "buf %s", buf);
				D(DBF_NET, "sending shared list");
                sprintf(buf, "ss 0 0");
            	parse_cmd(host, buf);
				buf2[0] = 0x14;
	            offset = 1;
	            memcpy(buf2 + offset, buf + 1, 16);
	            offset += 16;
	            sendPacket(host->sockfd, buf2, offset);
		        break;
         	case OP_IDCHANGE:  // login confirmation from serveur
				{					
					unsigned int nb_id;

            		D(DBF_NET, "0x40 ... OP_IDCHANGE");
					if(host->sockfd != serverSockfd)
						{
							D(DBF_NET, "Only server please ! ");
							break;
						}
            		nb_id = SWAPLONG(*(unsigned int *)&buf[1]);
					D(DBF_NET, " 0x40 ... OP_IDCHANGE %ld", nb_id);
            		//sprintf(buffer, GetStr(MSG_Server_ID), nb_id);
         		   	host->state = CONNECTION_ACTIVE;
         			D(DBF_NET, "sending shared list");
                   	sprintf(buf, "ss 0 0");
            		parse_cmd(NULL, buf);	
					sprintf(buf, "danger girl 0 0");						
					parse_cmd(host, buf);							
				}
            break;
         case OP_REQUESTPARTS:
            {
               	//static char ID[16];

               	D(DBF_NET, "0x47 ... OP_REQUESTPARTS");
               	offset = 1;
	       		//memcpy(ID, buf + offset, 16); // here we save the file ID in host ID
               	//host->ID = (char *)mystrdup(hash_bin2hex(ID));  // Partie a revoir possible probleme de texte
	       		offset += 16;
	       		host->gapStart[0] = SWAPLONG(*(unsigned int *) &buf[offset]);
	       		offset += 4;
	       		host->gapStart[1] = SWAPLONG(*(unsigned int *) &buf[offset]);
	       		offset += 4;
	       		host->gapStart[2] = SWAPLONG(*(unsigned int *) &buf[offset]);
	       		offset += 4;
	       		host->gapEnd[0]   = SWAPLONG(*(unsigned int *) &buf[offset]);
	       		offset += 4;
	       		host->gapEnd[1]   = SWAPLONG(*(unsigned int *) &buf[offset]);
	       		offset += 4;
	       		host->gapEnd[2]   = SWAPLONG(*(unsigned int *) &buf[offset]);
	       		offset += 4;
				D(DBF_NET, "st1: %ld end1: %ld st2: %ld end2: %ld st3: %ld end: %ld",host->gapStart[0],host->gapEnd[0], host->gapStart[1],host->gapEnd[1],host->gapStart[2], host->gapEnd[2]);
	       		D(DBF_NET, "gap: %ld %ld", host->gapStart[0], host->gapEnd[1]);
	       		if(host->gapEnd[0] > 0) host->nbGaps = 1;
	       		if(host->gapEnd[1] > 0) host->nbGaps = 2;
	       		if(host->gapEnd[2] > 0) host->nbGaps = 3;
	       		host->state = CONNECTION_UPLOAD_STARTED;			
#if 1		
				if(host->hosttype == 2 && host->connection == 1)  // client + incoming connection
      				{
						char fileName[255];
						BPTR fp;
		   				int gapSize = 0;
		   				int offset = 0;

						D(DBF_NET, "p->hostType == 2 && p->connectionType == 1");
         				if(host->state == CONNECTION_UPLOAD_STARTED && host->nbGaps)
         					{
            					//struct SharedList *sl = (struct SharedList *)list_first(&sh_list);
					
								D(DBF_NET, "host->state == CONNECTION_UPLOAD_STARTED && host->nbGaps");
            					// z = (pelement)findbyID(shares, p->ID);
								/*while(strncmp(host->ID, sl->FileID, 16) != 0 && sl != NULL) 
									{
										sl = (struct SharedList *)node_next(&sl->node); 
										// D(DBF_NET, "And there FileID : %s!\n",dl->ID));
									}
								if(strncmp(host->ID, sl->FileID, 16) == 0 && sl != NULL)
									{	
            							sprintf(fileName, "%s%s", C->SharedDir, sl->FileName);
            							fp = Open(fileName, MODE_OLDFILE);
            							if(fp)
            								{
         	   									gapSize = host->gapEnd[0] - host->gapStart[0];
              									if(gapSize > MAXPARTSEND) gapSize = MAXPARTSEND;
               									buf[0] = 0x46;
               									offset = 1;
               									memcpy(buf + offset, hash_hex2bin(sl->FileID), 16);
               									offset+=16;
               									*(unsigned int *)&buf[offset] = SWAPLONG(host->gapStart[0]);
               									offset += 4;
               									*(unsigned int *)&buf[offset] = SWAPLONG(host->gapStart[0] + gapSize);
               									offset += 4;
               									Seek(fp, host->gapStart[0], OFFSET_BEGINNING);
                								// fseek(fp, host->gapStart[0], SEEK_SET);
               									FRead(fp, buf + offset, 1, gapSize);
                								// fread(buf + offset, 1, gapSize, fp);
               									offset += gapSize;
               									Close(fp);
               									sendPacket(host->sockfd, buf, offset);
												printf("Start : %ld , End : %ld size :%ld\n",host->gapStart[0], host->gapEnd[0],gapSize);
               									uploadGapHandle(node, host->gapStart[0], host->gapStart[0] + gapSize);
												Delay(100);
												if(host->nbGaps == 0)
													{
														printf("file completed\n");
														host->state = CONNECTION_NULL;
														closeHandler(node);
													}
											}
            						}*/
         					}
      				}
#endif		
            }
            break;
         case OP_ASKSHAREDFILES:  // host Ask for shared shared files
            {
               //struct SharedList *sl = (struct SharedList *)list_first(&sh_list);

               D(DBF_NET, "0x4A  ... host Ask for shared shared files");
               /*buf2[1] = 0x4B; // Ask shared file reply
	       		offset  = 1;
	       		*(unsigned int *) &buf2[offset] = SWAPLONG(nb_sharedfiles);
	       		offset += 4;
	       		D(DBF_NET, "nb: %i\n", nb_sharedfiles));*/

               /*while(sl)
               {
                  memcpy(buf2 + offset, sl->FileID, 16);
		     			offset += 16;
		     			*(unsigned int *) &buf2[offset] = SWAPLONG(0);
		     			offset += 4;
		     			*(unsigned short *) &buf2[offset] = SWAPWORD(0);
		     			offset += 2;
		     			*(unsigned int *) &buf2[offset] = SWAPLONG(3); // nombre de tags...
		     			offset += 4;
		     			offset += write_edk_tags_shared(buf2 + offset, 0x01, sl); // write the name of the file in buf
		     			offset += write_edk_tags_shared(buf2 + offset, 0x02, sl); // fileSize
		     			offset += write_edk_tags_shared(buf2 + offset, 0x03, sl); // fileType
                  sl = (struct SharedList *)node_next(&sl->node);
               }
               sendPacket(host->sockfd, buf2, offset);*/
            }
            break;
         case OP_HELLOANSWER:  // login confirmation from client
            D(DBF_NET, "0x4C... login confirmation from client...");
            host->state = CONNECTION_ACTIVE;
	    	LoginEDKTags(buf + 27, host);						
            buf2[0] = 0x58;
            offset = 1;
            //memcpy(buf2 + offset, hash_hex2bin(host->ID), 16);
            offset += 16;
            sendPacket(host->sockfd, buf2, offset);            			
            break;
         case OP_CHANGE_CLIENT_ID:
				{					
					unsigned int nb_id;
				
            		nb_id = SWAPLONG(*(unsigned int *)&buf[1]);
					D(DBF_NET, "0x4D...OP_CHANGE_CLIENT_ID.. %ld", nb_id);
            		//sprintf(buffer, GetStr(MSG_Server_ID), nb_id);
            		//set(G->MA->GUI.TX_SE_INFO_ID, MUIA_Text_Contents, buffer);
				}
            	break;
			case OP_SETREQFILEID:
				{
					/*struct SharedList *sh = (struct SharedList *)list_first(&sh_list);
					memcpy (fileID, buf + 1, 16);

					buf2[0] = 0x50; // file status
					offset=1;

					while(strncmp(fileID, sh->FileID, 16)!=0 && sh != NULL)
					{
						sh = (struct SharedList *)node_next(&sh->node);
					}

					if(strncmp(sh->FileID,fileID,16)==0&& sh!=NULL)
					{
						memcpy(buf2+offset, sh->FileID, 16);
						offset+=16;

						// dunno what to do here we have 3 bytes

						*(unsigned short *) &buf2[offset] = SWAPWORD (0);
						offset += 2;			

						buf2[offset]=0;
						offset+=1;
	
						sendPacket (host->sockfd, buf2, offset);
					}*/
				}
            break;
         case OP_STARTUPLOADREQ:
            D(DBF_NET, "0x54... open slot for the host...");
            buf2[0] = 0x55; // We open a slot for the host
            sendPacket(host->sockfd, buf2, 1);
            break;
         case OP_CANCELTRANSFER:  // Cancel transfert
            D(DBF_NET, "0x56... cancel transfert...");
            break;
         case OP_REQUESTFILENAME:
				{
					//struct SharedList *sh = (struct SharedList *)list_first(&sh_list);
            		D(DBF_NET, "0x58... ask shared file reply...");
					//memcpy (fileID, buf + 1, 16);
										

					/*while(strncmp(hash_bin2hex(fileID), sh->FileID,16)!=0 && sh != NULL)
					{
						sh = (struct SharedList *)node_next(&sh->node);						
					}					
					
	
	   			if(strncmp(sh->FileID,hash_bin2hex(fileID),16)==0&& sh!=NULL)
					{
						printf("ok found file\n");
						buf2[0] = 0x59; // Ask shared file reply
						offset=1;
						memcpy(buf2+offset, hash_hex2bin(sh->FileID), 16);
						offset+=16;

						*(unsigned short *) &buf2[offset] = SWAPWORD (strlen(sh->FileName));
						offset += 2;			

						printf("nom: %s\n",sh->FileName);

						memcpy(buf2+offset,sh->FileName, strlen(sh->FileName));
						offset+=strlen(sh->FileName);
	
						sendPacket (host->sockfd, buf2, offset);

						#if 0
						buf2[0] = 0x50; // file status
						offset=1;

						p=findbyID(shares, buf+1);
	
	   				if(strncmp(p->ID,buf+1,16)==0)
						{
							memcpy(buf2+offset, p->ID, 16);
							offset+=16;

							// dunno what to do here we have 3 bytes

							*(unsigned short *) &buf2[offset] = SWAPWORD (0);
						offset += 2;			

						buf2[offset]=0;
						offset+=1;
	
						sendPacket (sockfd, buf2, offset);
						}
						#endif
					}else
					{
						printf(" 0x48 ... sorry no such file : %s !\n", hash_bin2hex(fileID)); 

						buf2[0] = 0x48; // Ask shared file reply
						offset=1;
						memcpy(buf2+offset, buf+1, 16);
						offset+=16;
						sendPacket (host->sockfd, buf2, offset);
					}*/




				
				}
            break;				
         case OP_REQFILENAMEANSWER:  // file request answer
            D(DBF_NET, "0x59... file request answer...");
            buf2[0] = 0x4f;
            offset = 1;
            memcpy(buf2 + offset, buf + 1, 16);
            offset += 16;
            sendPacket(host->sockfd, buf2, offset);
            break;
         case OP_ACCEPTUPLOADREQ:  // Accept upload request, no we are ready to download, simply ask for the file
				{
					//struct DownloadList *dl = (struct DownloadList *)list_first(&dl_list);
            		
					D(DBF_NET, "0x55... accept download request...");
                   	D(DBF_NET, "We passed here socket : %ld, ID : %s !", host->sockfd, host->ID);
           	      	//D(DBF_NET, "And here FileID : %s!", dl->ID);
					host->state = CONNECTION_SLOT_OPEN;
		  			/*while(strncmp(host->ID, dl->ID, 16) != 0 && dl != NULL) 
						{
							dl = (struct DownloadList *)node_next(&dl->node); 
							D(DBF_NET, "And there FileID : %s!\n",dl->ID);
						}
					if(dl == NULL || dl->ID == NULL || host->ID == NULL) D(DBF_NET, "Sorry we missed the file :(");
					else
						{
		  					if(strncmp(host->ID, dl->ID, 16) == 0)
		     					{
									sprintf(buf2,"d %i 0", dl->nb);
									parse_cmd(node, buf2);
									D(DBF_NET, "Bingo !");
		     					}
						}*/
	       		}
            	break;
         	case OP_QUEUERANK: //Queue rank
            	host->queueRank = SWAPLONG(*(unsigned int *)&buf[1]);
            	D(DBF_NET, "0x5C queue rank...%ld", host->queueRank);
            	break;
         	case OP_FOUNDSOURCES:
            	{  // Found sources, we now know all the IP where files are available
               		unsigned char fileID[16];
               		char clientIP[16];
               		int clientPort, x, nb_sources;
					long ret;
					//struct DownloadList *dl = (struct DownloadList *)list_first(&dl_list);
					//APTR lv = G->TR->GUI.LV_DOWNLOAD;

               		offset = 1;
               		memcpy(fileID, buf + offset, 16);
               		offset += 16;
               		nb_sources = buf[offset];
               		D(DBF_NET, "0x42...nombre de sources disponible...%ld", nb_sources);
               		offset += 1;
               		x = 0;

					if (nb_sources>0)
					{
						/*while(strncmp(hash_bin2hex(fileID), dl->ID,16)!=0) 
						{
							dl = (struct DownloadList *)node_next(&dl->node);
						}
						dl->Sources = nb_sources;
					
						D(DBF_NET, "dlID:%s fileIDhashed:%s\n",dl->ID,hash_bin2hex(fileID)));
						// A revoir, ca plante si la fenetre n'est pas ouverte !!! SixK
						if (G->TR)
						{ 
							DoMethod(lv, MUIM_NList_Redraw, dl->nb -1);
							DoMethod(lv, MUIM_NList_Redraw, MUIV_NList_Redraw_Active);
						}
					
               	while(x < nb_sources)
               	{
                     sprintf(clientIP, "%i.%i.%i.%i", buf[offset], buf[offset + 1], buf[offset + 2], buf[offset + 3]);
                     offset += 4;
                     clientPort = SWAPWORD(*(unsigned short *)&buf[offset]);
                     offset += 2;
                     if(IP_OK(clientIP)) ret = connect_client(clientIP, clientPort, (char *)hash_bin2hex(fileID));
                     x++;
               	}*/
					}
            }
            break;
         case 0x46:  // part arrived, let's save it            
			{
				long dataStart, dataEnd, datalen;
				int x;
				char tmp_buf[255];
			   char tmp_buf2[255];
				FILE *fp;

           		//struct DownloadList *dl = (struct DownloadList *)list_first(&dl_list);
				D(DBF_NET, "cmd: 46... part arrived...");
				/*memcpy (fileID, buf + 1, 16);
				while(strncmp(hash_bin2hex(fileID), dl->ID, 16) != 0 && dl != NULL) 
					{
						dl = (struct DownloadList *)node_next(&dl->node); 
						D(DBF_NET, "And there FileID : %s!\n",dl->ID));
					}
				dataStart = SWAPLONG (*(unsigned int *) &buf[17]);
				dataEnd = SWAPLONG (*(unsigned int *) &buf[21]);
				printf("start : %ld, end : %ld\n", dataStart, dataEnd);
				datalen = dataEnd - dataStart;
				memcpy (pload, buf + 25, datalen);
				if(dl->nb >= 0)
					{
						//Gestion des noms de fichiers					
						if(strlen (dl->tmpFileName) == 0)
							{
								x = 1;
								// sprintf(tmp_buf, "%s%i.part", "ram:", x);
								sprintf(tmp_buf, "%s%i.part", C->TempDir, x);
								fp = fopen(tmp_buf, "r");
								fclose(fp);
								x++;
								while(fp != NULL)
									{
//								sprintf (tmp_buf, "%s%i.part", tmpDownloadDirectory, x);  // Should be Config->tempDir SixK
//								sprintf (tmp_buf, "%s%i.part", "ram:", x);  // Should be Config->tempDir SixK
								sprintf (tmp_buf, "%s%i.part", C->TempDir, x);
								fp = fopen (tmp_buf, "r");
								fclose (fp);
								x++;
							}
						strcpy (dl->tmpFileName, tmp_buf);
						dl->tmpFileName[strlen (dl->tmpFileName)] = 0;
					}
				fp = fopen (dl->tmpFileName, "rb+");
				if(fp==NULL) fp=fopen (dl->tmpFileName, "wb+");
				if(dataStart>0)
					{
						myseek(fp, dataStart);
					}
				else
					{
						// printf("not in myseek\n");
					}
				fwrite (pload, datalen, 1, fp);
				fclose (fp);
				dl->Received += datalen;
				// Gestion des Gaps à faire
				gapHandle(dl,dataStart,dataEnd);
				sprintf (tmp_buf, "%s.met", dl->tmpFileName);
				fp = fopen (tmp_buf, "w");
				offset = 0;
				buf2[offset] = 0xe0;
				offset += 1;
				*(unsigned int *) &buf2[offset] = SWAPLONG (0);
				offset += 4;
				memcpy (buf2 + offset, fileID, 16);
				offset += 16;
				*(unsigned short *) &buf2[offset] = SWAPWORD (0);
				offset += 2;
				*(unsigned int *) &buf2[offset] = SWAPLONG (8);
				offset += 4;
				offset+=write_edk_tags_download(buf2+offset,0x01, dl); // write the name of the file in buf
				offset+=write_edk_tags_download(buf2+offset,0x02, dl); // write the name of the file in buf
				offset+=write_edk_tags_download(buf2+offset,0x08, dl); // write the transfered size of the file in buf
				offset+=write_edk_tags_download(buf2+offset,0x12, dl); // write the name of tmpfile in buf
				offset+=write_edk_tags_download(buf2+offset,0x13, dl); // write tag 0x13 in buf
				offset+=write_edk_tags_download(buf2+offset,0x14, dl); // write tag 0x14 in buf
				for(x=0;x<dl->nbGaps;x++)
					{
						buf2[offset] = 0x03;
						buf2[offset + 1] = 0x02;
						buf2[offset + 2] = 0x00;
						buf2[offset + 3] = 0x09;
						offset += 4;
						buf2[offset] = 0x30+x;
					  	offset += 1;			
			  		  	*(unsigned int *) &buf2[offset] = SWAPLONG (dl->gapStart[x]);
					  	offset += 4;				
					  	buf2[offset] = 0x03;
					  	buf2[offset + 1] = 0x02;
					  	buf2[offset + 2] = 0x00;
					  	buf2[offset + 3] = 0x0a;
					  	offset += 4;
					  	buf2[offset] = 0x30+x;
					  	offset += 1;			
  					  	*(unsigned int *) &buf2[offset] = SWAPLONG (dl->gapEnd[x]);
					  	offset += 4;
					}
				// update_list(&databases_list, files->courant);
				//--------------------------------------------
				fwrite (buf2, offset, 1, fp);
				fclose (fp);
				if(dl->Received==dl->Size)
					{
//						sprintf (tmp_buf2, "%s%s", tmpDownloadDirectory, dl->Name);
						sprintf (tmp_buf2, "%s%s", C->IncomingDir, dl->Name);  // Should be Config->tempDir SixK
						printf("renaming %s to %s\n", dl->tmpFileName, tmp_buf2);
						rename(dl->tmpFileName, tmp_buf2);
						sprintf(tmp_buf2, "%s.met", dl->tmpFileName);
						remove(tmp_buf2);
						// Stop Download we have all we need
						buf2[0] = 0x49; // file status
						offset=1;
						memcpy(buf2+offset, hash_hex2bin(dl->ID), 16);
						offset+=16;
						sendPacket (host->sockfd, buf2, offset);
						# if 0  //sorry no upload for the moment SixK
						// we share the new file
	
						// w=(pelement) malloc (sizeof (ELEMENT));

						// w=copy_element_courant(files,w);
						// inserer_en_tete_de_liste(shares, w);

						buf2[0] = 0x15; // offer file list to server
						offset=1;

						*(unsigned int *) &buf2[offset] = SWAPLONG (1);
						offset+=4;
		
						memcpy(buf2+offset, files->courant->ID, 16);
						offset+=16;

						*(unsigned int *) &buf2[offset] = SWAPLONG (0);
						offset+=4;

						*(unsigned short *) &buf2[offset] = SWAPWORD (0);
						offset+=2;
		
						*(unsigned int *) &buf2[offset] = SWAPLONG (3); // nombre de tags...
						offset+=4;

						offset+=write_edk_tags_download(buf2+offset,0x01, files); // write the name of the file in buf
						offset+=write_edk_tags_download(buf2+offset,0x02, files); // fileSize
						offset+=write_edk_tags_download(buf2+offset,0x03, files); // fileType
			
						sendPacket (serverSockfd, buf2, offset);
						#endif


					}else
					{
						host->nbPartsRecv++;
					}

				}*/

			}

				D(DBF_NET, "Leaving OPCode : 0x46");
            	break;
         	case 0x50:  // file status, don't know much about this command
            	D(DBF_NET, "cmd: 50... file status...");
				offset=1;		
				offset+=16;
				nb_match = SWAPWORD(*(unsigned short *)&buf[offset]);
            	offset += 2;
				printf("File Info !!!! : %ld\n", nb_match); 
				buf2[0] = 0x51;
            	offset = 1;
            	memcpy(buf2 + offset, buf + 1, 16);
            	offset += 16;
            	sendPacket(host->sockfd, buf2, offset);
 	           break;
			case 0x49:
            D(DBF_NET, "cmd: 49...Upload stop request");
			break;

         case 0x48:
            D(DBF_NET, "cmd: 48...");
				printf("Sorry, no such file, we asked a not existing file\n");

			break;
         case 0x52:  //hashset answer, don't know much about this function
			{
           	//struct DownloadList *dl = (struct DownloadList *)list_first(&dl_list);

            /*D(DBF_NET, "cmd: 52...hashet answer...");
            	
				offset=1;		
					
				memcpy (fileID, buf + offset, 16);
				offset+=16;

				while(strncmp(hash_bin2hex(fileID), dl->ID, 16) != 0 && dl != NULL) 
				{
					dl = (struct DownloadList *)node_next(&dl->node); 
					// D(DBF_NET, "And there FileID : %s!\n",dl->ID));
				}


				if(strncmp(hash_bin2hex(fileID), dl->ID, 16) == 0)
				{
					int x=0;

					nb_match = SWAPWORD(*(unsigned short *)&buf[offset]);
            	offset += 2;
				
					printf("NB HASH !!!! : %ld\n", nb_match);				
					dl->nbHash = nb_match;

					if(nb_match>0)
					{
						dl->Hashlist = malloc ( 16 * nb_match * sizeof(char) + 1);
					}


					while(nb_match>x)
					{
						memcpy (dl->Hashlist+(x*16), buf + offset, 16);
						
						
						printf ("%d : %x %x %x %x %x %x %x %x \n", x, dl->Hashlist, dl->Hashlist + 1, dl->Hashlist + 2, dl->Hashlist + 3, dl->Hashlist + 4,
									dl->Hashlist + 5, dl->Hashlist + 6, dl->Hashlist + 7);
												
						
						memcpy(buf2, dl->Hashlist+(x*16),16);
						// buf2[16]=0;			
			
						printf("Hash : %s HashNb : %d Hash : %s\n", dl->ID, x , hash_bin2hex(buf2) );

						offset+=16;
						x++;
					}					
				}


            buf2[0] = 0x54;
            offset = 1;

            memcpy(buf2 + offset, hash_hex2bin(host->ID), 16);
            offset += 16;
            sendPacket(host->sockfd, buf2, offset);            

	    		host->state = CONNECTION_SLOT_ASKED;*/
			}
         break;
         default:
            break;
      }
}

void parse_cmd(struct Host *host, char *buf)
{
   	char *firstword;
   	char *param1;
   	char *param2;
   	int query_len;
   	int offset;

   	if(strncmp(buf, "quit", 4) == 0) return; //exit(5);
   	firstword = strtok(buf, " ");
   	param1 = strtok(NULL, " ");
   	param2 = strtok(buf + strlen(firstword) + strlen(param1) + 2, "\n");

   	// query a server for files
   	if(strcmp(firstword, "s") == 0)       
      	{
         	char *buf2;

         	buf2 = (char *)AllocVec(25600, MEMF_CLEAR);
         	if(buf2)
            	{
               		query_len = strlen(param2);
               		buf2[0] = 0x16;
               		buf2[1] = 0x01;
               		offset = 2;
               		*(unsigned short *)&buf2[offset] = SWAPWORD(query_len);
               		offset += 2;
               		memcpy(buf2 + offset, param2, query_len);
               		offset += query_len;
               		sendPacket(serverSockfd, buf2, offset);
               		FreeVec(buf2);
            	}
         	else D(DBF_NET, "NOMEMRAM");
      	}
   
   // sending list of shared files
  	if(strcmp(firstword, "ss") == 0)      
      	{
         	struct Share *sh = (struct Share *)list_first(&G->list_shared);
         	char *buf2;
			LONG nb_sharedfiles = 0;

         	buf2 = (char *)AllocVec(25600, MEMF_CLEAR);
         	if(buf2)
            	{
               		buf2[0] = 0x15;
               		offset = 1;
               		*(unsigned int *)&buf2[offset] = SWAPLONG(nb_sharedfiles);
               		offset += 4;
               		while(sh)
                  		{
                     		memcpy(buf2 + offset, hash_hex2bin(sh->FileID), 16);
                     		offset += 16;
                     		*(unsigned int *)&buf2[offset] = SWAPLONG(0);
                     		offset += 4;
                     		*(unsigned short *)&buf2[offset] = SWAPWORD(0);
                     		offset += 2;
                     		*(unsigned int *) &buf2[offset] = SWAPLONG(3);
                     		offset += 4;
                     		offset += write_edk_tags_shared(buf2 + offset, 0x01, sh); // write fileName
                     		offset += write_edk_tags_shared(buf2 + offset, 0x02, sh); // write fileSize
                     		offset += write_edk_tags_shared(buf2 + offset, 0x03, sh); // write fileType
                     		sh = (struct Share *)node_next(&sh->node);
                  		}
               		sendPacket(serverSockfd, buf2, offset);
					D(DBF_NET, "share list sent");
               		FreeVec(buf2);
            	}
         	else D(DBF_NET, "NOMEMRAM");
      	}
}
