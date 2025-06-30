/***************************************************************************
 Bourriquet MorphOS
 2024-2025 digitally
 
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <proto/alib.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <hardware/byteswap.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>

#include "Extrasrc.h"

#include "Bourriquet.h"
#include "Debug.h"
#include "Global.h"
#include "Locale.h"
#include "MD4.h"
#include "ServerMet.h"
#include "Share.h"
#include "ShareList.h"
#include "FileInfo.h"
#include "Error.h"
#include "DashBoard.h"
#include "tcp/Connection.h"

void SearchDirShare(char *dname, char *extension)
{
   	char fileName[255];
   	FILE *fp;
   	unsigned char *buf3;
   	int x, j = 0;
   	struct FileInfoBlock *fib;
   	static unsigned char hash[16];
   	int partLen;
   	int bytesLeft;
   	char *tmp_parts;
   	char str[SIZE_DEFAULT];
	int nb_sharedfiles = 0;
	APTR lv = G->MA->GUI.LV_SHARED;
	
	if(FileExists(dname) == TRUE)
		{
		  	fib = AllocDosObject(DOS_FIB, NULL);
   			if(fib)
      			{
         			BPTR lock, lockfile;

         			lock = Lock(dname, SHARED_LOCK);
         			if(lock)
            			{
               				Examine(lock, fib);
               				while(ExNext(lock, fib) && (IoErr() != ERROR_NO_MORE_ENTRIES))
                  				{
                     				if(extension != NULL)
                        				{
											D(DBF_SHARE, "extension != NULL");
                        				}
                     				else
                        				{
                           					sprintf(fileName, "%s/%s", dname, fib->fib_FileName);
                           					sprintf(str, "%s", IdentifyFile(fib->fib_FileName));
											D(DBF_SHARE, "fichier %s ... identification %s", fileName, str);
                           					lockfile = Lock(fileName, SHARED_LOCK);
                           					if(lockfile)
                              					{
                                 					if(fib->fib_Size)
                                    					{                                       
				       										struct Share *sh;
															//struct ShareNode *shnode;
															
															if((sh = calloc(sizeof(struct Share), 1)) != NULL)
                                          						{
                                             						fp = fopen(fileName, "rb");
                                             						if(fp)
                                                						{
																			MD4_CTX ctx;

                                                   							buf3 = malloc(PARTSIZE);
													                		sh->nb_parts = (fib->fib_Size / PARTSIZE) + ((fib->fib_Size % PARTSIZE) ? 1 : 0);
																			sh->FileName[strlen(fib->fib_FileName) + 2] = '\0'; 
                       														strlcpy(&sh->FileName[0],  &fib->fib_FileName[0], strlen(fib->fib_FileName) + 1);
                                                   							sh->Size = fib->fib_Size;
																			sh->FileType[strlen(str) + 2] = '\0'; 
                       														strlcpy(&sh->FileType[0],  &str[0], strlen(str) + 1);
                                  											tmp_parts = (unsigned char *)malloc(sh->nb_parts * 16 * sizeof(unsigned char));
                                                   							bytesLeft = fib->fib_Size;
																			D(DBF_SHARE, "parts: %ld, %ld", sh->nb_parts, bytesLeft);
						   													for(x = 1; x < sh->nb_parts; x++)
                                                      							{
                                                         							partLen = fread(buf3, 1, PARTSIZE, fp);
                                                         							MD4Init(&ctx);
                                                         							MD4Update(&ctx, (unsigned char *)buf3, partLen);
                                                         							MD4Final(hash, &ctx);
																					//memcpy(tmp_parts + ((x - 1) * 16), hash, 16);
                                                         							memcpy(tmp_parts + (x * 16), hash, 16);
                                                         							bytesLeft -= PARTSIZE;
							 														D(DBF_SHARE, " F2 : %s %x %x %x %x", hash_bin2hex(hash), hash[0], hash[1], hash[2], hash[3]);
                                                      							}
                                                   							partLen = fread(buf3, 1, bytesLeft, fp);
                                                   							MD4Init(&ctx);
                                                   							MD4Update(&ctx, (unsigned char *)buf3, partLen);
                                                   							MD4Final(hash, &ctx);
                                                   							memcpy(tmp_parts + ((sh->nb_parts - 1) * 16), hash, 16);
													                		//sh->part_hash = (char *)mystrdup(tmp_parts);
                                                   							if(sh->nb_parts > 1)
                                                      							{
                                                         							MD4Init(&ctx);
                                                         							MD4Update(&ctx, (unsigned char *)tmp_parts, sh->nb_parts * 16);
                                                         							MD4Final(hash, &ctx);
                                                      							}
																			sh->FileID[strlen(hash) + 2] = '\0'; 
                       														strlcpy(&sh->FileID[0],  &hash[0], strlen(hash) + 1);
					   	   													D(DBF_SHARE," F2 : %s %x %x %x %x", hash_bin2hex(hash), hash[0], hash[1], hash[2], hash[3]);
																   			sh->Permission[strlen("Public") + 2] = '\0'; 
                       														strlcpy(&sh->Permission[0],  "Public", strlen("Public") + 1);
																			//sh->Permission = (char *)mystrdup("Public");
																   			j++;
																   			sh->count += j;
																   			sh->status = 4;
																   			nb_sharedfiles++;
																   			list_insert_tail(&G->list_shared, &sh->node);
																			/*if((shnode = CreateShareNode(sh, 0)) != NULL)
																				{
																					AddShareNode(G->ShareFiles, shnode);
																					DoMethod(lv, MUIM_NList_InsertSingle, shnode->share, MUIV_NList_Insert_Bottom); 
																				}*/
                                                   							free(buf3);
                                                   							free(tmp_parts);
                                                   							fclose(fp);
                                                						}
                                          						}
                                    					}
                                 					UnLock(lockfile);
                              					}
											else D(DBF_SHARE, "no fileinfo "); 
                        				}
								}
               				UnLock(lock);
            			}
         			FreeDosObject(DOS_FIB, fib);
      			}
			else D(DBF_SHARE, "no fib"); 
		}
	else ER_NewError("Aucun répertoire trouvé pour les fichiers partagés.\nVeuillez remplir le champ SharedDir = de votre fichier .config svp"); 
}

/*************************************************************************************************/
int write_edk_tags_shared(unsigned char *buf, int tag, struct Share *sh)
{
   	int offset = 0;

   	switch(tag)
      	{
         	case OP_EMULEINFO:
            	buf[offset] = 0x02;
            	buf[offset + 1] = 0x01;
            	buf[offset + 2] = 0x00;
            	buf[offset + 3] = 0x01;
            	offset += 4;
            	*(unsigned short *)&buf[offset] = SWAPWORD(strlen(sh->FileName));
            	offset += 2;
            	memcpy(buf + offset, sh->FileName, strlen(sh->FileName));
            	offset += strlen(sh->FileName);
            	break;
         	case FT_FILESIZE:
            	buf[offset] = 0x03;
            	buf[offset + 1] = 0x01;
            	buf[offset + 2] = 0x00;
            	buf[offset + 3] = 0x02;
            	offset += 4;
            	*(unsigned int *) &buf[offset] = SWAPLONG(sh->Size);
            	offset += 4;
            	break;
         	case FT_FILETYPE:
            	buf[offset] = 0x02;
            	buf[offset + 1] = 0x01;
            	buf[offset + 2] = 0x00;
            	buf[offset + 3] = 0x03;
            	offset += 4;
            	*(unsigned short *)&buf[offset] = SWAPWORD(strlen(sh->FileType));
            	offset += 2;
            	memcpy(buf + offset, sh->FileType, strlen(sh->FileType));
            	offset += strlen(sh->FileType);
            	break;
      	}
   return(offset);
}