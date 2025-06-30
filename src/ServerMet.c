/***************************************************************************
Bourriquet MorphOS
Copyright (C) 2022 Digitally
***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/icon.h>
#include <hardware/byteswap.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>

#include "Extrasrc.h"
#include "mui/ServerPage.h"

#include "Bourriquet.h"
#include "Global.h"
#include "Locale.h"
#include "Server.h"
#include "Busy.h"
#include "Lists.h"
#include "ServerMet.h"
#include "ServerList.h"
#include "DashBoard.h"
#include "Debug.h"

///
/// ParseServerMetFile
// Analyse un fichier server.met et retourne > 0 en cas de succès
LONG ParseServerMetFile(const char *smf)
{
	LONG result = 0; 
	char *buf;
    int dst = 0, nbserv = 0;	
	APTR lv = G->MA->GUI.LV_SERVERS;
	
  	ENTER();	
	InitServerList(G->ServerMet);
	DoMethod(lv, MUIM_NList_Clear);
 	buf = FileToBuffer(smf, NULL);
 	if(buf)
 		{
			set(lv, MUIA_NList_Quiet, MUIV_NList_Quiet_Full);
			if((buf[dst] == 0x0e) || (buf[dst] == 0xe0))
				{
					int i;
					int tag;
						
					dst++;
					nbserv = SWAPLONG(*(unsigned int *)&buf[dst]);
					dst += 4;
					for(i = 0; i < nbserv; i++)
						{
							int nbtags;	
          					struct Server *sv = calloc(sizeof(struct Server), 1);
							struct ServerNode *snode;
							
							if(sv) 
								{						
									sprintf(sv->IP, "%d.%d.%d.%d", buf[dst], buf[dst + 1], buf[dst + 2], buf[dst + 3]); 
									D(DBF_SERVER, "IP %s", sv->IP); 
									dst += 4;
									sv->Port = SWAPWORD(*(unsigned short *)&buf[dst]);
									D(DBF_SERVER, "Port %d", sv->Port);
									dst += 2;
									nbtags = SWAPLONG(*(unsigned int *)&buf[dst]);
									D(DBF_SERVER, "nbtags %d",  nbtags);
									dst += 4;
									if(nbtags < 18) for(tag = 0; tag < nbtags + 2; tag++) dst += parse_opcodes(buf + dst,  sv); 
									//list_insert_tail(&G->list_servers, &sv->node);
									//DoMethod(lv, MUIM_NList_InsertSingle, sv, MUIV_NList_Insert_Bottom); 
								}
							if((snode = CreateServerNode(sv, 0)) != NULL)
								{
									AddServerNode(G->ServerMet, snode);
									DoMethod(lv, MUIM_NList_InsertSingle, snode->server, MUIV_NList_Insert_Bottom); 
								}
						}
				}
			free(buf);
			set(lv, MUIA_NList_Quiet, MUIV_NList_Quiet_None);
			DoMethod(lv, MUIM_ServerPage_Update, nbserv); //G->ServerMet->count);
			result = 1;
  		}
  	else W(DBF_SERVER, "ATTENTION ! Impossible d'ouvrir le fichier '%s'", smf); 
  	RETURN(result);
  	return(result);
}

///
/// parse_opcodes                                                       
/// Analyse les données venant du fichier server.met et 
///  les transmet pour la listview des serveurs                
int parse_opcodes(unsigned char *buf,  struct Server *sv)
{
 	LONG datalen;
   	int offset = 0;
	
   	switch(buf[offset++])
    	{
         	case 0x02:
           		switch(datalen = SWAPWORD(*(unsigned short *)&buf[offset]))
               		{
                  		case 0x01:
                     		offset += 2;
                     		switch(buf[offset++])
                        		{
                        			case ST_SERVERNAME:  // nom
										{
                           					datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                           					offset += 2;
											sv->Name[datalen + 2] = '\0'; 
											strlcpy(&sv->Name[0], &buf[offset], datalen + 1);
			      							D(DBF_SERVER, "-- 0x02/case0x01 ... 0x01 nom %s longueur %d", sv->Name, datalen); 
                           					offset += datalen;
										}
                           				break;
                        			case ST_DESCRIPTION:  // description
										{
											datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                          					offset += 2;
											sv->Description[datalen + 2] = '\0'; 
                       						strlcpy(&sv->Description[0],  &buf[offset], datalen + 1);
	      									D(DBF_SERVER, "-- 0x02/case0x01 ... -- 0x0B description %s longueur %d",  sv->Description, datalen);
                       						offset += datalen;
                           				}
										break;
                        			case ST_FAIL: // échec
                           				datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
                           				offset += 2;
                           				sv->Failed = datalen;
			      						D(DBF_SERVER, "-- 0x02/case0x01 ... -- 0x0D failed %d", datalen);
                           				offset += datalen;
                           				break;
                        			case ST_DYNIP: //dynamique ip
                           				datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
			      						D(DBF_SERVER, "-- 0x02/case0x01 ... -- 0x85 dynip %ld", datalen);
			      						offset += 2;
			      						offset += datalen;
                           				break;
                        			case ST_MAXUSERS: //maxusers
                           				datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
			      						D(DBF_SERVER, " -- 0x02/case0x01 ... -- 0x87 maxusers %ld", datalen);
			      						sv->maxusers = datalen;
                          				offset += 2;
                           				offset += datalen;
                           				break;
									case ST_VERSION: //version
										datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
										offset += 2;
										sv->Version[datalen + 2] = '\0'; 
                          				strlcpy(&sv->Version[0],  &buf[offset], datalen + 1);					
			      						D(DBF_SERVER, "-- 0x02/case0x01 ... -- 0x91 version %s", sv->Version);
										offset += datalen;
			      						break;
                        			case ST_AUXPORTSLIST:  //auxportslist
			      						datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
			      						D(DBF_SERVER, "-- 0x02/case0x01 ... -- 0x93 auxportslist %ld", datalen);
			      						offset += 2;
			      						offset += datalen;
			      						break;
			   						default:
                           				D(DBF_SERVER, "0x02/case0x01--> oups, l'implementation du tag (%02x) n'est pas encore fait", buf[offset - 1]);
			      						datalen = SWAPWORD(*(unsigned short *)&buf[offset]);
			      						offset += 2;
			      						offset += datalen;
			    						break;
                        		}
                     		break;
		  				case 0x02:
                     		offset += 2;
		     				break;
		  				default:
		    				D(DBF_SERVER, "0x02--> oups, l'implementation du tag (%02x) n'est pas encore fait", buf[offset - 1]);
		     				break;
               		}
            	break;
         	case 0x03:
            	switch(datalen = SWAPWORD(*(unsigned short *)&buf[offset]))
            		{
               			case 0x01:
                   			offset += 2;
                   			switch(buf[offset++])
                       			{
                       				case OP_OFFERFILES:  // sources
                           				datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
			   							sv->Offerfiles = datalen;
			   							D(DBF_SERVER, " -- 0x03/case0x01 ... -- 0x15 sources %ld", datalen);
                           				offset += 4;
                           				break;
                       				case ST_PING: //ping
                           				datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
                           				sv->Ping = datalen;
			   							D(DBF_SERVER, " -- 0x03/case0x01 ... -- 0x0C ping %ld", datalen);
                           				offset += 4;
                           				break;
									case ST_PORT: //port
                              			datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
			      						D(DBF_SERVER, "-- 0x03/case0x01 ... -- 0x0F port %ld", datalen);
                              			offset += 4;
                              			break;
                           			case ST_PREFERENCE:  // preferences
                              			datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
										sv->Prefs = datalen;
			      						D(DBF_SERVER, " -- 0x0E prefs %d", datalen);
                              			offset += 4;
                              			break;
									case ST_MAXUSERS: //maxusers
									 	datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
									 	sv->maxusers = datalen;
									    D(DBF_SERVER, "-- 0x03/case0x01 ...  -- 0x87 maxusers %ld", datalen);
									  	offset += 4;
									  	break;
									case ST_HARDFILES: //hardfiles
									  	datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
									  	sv->hardfiles = datalen;
									  	D(DBF_SERVER, "-- 0x03/case0x01 ...  -- 0x89 hardfiles %ld", datalen);
									  	offset += 4;
									  	break;
									case ST_SOFTFILES: //softfiles
									  	datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
									  	sv->softfiles = datalen;
									  	D(DBF_SERVER, "-- 0x03/case0x01 ...  -- 0x88 softfiles %ld", datalen);
									  	offset += 4;
									  	break;
									case ST_LASTPING:
										datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
									  	D(DBF_SERVER, "-- 0x03/case0x01 ...  -- 0x90 lastping %ld", datalen);
									  	offset += 4;
										break;
			   						case ST_UDPFLAGS:
			      						datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
			      						D(DBF_SERVER, "-- 0x03/case0x01 ...  -- 0x92 udpflags %ld", datalen);
			      						offset += 2;
			      						break;
									case ST_LOWIDUSERS: //lowidusers
			      						datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
			      						sv->lowidusers = datalen;
									  	D(DBF_SERVER, "-- 0x03/case0x01 ...  -- 0x94 lowidusers %ld", datalen);
									    offset += 4;
									    break;
									case ST_UDPKEY:
										datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
			      						D(DBF_SERVER, "-- 0x03/case0x01 ...  -- 0x95 udpkey %ld",  datalen);
				  						offset += 4;
			      						break;
									case ST_UDPKEYIP:
										datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
			      						D(DBF_SERVER, "-- 0x03/case0x01 ...  -- 0x96 udpkeyip %ld",  datalen);
				  						offset += 4;
			      						break;
									case ST_TCPPORTOBFUSCATION:
										datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
			      						D(DBF_SERVER, "-- 0x03/case0x01 ...  -- 0x97 tcpportobfuscation %ld",  datalen);
				  						offset += 4;
			      						break;
									case ST_UDPPORTOBFUSCATION:
										datalen = SWAPLONG(*(unsigned int *)&buf[offset]);
			      						D(DBF_SERVER, "-- 0x03/case0x01 ...  -- 0x98 udpportobfuscation %ld",  datalen);
				  						offset += 4;
			      						break;
			   						default:
			    						D(DBF_SERVER, "0x03/case 0x01--> oups, l'implementation du tag (%02x) n'est pas encore fait", buf[offset - 1]);
			    						break;
                        		}
                     			break;
                  		case 0x05:
                     		offset+=2;
                     		switch(buf[offset++])
                        		{
                           			case 0x75: // Users
			     						offset += 4;
                             			datalen = SWAPLONG(*(unsigned int *) &buf[offset]);
			     						D(DBF_SERVER, "-- 0x03/case0x05 ...   -- 0x75 users %ld", datalen);
			     						sv->Users = datalen;
			     						offset += 4;
                             			break;
                           			case 0x66: // Files
			     						offset += 4;
                             			datalen = SWAPLONG(*(unsigned int *) &buf[offset]);
			     						D(DBF_SERVER, "-- 0x03/case0x05 ...   -- 0x66 files %ld", datalen);
			     						sv->Files = datalen;
			     						offset += 4;
                             			break;
                        		}
                     			break;
                  		case 0x08:
		      				offset+=2;
		      				switch(buf[offset++])
								{
			    					case 0x6C: //lowlimit_users
			       						offset += 8;
			       						datalen = SWAPLONG(*(unsigned int *) &buf[offset]);
			       						D(DBF_SERVER, "-- 0x03/case0x01 ...   -- 0x6C lowlimit %ld", datalen);
			       						sv->lowlimit_users = datalen;
			       						offset += 3;
			       						break;
			    					case 0x6D: // maxlimit_users
			       						offset += 8;
			       						datalen = SWAPLONG(*(unsigned int *) &buf[offset]);
			       						D(DBF_SERVER, " -- 0x6D maxlimit %ld", datalen);
			       						sv->maxlimit_users = datalen;
			       						offset += 3;
			       						break;
								}
		    					break;
						default:
		      				D(DBF_SERVER, "0x03--> oups, l'implementation du tag (%02x) n'est pas encore fait", buf[offset - 1]);
		      				break;
               		}
            	break;
		}
	return(offset);
}

void servers_cleanup(void)
{
   	//struct Server *entry;

   	//while((entry = (struct Server *)list_remove_tail(&G->list_servers)))
     // 	{
	 //		free(entry);
    //	}
}
