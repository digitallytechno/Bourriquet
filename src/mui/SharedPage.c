/***************************************************************************
 Bourriquet MorphOS
 Copyright (C) 2023 digitally
 
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

 Superclass:  MUIC_Virtgroup
 Description: Classe de base pour les pages de configuration
 
***************************************************************************/

#include "SharedPage_cl.h"

#include <proto/muimaster.h>
#include <proto/asl.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>

#include "mui/Classes.h"
#include "mui/ImageArea.h"

#include "Bourriquet.h"
#include "Config.h"
#include "Debug.h"
#include "Stringsizes.h"
#include "MUIObjects.h"
#include "Locale.h"

/* CLASSDATA
struct Data
	{
		Object *LV_SHARED;
	};
*/

/* Overloaded Methods */
/// OVERLOAD(OM_NEW)
OVERLOAD(OM_NEW)
	{
		Object *LV_SHARED;
		
   		ENTER();
				
  		if((obj = DoSuperNew(cl, obj,
												Child, HGroup,
															Child, MakeImageObject("shared_big", G->theme.itemImages[SI_SHARED_BIG]),
															Child, VGroup,
																		Child, TextObject,
																					  MUIA_Text_PreParse, "\033b",
																					  MUIA_Text_Contents, GetStr(MSG_SF_TITLE),
																					  MUIA_Text_Copy,     FALSE,
																		End,
																		Child, TextObject,
																					  MUIA_Text_Contents, GetStr(MSG_SF_SUMMARY),
																					  MUIA_Text_Copy,     FALSE,
																		End,
															End,
												End,
												Child, RectangleObject,
															  MUIA_Rectangle_HBar, TRUE,
															  MUIA_FixHeight,      4,
												End,
												Child, VGroup, 
															TextFrame, 
															Child, HGroup, 
																		Child, VGroup,
																					Child, HGroup,
																								Child, MakeImageObject("shared",  G->theme.itemImages[SI_SHARED]), 
																								Child, CLabel(GetStr(MSG_SF_TITLE)),
																								Child, RectangleObject, End,
																					End,
																					MUIA_HorizWeight, 350,
																					Child,  NListviewObject,
																								MUIA_CycleChain,        TRUE,
																								MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
																								MUIA_NListview_NList,   LV_SHARED = SharedListObject, End,
																					End,
																		End,
															End,
												End,
  												TAG_MORE, inittags(msg))) != NULL)
  			{
				GETDATA;
				
				data->LV_SHARED = LV_SHARED;
  			}
  		RETURN((IPTR)obj);
  		return (IPTR)obj;
	}

///
/// OVERLOAD(OM_GET)
OVERLOAD(OM_GET)
	{
  		IPTR *store = ((struct opGet *)msg)->opg_Storage;
  		ULONG result = FALSE;

  		ENTER();
  		RETURN(result);
  		return result;
	}
