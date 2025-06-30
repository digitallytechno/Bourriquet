/***************************************************************************
Bourriquet MorphOS
 Copyright (C) 2024-2025 digitally
  
***************************************************************************/
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include <netinet/in.h>

#include <clib/alib_protos.h>
#include <dos/doshunks.h>
#include <dos/dostags.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <libraries/asl.h>
#include <libraries/gadtools.h>
#include <libraries/openurl.h>
#include <mui/BetterString_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/NListview_mcc.h>
#include <mui/TextEditor_mcc.h>
#include <workbench/startup.h>
#include <proto/codesets.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <proto/iffparse.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/keymap.h>
#include <proto/layers.h>
#include <proto/locale.h>
#include <proto/muimaster.h>
#include <proto/openurl.h>
#include <proto/timer.h>
#include <proto/utility.h>
#include <proto/wb.h>

#include "Extrasrc.h"

#include "SDI_hook.h"
#include "SDI_stdarg.h"
#include "timeval.h"

#include "Bourriquet.h"
#include "Global.h"

#include "mui/ClassesExtra.h"
#include "mui/BourriquetApplication.h"

#include "Locale.h"
#include "MethodStack.h"
#include "MUIObjects.h"
#include "Threads.h"
#include "Debug.h"
#include "FileInfo.h"

#define CRYPTBYTE 164

#if !defined(SDTA_Cycles)
#define SDTA_Cycles           (SDTA_Dummy + 6)
#endif

struct UniversalClassData
{
 	struct UniversalGUIData { Object *WI; } GUI;
};

int BusyLevel = 0;
/***************************************************************************
 Utilitaires
***************************************************************************/
//  Detecte le type du fichier en utilisant la datatypes.library
char *IdentifyFileDT(char *fname)
{
   	static char ctype[SIZE_CTYPE], *type = NULL;

   	strcpy(ctype, "application");
   	if(DataTypesBase)
      	{
         	BPTR lock;

         	if((lock = Lock(fname, ACCESS_READ)))
            	{
               		struct DataType *dtn;

               		if((dtn = ObtainDataTypeA(DTST_FILE, (APTR)lock, NULL)))
                  		{
                     		struct DataTypeHeader *dth = dtn->dtn_Header;

                     		switch(dth->dth_GroupID)
                        		{
                           			case GID_SYSTEM:
                           		   		break;
                           			case GID_DOCUMENT:
                              			type = "application";
                              			break;
                           			case GID_TEXT:
                              			type = "text";
                              			break;
                           			case GID_SOUND:
                           			case GID_INSTRUMENT:
                              			type = "audio";
                              			break;
                           			case GID_PICTURE:
                              			type = "image";
                              			break;
                           			case GID_MOVIE:
                           			case GID_ANIMATION:
                              			type = "video";
                              			break;
                        		}
                     		if(type) sprintf(ctype, "%s", dth->dth_BaseName);
                     		ReleaseDataType(dtn);
                  		}
               		UnLock(lock);
            	}
      	}
   return(ctype);
}

//  Detecte le type de fichier en analysant l'extension du fichier
char *IdentifyFile(char *fname)
{
   	char *ctype = "";
   	char *ext;

   	if((ext = strrchr(fname, '.'))) ++ext; else ext = "--";
   	if(!stricmp(ext, "htm") || !stricmp(ext, "html")) ctype = GetStr(MSG_RS_TYPE_ARCHIVE); //ContType[1];
   	else if(!strcmp(ext, "txt")) ctype = GetStr(MSG_RS_TYPE_ARCHIVE); //ContType[1];
   	else if(!strcmp(ext, "rar")) ctype = GetStr(MSG_RS_TYPE_ARCHIVE); //ContType[1];
   	else if(!strcmp(ext, "ace")) ctype = GetStr(MSG_RS_TYPE_ARCHIVE); //ContType[1];
   	else if(!strcmp(ext, "guide")) ctype = GetStr(MSG_RS_TYPE_ARCHIVE); //ContType[2];
   	else if(!strcmp(ext, "ps") || !stricmp(ext, "eps")) ctype = "Archive"; //ContType[4];
   	else if(!strcmp(ext, "rtf")) ctype = GetStr(MSG_RS_TYPE_ARCHIVE); //ContType[5];
   	else if(!strcmp(ext, "lha")) ctype = GetStr(MSG_RS_TYPE_ARCHIVE); //ContType[6];
   	else if(!strcmp(ext, "lzx")) ctype = GetStr(MSG_RS_TYPE_ARCHIVE); //ContType[7];
   	else if(!strcmp(ext, "zip")) ctype = GetStr(MSG_RS_TYPE_ARCHIVE); //ContType[8];
   	else if(!strcmp(ext, "rexx") || !stricmp(ext + strlen(ext) - 2, "rx")) ctype = GetStr(MSG_RS_TYPE_ARCHIVE); //ContType[11];
   	else if(!strncmp(ext, "jpg", 3)) ctype = GetStr(MSG_RS_TYPE_PICTURE); //"image/jpeg";
   	else if(!strncmp(ext, "gif", 3)) ctype = GetStr(MSG_RS_TYPE_PICTURE); //"image/gif";
   	else if(!strncmp(ext, "png", 3)) ctype = GetStr(MSG_RS_TYPE_PICTURE); //"image/png";
   	else if(!strncmp(ext, "tiff", 4)) ctype = GetStr(MSG_RS_TYPE_PICTURE); //"image/tiff";
   	else if(!strncmp(ext, "bmp", 3)) ctype = GetStr(MSG_RS_TYPE_PICTURE);
   	else if(!strcmp(ext, "au") || !stricmp(ext, "snd")) ctype = GetStr(MSG_RS_TYPE_AUDIO); //ContType[17];
   	else if(!strcmp(ext, "wav")) ctype = GetStr(MSG_RS_TYPE_AUDIO); //ContType[19];
   	else if(!strcmp(ext, "mpg") || !stricmp(ext, "mpeg")) ctype = GetStr(MSG_RS_TYPE_VIDEO); //ContType[20];
   	else if(!strcmp(ext, "qt") || !stricmp(ext, "mov")) ctype = GetStr(MSG_RS_TYPE_VIDEO); //ContType[21];
   	else if(!strcmp(ext, "avi")) ctype = GetStr(MSG_RS_TYPE_VIDEO); //ContType[23];
   	//else if(!strcmp(ext, "mp3")) ctype = GetStr(MSG_RS_TYPE_MUSIC);
   	if(!*ctype) ctype = IdentifyFileDT(fname);
   	return(ctype);
}

char *mystrdup(const char *str)
{
   char *new_str;
   int len;

   if(!str) return NULL;
   len = strlen(str);
   if(!len) return NULL;
   if((new_str = (char *)malloc(len + 1))) strcpy(new_str, str);
   return(new_str);
}

char *TrimStart(const char *s)
{
 	ENTER();
  	while(*s != '\0' && isspace(*s)) s++;
  	RETURN(s);
  	return(char *)s;
}

///
/// TrimEnd
//  Removes trailing spaces
char *TrimEnd(char *s)
{
 	char *e = s+strlen(s)-1;

  	ENTER();
  	while(e >= s && isspace(*e)) *e-- = '\0';
	RETURN(s);
  	return(s);
}

///
/// Trim
// Removes leading and trailing spaces
char *Trim(char *s)
{
 	ENTER();
  	if(s != NULL)
  		{
   	 		s = TrimStart(s);
    		s = TrimEnd(s);
  		}
  	RETURN(s);
  	return(s);
}

char *AddPath(char *dst, const char *src, const char *add, const size_t size)
{
 	ENTER();
    strlcpy(dst, src, size);
    if(AddPart(dst, add, size) == FALSE)
    	{
       		E(DBF_ALWAYS, "AddPath()/AddPart() dépassement du buffer détécté!");
       		dst = NULL;
    	}
    RETURN(dst);
    return(dst);
}

/// GetDateStamp
//  Get number of seconds since 1/1-1978
ULONG GetDateStamp(void)
{
 	struct DateStamp ds;
    ULONG seconds;

    ENTER();
    DateStamp(&ds);
    seconds = ds.ds_Days * 24 * 60 * 60 + ds.ds_Minute * 60 + ds.ds_Tick / TICKS_PER_SECOND;
    RETURN(seconds);
    return(seconds);
}

///
/// FormatSize
//  Displays large numbers using group separators
void FormatSize(LONG size, char *buf, int buflen, enum SizeFormat forcedPrecision)
{
 	ENTER();
	if(size >= 0)
		{
   			const char *dp = G->Locale ? (const char *)G->Locale->loc_DecimalPoint : ".";
   			double dsize = (double)size;

   			// see if the user wants to force a precision output or if he simply
   			// wants to output based on C->SizeFormat (forcedPrecision = SF_AUTO)
   			if(forcedPrecision == SF_AUTO) forcedPrecision = SF_AUTO; //C->SizeFormat;
   			// we check what SizeFormat the user has choosen
   			switch(forcedPrecision)
   				{
   					// the precision modes use sizes as base of 2
   					enum { KB = 1000, MB = 1000 * 1000, GB = 1000 * 1000 * 1000 };

   					case SF_1PREC:
   						{
       						char *p;

       						if(size < KB) snprintf(buf, buflen, "%d %s", (unsigned int)size, " ");
       						else if(size < MB) snprintf(buf, buflen, "%.1f %s", dsize/KB, "K");
       						else if(size < GB) snprintf(buf, buflen, "%.1f %s", dsize/MB, "M");
       						else snprintf(buf, buflen, "%.1f %s", dsize/GB, "G");
       						if((p = strchr(buf, '.')) != NULL) *p = *dp;
   						}
   						break;    
   					case SF_2PREC:
   						{
       						char *p;

       						if(size < KB) snprintf(buf, buflen, "%d %s", (unsigned int)size, " ");
							else if(size < MB) snprintf(buf, buflen, "%.2f %s", dsize/KB, "K");
							else if(size < GB) snprintf(buf, buflen, "%.2f %s", dsize/MB, "M");
							else snprintf(buf, buflen, "%.2f %s", dsize/GB, "G");
					        if((p = strchr(buf, '.')) != NULL) *p = *dp;
   						}
   						break;
			      case SF_3PREC:
   						{
       						char *p;

       						if(size < KB) snprintf(buf, buflen, "%d %s", (unsigned int)size, " ");
							else if(size < MB) snprintf(buf, buflen, "%.3f %s", dsize/KB, "K");
							else if(size < GB) snprintf(buf, buflen, "%.3f %s", dsize/MB, "M");
							else snprintf(buf, buflen, "%.3f %s", dsize/GB, "G");
							if((p = strchr(buf, '.')) != NULL) *p = *dp;
						}
					  	break;
					case SF_MIXED:
					  	{
							char *p;

							if(size < KB) snprintf(buf, buflen, "%d %s", (unsigned int)size," ");
							else if(size < MB) snprintf(buf, buflen, "%.1f %s", dsize/KB, "K");
							else if(size < GB) snprintf(buf, buflen, "%.2f %s", dsize/MB, "M");
							else snprintf(buf, buflen, "%.3f %s", dsize/GB, "G");
							if((p = strchr(buf, '.')) != NULL) *p = *dp;
					  	}
					  	break;
				     case SF_AUTO:
   						default:
   							{
       							const char *gs = G->Locale ? (const char *)G->Locale->loc_GroupSeparator : ",";
       							// as we just split the size to another value, we redefine the KB/MB/GB values to base 10 variables
       							enum { KiB = 1000, MiB = 1000 * 1000, GiB = 1000 * 1000 * 1000 };

								if(size < KiB) snprintf(buf, buflen, "%d %s", (unsigned int)size, " ");
								else if(size < MiB)
									{
								  		ldiv_t k;

								  		k = ldiv(size, KiB);
							  			snprintf(buf, buflen, "%d%s%03d %s", (unsigned int)k.quot, gs, (unsigned int)k.rem, " ");
									}
								else if(size < GiB)
									{
								  		ldiv_t m, k;

								  		m = ldiv(size, MiB);
								  		k = ldiv(m.rem, KiB);
							  			snprintf(buf, buflen, "%d%s%03d%s%03d %s", (unsigned int)m.quot, gs, (unsigned int)k.quot, gs, (unsigned int)k.rem, " ");
									}
								else
									{
								  		ldiv_t g, m, k;

								  		g = ldiv(size, GiB);
								  		m = ldiv(g.rem, MiB);
								  		k = ldiv(m.rem, KiB);
							  			snprintf(buf, buflen, "%d%s%03d%s%03d%s%03d %s", (unsigned int)g.quot, gs, (unsigned int)m.quot, gs, (unsigned int)k.quot, gs, (unsigned int)k.rem, " ");
									}
   							}
   							break;
    			}
  		}
  	else strlcpy(buf, "TAILLE INCONNUE", buflen);
   	LEAVE();
}

/*** MUI related ***/
/// SafeOpenWindow
//  Tries to open a window
BOOL SafeOpenWindow(Object *obj)
{
 	BOOL success = FALSE;

    ENTER();
    if(obj != NULL)
     	{
        	set(obj, MUIA_Window_Open, TRUE);
        	if(xget(obj, MUIA_Window_Open) == TRUE || xget(_app(obj), MUIA_Application_Iconified) == TRUE) success = TRUE;  
      	}
    if(success == FALSE && obj != NULL)
    	{
       		E(DBF_ALWAYS, "Impossibilité d'ouvrir la fenêtre avec le titre '%s'", (char *)xget(obj, MUIA_Window_Title));
       		DisplayBeep(_screen(obj));
    	}
    RETURN(success);
    return(success);
}

void DisposeModule(void *modptr)
{
 	struct UniversalClassData **module = (struct UniversalClassData **)modptr;

    ENTER();
    if(*module != NULL)
    	{
       		Object *window = (*module)->GUI.WI;
       		D(DBF_UTIL, "suppression de la fenêtre %08lx", window);
       		DoMethod(_app(window), MUIM_BourriquetApplication_DisposeWindow, window);
       		free(*module);
       		*module = NULL;
    	}
    LEAVE();
}

HOOKPROTONHNO(DisposeModuleFunc, void, void **arg)
{
	DisposeModule(arg[0]);
}
MakeHook(DisposeModuleHook,DisposeModuleFunc);

/*** GFX related ***/
/// struct LayerHookMsg
struct LayerHookMsg
{
	struct Layer *layer;
	struct Rectangle bounds;
	LONG offsetx;
	LONG offsety;
};

///
/// struct BltHook
struct BltHook
{
 	struct Hook hook;
  	struct BitMap maskBitMap;
  	struct BitMap *srcBitMap;
  	LONG srcx,srcy;
  	LONG destx,desty;
};

///
/// MyBltMaskBitMap
static void MyBltMaskBitMap(const struct BitMap *srcBitMap, LONG xSrc, LONG ySrc, struct BitMap *destBitMap, LONG xDest, LONG yDest, LONG xSize, LONG ySize, struct BitMap *maskBitMap)
{
 	ENTER();
  	BltBitMap((struct BitMap *)srcBitMap,xSrc,ySrc,destBitMap, xDest, yDest, xSize, ySize, 0x99,~0,NULL);
  	BltBitMap(maskBitMap,xSrc,ySrc,destBitMap, xDest, yDest, xSize, ySize, 0xe2,~0,NULL);
  	BltBitMap((struct BitMap *)srcBitMap,xSrc,ySrc,destBitMap, xDest, yDest, xSize, ySize, 0x99,~0,NULL);
  	LEAVE();
}

///
/// BltMaskHook
HOOKPROTO(BltMaskFunc, void, struct RastPort *rp, struct LayerHookMsg *msg)
{
 	struct BltHook *h = (struct BltHook*)hook;
	LONG width = msg->bounds.MaxX - msg->bounds.MinX+1;
	LONG height = msg->bounds.MaxY - msg->bounds.MinY+1;
	LONG offsetx = h->srcx + msg->offsetx - h->destx;
	LONG offsety = h->srcy + msg->offsety - h->desty;
  
	MyBltMaskBitMap(h->srcBitMap, offsetx, offsety, rp->BitMap, msg->bounds.MinX, msg->bounds.MinY, width, height, &h->maskBitMap);
}
MakeStaticHook(BltMaskHook, BltMaskFunc);

///
/// MyBltMaskBitMapRastPort
void MyBltMaskBitMapRastPort(struct BitMap *srcBitMap, LONG xSrc, LONG ySrc, struct RastPort *destRP, LONG xDest, LONG yDest, LONG xSize, LONG ySize, ULONG minterm, APTR bltMask)
{
 	ENTER();
  	if(GetBitMapAttr(srcBitMap, BMA_FLAGS) & BMF_INTERLEAVED)
  		{
    		LONG src_depth = GetBitMapAttr(srcBitMap, BMA_DEPTH);
    		struct Rectangle rect;
    		struct BltHook hook;

    		// Define the destination rectangle in the rastport
    		rect.MinX = xDest;
    		rect.MinY = yDest;
    		rect.MaxX = xDest + xSize - 1;
    		rect.MaxY = yDest + ySize - 1;
    		// Initialize the hook
    		InitHook(&hook.hook, BltMaskHook, NULL);
    		hook.srcBitMap = srcBitMap;
    		hook.srcx = xSrc;
    		hook.srcy = ySrc;
    		hook.destx = xDest;
    		hook.desty = yDest;
    		// Initialize a bitmap where all plane pointers points to the mask
    		InitBitMap(&hook.maskBitMap, src_depth, GetBitMapAttr(srcBitMap, BMA_WIDTH), GetBitMapAttr(srcBitMap, BMA_HEIGHT));
    		while(src_depth)
    			{
    				hook.maskBitMap.Planes[--src_depth] = bltMask;
    			}
    			// Blit onto the Rastport */
    		DoHookClipRects(&hook.hook, destRP, &rect);
  		}
  	else BltMaskBitMapRastPort(srcBitMap, xSrc, ySrc, destRP, xDest, yDest, xSize, ySize, minterm, bltMask);
  	LEAVE();
}

///
/// GotoURLPossible
//  Check whether there is some kind of openurl.library or OS4.1's URL: device available
BOOL GotoURLPossible(void)
{
 	BOOL gotoURLPossible = FALSE;
  
    ENTER();
    if(gotoURLPossible == FALSE && OpenURLBase != NULL)
    	{
      		gotoURLPossible = TRUE;
       		D(DBF_UTIL, "openurl.library trouvée");
    	}
    RETURN(gotoURLPossible);
    return(gotoURLPossible);
}

///
/// GotoURL
//  Loads an URL using an ARexx script or openurl.library
BOOL GotoURL(const char *url, const BOOL newWindow)
{
 	BOOL wentToURL = FALSE;

    ENTER();
    if(wentToURL == FALSE && OpenURLBase != NULL)
    	{
       		D(DBF_UTIL, "openurl.library pour ouvrir l'url '%s'", url);
       		wentToURL = URL_Open((STRPTR)url, URL_NewWindow, newWindow, TAG_DONE);
   		}
   	if(wentToURL == FALSE) W(DBF_UTIL, "L'ouverture de l'url '%s' n'a pas réussi", url);
   	else D(DBF_UTIL, "L'ouverture de l'url '%s' a réussi", url);
   	RETURN(wentToURL);
   	return(wentToURL);
}

///
/// strippedCharsetName()
// return the charset code stripped and without any white spaces
char *strippedCharsetName(const struct codeset *codeset)
{
  char *name = (char *)"";

  ENTER();

  if(codeset != NULL)
  {
    char *strStart = TrimStart(codeset->name);
    char *strEnd = strchr(strStart, ' ');

    if(strEnd > strStart || strStart > codeset->name)
    {
      static char strippedName[SIZE_CTYPE+1];

      if(strEnd > strStart && (size_t)(strEnd-strStart) < sizeof(strippedName))
        strlcpy(strippedName, strStart, strEnd-strStart+1);
      else
        strlcpy(strippedName, strStart, sizeof(strippedName));

      name = strippedName;
    }
    else
      name = codeset->name;
  }

  RETURN(name);
  return name;
}

///
/// GetPubScreenName
// return the name of a public screen, if the screen is public
void GetPubScreenName(const struct Screen *screen, char *pubName, ULONG pubNameSize)
{
  ENTER();

  // we use "Workbench" as the default public screen name
  strlcpy(pubName, "Workbench", pubNameSize);

  if(screen != NULL)
  {
    // try to get the public screen name
    
    struct List *pubScreenList;

    // on all other systems we have to obtain the public screen name in the hard way
    // first get the list of all public screens
    if((pubScreenList = LockPubScreenList()) != NULL)
    {
      struct PubScreenNode *psn;

      // then iterate through this list
      IterateList(pubScreenList, struct PubScreenNode *, psn)
      {
        // check if we found the given screen
        if(psn->psn_Screen == screen)
        {
          // copy the name and get out of the loop
          strlcpy(pubName, psn->psn_Node.ln_Name, pubNameSize);
          break;
        }
      }

      // unlock the list again
      UnlockPubScreenList();
    }
  }

  LEAVE();
}

///
/// TimeHasElapsed
// check wheter the given number of microsecond has passed since the last
// check specified by <last>. If yes, then <last> will be set to the current
// time
BOOL TimeHasElapsed(struct TimeVal *last, ULONG micros)
{
  struct TimeVal now;
  struct TimeVal delta;
  BOOL elapsed = FALSE;

  ENTER();

  // get the current time
  GetSysTime(TIMEVAL(&now));
  delta = now;

  // substract the known last time
  SubTime(TIMEVAL(&delta), TIMEVAL(last));

  // check whether either one second or the number of microseconds has passed
  if(delta.Seconds > 0 || delta.Microseconds > micros)
  {
    // remember the current time and signal success
    *last = now;
    elapsed = TRUE;
  }

  RETURN(elapsed);
  return elapsed;
}

///

/*** REXX interface support ***/
/// AllocReqText
//  Prepare multi-line text for requesters, converts \n to line breaks
char *AllocReqText(const char *s)
{
  char *reqtext;

  ENTER();

  if((reqtext = calloc(strlen(s) + 1, 1)) != NULL)
  {
    char *d = reqtext;

    while(*s != '\0')
    {
      if(s[0] == '\\' && s[1] == 'n')
      {
        *d++ = '\n';
        s++;
        s++;
      }
      else
        *d++ = *s++;
    }
  }

  RETURN(reqtext);
  return reqtext;
}

///
/// ToLowerCase
//  Change a complete string to lower case
void ToLowerCase(char *str)
{
  char c;

  ENTER();

  while ((c = *str) != '\0')
    *str++ = tolower(c);

  LEAVE();
}

///
/// WriteUInt32
// write a 32bit variable to a stream, big endian style
// no ENTER/RETURN macro calls on purpose as this would blow up the trace log too much
int WriteUInt32(FILE *stream, ULONG value)
{
  int n;

  // convert the value to big endian style
  value = htonl(value);

  n = fwrite(&value, sizeof(value), 1, stream);

  return n;
}

///
/// ReadUInt32
// read a 32bit variable from a stream, big endian style
// no ENTER/RETURN macro calls on purpose as this would blow up the trace log too much
int ReadUInt32(FILE *stream, ULONG *value)
{
  int n;

  if((n = fread(value, sizeof(*value), 1, stream)) == 1)
  {
    // convert the value to big endian style
    *value = ntohl(*value);
  }

  return n;
}

///
/// DuplicateNode
// create a copy of an existing node using AllocSysObject()
void *DuplicateNode(const void *node, const size_t size)
{
  void *dup;

  ENTER();

  if((dup = AllocSysObjectTags(ASOT_NODE,
    ASONODE_Size, size,
    ASONODE_Min, TRUE,
    TAG_DONE)) != NULL)
  {
    memcpy(dup, node, size);
  }

  RETURN(dup);
  return dup;
}

///
/// CountNodes
//  returns the number of nodes currently in a struct List
ULONG CountNodes(const struct MinList *list)
{
  struct MinNode *curNode;
  ULONG result = 0;

  ENTER();

  IterateList(list, struct MinNode *, curNode)
    result++;

  RETURN(result);
  return result;
}

///
/// CompareLists
// compare two lists using a comparison function
BOOL CompareLists(const struct List *lh1, const struct List *lh2, BOOL (* compare)(const struct Node *, const struct Node *))
{
  BOOL equal = TRUE;
  struct Node *ln1;
  struct Node *ln2;

  ENTER();

  ln1 = GetHead((struct List *)lh1);
  ln2 = GetHead((struct List *)lh2);

  // walk through both lists in parallel and compare the single nodes
  while(ln1 != NULL && ln2 != NULL)
  {
    // compare the two nodes
    if(compare(ln1, ln2) == FALSE)
    {
      // something does not match
      equal = FALSE;
      break;
    }

    // advance to the next nodes in each list
    ln1 = GetSucc(ln1);
    ln2 = GetSucc(ln2);
  }

  if(equal == TRUE)
  {
    // if both lists are equal so far, but there are any nodes left in either list
    // then the two lists cannot be equal
    if(ln1 != NULL || ln2 != NULL)
      equal = FALSE;
  }

  RETURN(equal);
  return equal;
}

/*
///
/// SortExecList
// sort an Exec list using merge sort
void SortExecList(struct MinList *lh, int (* compare)(const struct MinNode *, const struct MinNode *))
{
  struct List list[2], *from, *to;
  size_t insize;

  from = &list[0];
  to = &list[1];
  NewList(from);
  NewList(to);

  MoveList(from, (struct List *)lh);

  insize = 1;

  while(TRUE)
  {
    size_t nmerges, psize, qsize, i;
    struct Node *p, *q, *e;

    NewList(to);
    p = GetHead(from);
    nmerges = 0;

    while(p->ln_Succ != NULL)
    {
      nmerges++;
      q = p;
      psize = 0;
      for(i = 0; i < insize; i++)
      {
        if(q->ln_Succ != NULL && q->ln_Succ->ln_Succ == NULL)
          break;

        q = q->ln_Succ;
        psize++;
      }

      qsize = insize;

      while(psize > 0 || (qsize > 0 && q->ln_Succ != NULL))
      {
        if(psize == 0)
        {
          e = q;
          q = q->ln_Succ;
          qsize--;
        }
        else if(qsize == 0 || q->ln_Succ == NULL)
        {
          e = p;
          p = p->ln_Succ;
          psize--;
        }
        else if(compare((const struct MinNode *)p, (const struct MinNode *)q) <= 0)
        {
          e = p;
          p = p->ln_Succ;
          psize--;
        }
        else
        {
          e = q;
          q = q->ln_Succ;
          qsize--;
        }

        Remove(e);
        AddTail(to, e);
      }

      p = q;
    }

    if(nmerges <= 1)
    {
      break;
    }
    else
    {
      struct List *tmp;

      tmp = from;
      from = to;
      to = tmp;

      NewList(to);

      insize *= 2;
    }
  }

  MoveList((struct List *)lh, to);
}
*/
/*
///
/// GetNthNode
// get the n-th node of a list
struct MinNode *GetNthNode(const struct MinList *list, ULONG n)
{
  struct MinNode *result = NULL;
  struct MinNode *curNode;
  ULONG nn;

  ENTER();

  nn = 0;
  IterateList(list, struct MinNode *, curNode)
  {
    if(nn == n)
    {
      result = curNode;
      break;
    }

    nn++;
  }

  RETURN(result);
  return result;
}
*/

///
/// SplitString
// split a string separated by certain delimiting characters into an array of words
char **SplitString(const char *str, const char *delimiters)
{
  char *copy;
  char **array = NULL;

  ENTER();

  if(str != NULL && (copy = strdup(str)) != NULL)
  {
    char *p = copy;
    LONG wordCnt;

    // count the number of words, we have one at least
    wordCnt = 1;
    while(*p != '\0')
    {
      if(strchr(delimiters, *p) != NULL)
        wordCnt++;

      p++;
    }

    if((array = calloc(wordCnt+1, sizeof(char *))) != NULL)
    {
      LONG i = 0;
      char *word = copy;

      // split the string
      do
      {
        char *e;

        if((e = strpbrk(word, delimiters)) != NULL)
          *e++ = '\0';

        array[i] = strdup(word);
        i++;

        word = e;
      }
      while(word != NULL);
    }

    free(copy);
  }

  RETURN(array);
  return array;
}

///
/// FreeStrArray
// free() a NULL terminated array of strings
void FreeStrArray(char **array)
{
  ENTER();

  if(array != NULL)
  {
    LONG i = 0;

    while(array[i] != NULL)
    {
      free(array[i]);
      i++;
    }

    free(array);
  }

  LEAVE();
}

///
///
/// FileToBuffer
//  Reads a complete file into memory
char *FileToBuffer(const char *file, size_t *outsize)
	{
  		char *text = NULL;
  		LONG size = 0;

  		ENTER();
  		if(ObtainFileInfo(file, 0, &size) == TRUE && size > 0 && (text = malloc((size+1)*sizeof(char))) != NULL)
  			{
    			FILE *fh;

    			text[size] = '\0'; // NUL-terminate the string
    			if((fh = fopen(file, "r")) != NULL)
    				{
      					if(fread(text, sizeof(char), size, fh) != (size_t)size)
      						{
        						free(text);
        						text = NULL;
      						}
      					fclose(fh);
    				}
    			else
    				{
      					free(text);
      					text = NULL;
    				}
  			}
 	if(outsize != NULL) *outsize = size;
 	RETURN(text);
  	return text;
}
