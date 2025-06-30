/************************************************************
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
 
Superclass:  MUIC_NList
Description: Classe NList pour l'affichage des fichiers partagés 
************************************************************/
#include "SharedList_cl.h"

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
#include "Debug.h"
#include "Share.h"

/* INCLUDE
#include "Themes.h"
#include "Stringsizes.h"
#include "UploadList.h"
*/

/* CLASSDATA
struct Data
{
  	char Name[SIZE_LARGE];
  	char Size[SIZE_DEFAULT];
	char Type[SIZE_DEFAULT];
	char FileID[2048];
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
	struct Share *sh = (struct Share *)ndm->entry;
		
	ENTER();
	if(sh != NULL)
		{
    		GETDATA;
			snprintf(data->Name, sizeof(data->Name), "%s", sh->FileName);
			ndm->strings[0] = data->Name;
			FormatSize(sh->Size, data->Size, sizeof(data->Size), SF_MIXED);
			ndm->strings[1] = data->Size;
			snprintf(data->Type, sizeof(data->Type), "%s", sh->FileType);
			ndm->strings[2] = data->Type;
			snprintf(data->FileID, sizeof(data->FileID), "%s", hash_bin2hex(sh->FileID));
			ndm->strings[3] = data->FileID;
  		}
  	else
  		{
    		ndm->strings[0] = GetStr(MSG_SF_NAME);
    		ndm->strings[1] = GetStr(MSG_SF_SIZE);
    		ndm->strings[2] = GetStr(MSG_SF_TYPE);
			ndm->strings[3] = GetStr(MSG_SF_SHARED_EDK); 
			ndm->strings[4] = GetStr(MSG_SF_REQUESTS);
			ndm->strings[5] = GetStr(MSG_SF_TRANSFERRED);
    		ndm->strings[6] = GetStr(MSG_SF_SHARED_PARTS);
    		ndm->strings[7] = GetStr(MSG_SF_COMPLETE);
			//ndm->strings[8] = GetStr(MSG_SF_SHARED_EDK);*/
  		}
  	RETURN(0);
  	return(0);
}

///

/* Public Methods */
/// DECLARE(MakeFormat)
// création de la définition du format pour la listview des fichiers partagés
DECLARE(MakeFormat)
	{
  		static const int defwidth[9] = { -1,-1,-1,-1,-1,-1,-1,-1,-1 };
  		char format[SIZE_LARGE];
  		BOOL first = TRUE;
  		int i;

  		ENTER();
  		*format = '\0';
  		for(i = 0; i < 9; i++)
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

