/********************************************************
Bourriquet MorphOS
Copyright (C) 2022 digitally

This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
Superclass:  MUIC_NList
Description: Classe NList pour l'affichage de la recherche 
********************************************************/
#include "ResearchList_cl.h"

#include <string.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <mui/NList_mcc.h>
#include <mui/NListtree_mcc.h>

#include "Bourriquet.h"

#include "mui/ImageArea.h"
#include "Locale.h"
#include "MUIObjects.h"
//#include "Server.h"
#include "Debug.h"

/* INCLUDE
#include "Themes.h"
#include "Stringsizes.h"
#include "ResearchList.h"
*/

/* CLASSDATA
struct Data
{
	Object *FTArchiveImage;
	Object *FTAudioImage;
	Object *FTDocumentImage;
	Object *FTPictureImage;
	Object *FTCDIsoImage;
	Object *FTProgrammeImage;
	Object *FTAnyImage;
	Object *FTVideoImage;
  	char name[SIZE_ADDRESS];
  	char size[SIZE_LARGE];
  	char disponibility[SIZE_LARGE];
  	char complets[SIZE_LARGE];
	char type[SIZE_DEFAULT];
  	char artist[SIZE_DEFAULT];
  	char album[SIZE_DEFAULT];
  	char title[SIZE_DEFAULT];
	char time[SIZE_DEFAULT];
	char debit[SIZE_DEFAULT];
  	char codec[SIZE_DEFAULT];
  	char connu[SIZE_DEFAULT];
};
*/

/* Overloaded Methods */
/// OVERLOAD(OM_NEW)
OVERLOAD(OM_NEW)
	{
  		ENTER();
  		if((obj = DoSuperNew(cl, obj, 		
												MUIA_ObjectID,          MAKE_ID('N','L','0','1'),
												MUIA_Font,                 MUIV_NList_Font,
												MUIA_ShortHelp,                  TRUE,
												MUIA_NList_MinColSortable,       0,
												//MUIA_NList_TitleClick,           TRUE,
												//MUIA_NList_TitleClick2,          TRUE,
												MUIA_NList_MultiSelect,          MUIV_NList_MultiSelect_Default,
												MUIA_NList_AutoVisible,          TRUE,
												MUIA_NList_Title,                TRUE,
												MUIA_NList_TitleSeparator,       TRUE,
												MUIA_NList_ActiveObjectOnClick,  TRUE,
												MUIA_NList_DefaultObjectOnClick, FALSE,
												MUIA_ContextMenu,                TRUE,
    											MUIA_Dropable,                   FALSE,
												TAG_MORE, inittags(msg))) != NULL)
  			{		
    			GETDATA;  
				//data->ServerImage = MakeImageObject("status",  G->theme.itemImages[SI_STATUS]);	
				//DoMethod(obj, MUIM_NList_UseImage, data->ServerImage, SI_STATUS, MUIF_NONE);		
				DoMethod(obj, METHOD(MakeFormat));				
  			}
  		RETURN((IPTR)obj);
 		return (IPTR)obj;
	}

///
/// OVERLOAD(OM_DISPOSE)
OVERLOAD(OM_DISPOSE)
	{
  		IPTR result;
  		GETDATA;

  		/*DoMethod(obj, MUIM_NList_UseImage, NULL, MUIF_NONE);
    	if(data->ServerImage != NULL)
    		{
      			MUI_DisposeObject(data->ServerImage);
      			data->ServerImage = NULL;
    		}*/
		result = DoSuperMethodA(cl, obj, msg);
		RETURN(result);
  		return result;
	}

///
/// OVERLOAD(OM_GET)
// prend quelques affaires de notre instance de données
OVERLOAD(OM_GET)
	{
  		GETDATA;
  		IPTR *store = ((struct opGet *)msg)->opg_Storage;

  		switch(((struct opGet *)msg)->opg_AttrID)
  			{
    			//case ATTR(ImageArray): *store = (ULONG)data->ServerImage; return TRUE;
  			}
  		return DoSuperMethodA(cl, obj, msg);
	}

///
/// OVERLOAD(MUIM_NList_Display)
OVERLOAD(MUIM_NList_Display)
	{
  		struct MUIP_NList_Display *ndm = (struct MUIP_NList_Display *)msg;
		//struct Server *sv = (struct Server *)ndm->entry;
		
  		ENTER();
  		//if(sv != NULL)
  			{
    			GETDATA;
				
				/*snprintf(data->name, sizeof(data->name), "\033o[%d] %s", SI_STATUS, sv->Name);
				ndm->strings[0] = data->name;
				snprintf(data->ip, sizeof(data->ip), "%s:%d", sv->IP, sv->Port);
				ndm->strings[1] = data->ip;
    			ndm->strings[2] = sv->Description;
				snprintf(data->ping, sizeof(data->ping), " %ld",  sv->Ping);
				ndm->strings[3] = data->ping;
				snprintf(data->users, sizeof(data->users), "%ld", sv->Users);
				ndm->strings[4] = data->users;
				FormatSize(sv->maxusers, data->maxusers, sizeof(data->maxusers), SF_MIXED);
				ndm->strings[5] = data->maxusers;
				FormatSize(sv->Files, data->files, sizeof(data->files), SF_MIXED);
				ndm->strings[6] = data->files;
				if(sv->Prefs == 1) snprintf(data->prefs, sizeof(data->prefs), "%s", GetStr(MSG_SE_HIGHPRI));
				else if(sv->Prefs == 2) snprintf(data->prefs, sizeof(data->prefs), "%s", GetStr(MSG_SE_LOWPRI));
				else snprintf(data->prefs, sizeof(data->prefs), "%s", GetStr(MSG_SE_NORMALPRI));
    			ndm->strings[7] = data->prefs;
				snprintf(data->failed, sizeof(data->failed), "%d", sv->Failed);
				ndm->strings[8] = data->failed;
				snprintf(data->statik, sizeof(data->statik), "%d", sv->Static);
				ndm->strings[9] = data->statik;
				FormatSize(sv->softfiles, data->limitebassefiles, sizeof(data->limitebassefiles), SF_MIXED);
				ndm->strings[10] = data->limitebassefiles;
				FormatSize(sv->hardfiles, data->limitehautefiles, sizeof(data->limitehautefiles), SF_MIXED);
				ndm->strings[11] = data->limitehautefiles;
    			ndm->strings[12] = sv->version;
				snprintf(data->lowidusers, sizeof(data->lowidusers), "%ld", sv->lowidusers);
				ndm->strings[13] = data->lowidusers;*/
  		/*	}
  		else
  			{*/
    			ndm->strings[0] = GetStr(MSG_RS_NAME);
    			ndm->strings[1] = GetStr(MSG_RS_SIZE);
    			ndm->strings[2] = GetStr(MSG_RS_AVAILABILITY);
				ndm->strings[3] = GetStr(MSG_RS_FULL); 
				ndm->strings[4] = GetStr(MSG_RS_TYPE);
				ndm->strings[5] = GetStr(MSG_RS_ARTIST);
    			ndm->strings[6] = GetStr(MSG_RS_ALBUM);
    			ndm->strings[7] = GetStr(MSG_RS_ALBUM_TITLE);
				ndm->strings[8] = GetStr(MSG_RS_DURATION);
				ndm->strings[9] = GetStr(MSG_RS_RATE);
				ndm->strings[10] = GetStr(MSG_RS_CODEC);
    			ndm->strings[11] = GetStr(MSG_RS_KNOWN);
  			}
  		RETURN(0);
  		return 0;
	}

///

/* Public Methods */
/// DECLARE(MakeFormat)
// création de la définition du format pour la listview des recherches
DECLARE(MakeFormat)
	{
  		static const int defwidth[12] = { -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1};
  		char format[SIZE_LARGE];
  		BOOL first = TRUE;
  		int i;

  		ENTER();
  		*format = '\0';
  		for(i = 0; i < 12; i++)
  			{
      			int p;

      			if(first) first = FALSE;
      			else strlcat(format, " TBAR,", sizeof(format));
      			p = strlen(format);
      			snprintf(&format[p], sizeof(format)-p, "COL=%d W=%d", i, defwidth[i]);
      		 	strlcat(format, " P=\033l", sizeof(format));
    		}
  		set(obj, MUIA_NList_Format, format);
  		RETURN(0);
  		return 0;
	}

