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

#include "ResearchPage_cl.h"

#include <proto/muimaster.h>
#include <proto/asl.h>
#include <mui/NBalance_mcc.h>
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
		Object *LV_RESEARCH;
	};
*/

/* Overloaded Methods */
/// OVERLOAD(OM_NEW)
OVERLOAD(OM_NEW)
	{
		static char *typeformat[9];
		Object *LV_RESEARCH;
		
		typeformat[0] = "\033c\033I[5:PROGDIR:resources/themes/default/images/FileTypeArchive]     Archive ";
  		typeformat[1] = "\033c\033I[5:PROGDIR:resources/themes/default/images/FileTypeAudio]          Audio  ";
  		typeformat[2] = "\033c\033I[5:PROGDIR:resources/themes/default/images/FileTypeDocument]     Document";
		typeformat[3] = "\033c\033I[5:PROGDIR:resources/themes/default/images/FileTypePicture]        Image  ";
  		typeformat[4] = "\033c\033I[5:PROGDIR:resources/themes/default/images/FileTypeCDImage]     CD image";
  		typeformat[5] = "\033c\033I[5:PROGDIR:resources/themes/default/images/FileTypeProgram]     Programme";	
		typeformat[6] = "\033c\033I[5:PROGDIR:resources/themes/default/images/FileTypeAny]            Tous   ";
  		typeformat[7] = "\033c\033I[5:PROGDIR:resources/themes/default/images/FileTypeVideo]          Vidéo  ";	
		typeformat[8] = NULL;
		
   		ENTER();
				
  		if((obj = DoSuperNew(cl, obj,
												Child, HGroup,
															Child, MakeImageObject("research_big", G->theme.itemImages[SI_RESEARCH_BIG]),
															Child, VGroup,
																		Child, TextObject,
																					  MUIA_Text_PreParse, "\033b",
																					  MUIA_Text_Contents, GetStr(MSG_RS_TITLE),
																					  MUIA_Text_Copy,     FALSE,
																		End,
																		Child, TextObject,
																					  MUIA_Text_Contents, GetStr(MSG_RS_SUMMARY),
																					  MUIA_Text_Copy,     FALSE,
																		End,
															End,
												End,
												Child, VGroup, 
															Child, HGroup,
																		Child, HGroup,
																					TextFrame, 
																					Child, VGroup,
																								Child, HGroup,
																											Child, MakeImageObject("searchparams",  G->theme.itemImages[SI_SEARCHPARAMS]), 
																											Child, CLabel(GetStr(MSG_RS_SEARCHPARAMS)),
																											Child, RectangleObject,
																														MUIA_FixHeight, 2,
																													  	MUIA_Rectangle_HBar, TRUE,
																											End,
																								End,
																								MUIA_HorizWeight, 120,
																								Child, HGroup,
																											Child, CLabel(GetStr(MSG_RS_FILENAME)),
																											Child, MakeString(SIZE_HOST, GetStr(MSG_RS_FILENAME)),
																								End,
																								Child, HGroup,
																											Child, CLabel(GetStr(MSG_RS_FILETYPE)),
																											Child, MakeCycle(typeformat,GetStr(MSG_RS_FILETYPE)),
																								End,
																								Child, HGroup,
																											Child, MakeButton(GetStr(MSG_RS_START)),
																											Child, MakeButton(GetStr(MSG_RS_ABORT)),
																											Child, MakeButton(GetStr(MSG_RS_REMOVEALL)),
																								End,
																					End,
																		End,
																		Child, NBalanceObject,
																					MUIA_Balance_Quiet, FALSE,
																		End,
																		Child, VGroup,
																					TextFrame, 
																					MUIA_HorizWeight, 300,
																					MUIA_Disabled, TRUE,
																		End,
															End,
															Child, HGroup, 
																		Child, VGroup,
																					TextFrame,
																					Child, HGroup,
																								Child, MakeImageObject("research",  G->theme.itemImages[SI_RESEARCH]), 
																								Child, CLabel(GetStr(MSG_RS_TITLE)),
																								Child, RectangleObject, End,
																					End,
																					MUIA_HorizWeight, 350,
																					Child,  NListviewObject,
																								MUIA_CycleChain,        TRUE,
																								MUIA_Listview_DragType, MUIV_Listview_DragType_Immediate,
																								MUIA_NListview_NList,   LV_RESEARCH = ResearchListObject, 
																								End,
																					End, 
																		End,
															End,
												End,
  												TAG_MORE, inittags(msg))) != NULL)
  			{
				GETDATA;
				
				data->LV_RESEARCH = LV_RESEARCH;
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
