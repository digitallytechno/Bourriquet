/***************************************************************************
 Bourriquet MorphOS
 Copyright (C) 2025 digitally
 
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
 
 $Id$

 Superclass:  MUIC_Group
 Description: Classe de base pour la barre de status en page principale
 
***************************************************************************/
#include "StatusBar_cl.h"

#include <proto/muimaster.h>

#include "mui/Classes.h"
#include "mui/ImageArea.h"

#include "Bourriquet.h"
#include "Debug.h"
#include "Stringsizes.h"
#include "MUIObjects.h"
#include "Locale.h"

/* CLASSDATA
struct Data
{
	Object *TXT;
	Object *USERS_FILES;
	Object *DL_UL;
};
*/

/* Overloaded Methods */
/// OVERLOAD(OM_NEW)
OVERLOAD(OM_NEW)
{
	Object *TXT;
	Object *USERS_FILES;
	Object *DL_UL;
		
   	ENTER();
  	if((obj = DoSuperNew(cl, obj, Child, HGroup,
																		TextFrame,
																		Child, MakeImageObject("connectednotnot",  G->theme.itemImages[SI_CONNECTEDNOTNOT]),
																		Child, TXT = TextObject,
																								MUIA_HorizWeight, 350,
																								MUIA_Text_Contents, GetStr(MSG_SE_TXT),       
																		End,
																		Child, MakeImageObject("user",  G->theme.itemImages[SI_USER]), 
																		Child, USERS_FILES = TextObject,
																					  								MUIA_HorizWeight, 150,
																					  								MUIA_Text_Contents, GetStr(MSG_SE_USERS_FILES),
																		End,
																		Child, MakeImageObject("updown",  G->theme.itemImages[SI_UPDOWN]),
																		Child, DL_UL = TextObject,
																					 					MUIA_HorizWeight, 100,
																					 					MUIA_Text_Contents, GetStr(MSG_SE_DL_UL),
																		End,
															End,
  											TAG_MORE, inittags(msg))) != NULL)
  		{
			GETDATA;
			data->TXT = TXT;
			data->USERS_FILES = USERS_FILES;	
			data->DL_UL = DL_UL;
			DoMethod(obj, METHOD(Update)); 
		}
	RETURN((IPTR)obj);
	return (IPTR)obj;
}

///
/// OVERLOAD(OM_GET)
OVERLOAD(OM_GET)
{
 	//IPTR *store = ((struct opGet *)msg)->opg_Storage;
 	ULONG result = FALSE;

 	ENTER();
 	RETURN(result);
 	return(result);
}

/// DECLARE(Update)
DECLARE(Update)
{
  	BOOL result = FALSE;
	char txt[SIZE_LINE], txt1[SIZE_LINE];
	int users = 0;
	int files = 0;	
	
	GETDATA;
	ENTER();
	snprintf(txt, sizeof(txt), (STRPTR)GetStr(MSG_SE_USERS_FILES), users, files);
	set(data->USERS_FILES, MUIA_Text_Contents, txt);
	snprintf(txt1, sizeof(txt1), (STRPTR)GetStr(MSG_SE_DL_UL), users, users, files, files);
	set(data->DL_UL, MUIA_Text_Contents, txt1);
	RETURN(result);
  	return(result);
}