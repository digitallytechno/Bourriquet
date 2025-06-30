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

#include "ConfigPage_cl.h"

#include <proto/muimaster.h>
#include <proto/asl.h>

#include "Bourriquet.h"
#include "Config.h"
#include "mui/ImageArea.h"
#include "mui/ThemeListGroup.h"
#include "Debug.h"
#include "Stringsizes.h"
#include "MUIObjects.h"

/* INCLUDE
#include "Locale.h"
#include "MUIObjects.h"
#include "Themes.h"
*/

/* CLASSDATA
struct Data
	{
		Object *ST_REALNAME;
		Object *CY_UPDATEINTERVAL;
  		Object *BT_UPDATENOW;
  		Object *TX_UPDATESTATUS;
  		Object *TX_UPDATEDATE;
		Object *VIRT;
		Object *PORT;
  		Object *THEMES;
		Object *ST_TEMPDIR;
		Object *ST_DOWNLOADDIR;
		Object *ST_SHAREDDIR;
	};
*/

/* Overloaded Methods */
/// OVERLOAD(OM_NEW)
OVERLOAD(OM_NEW)
	{
		static const char *updateInterval[5];
		Object *ST_REALNAME;
		Object *CY_UPDATEINTERVAL;
  		Object *BT_UPDATENOW;
  		Object *TX_UPDATESTATUS;
  		Object *TX_UPDATEDATE;
		Object *VIRT;
		Object *PORT;
		Object *THEMES;
		Object *ST_TEMPDIR;
		Object *ST_DOWNLOADDIR;
		Object *ST_SHAREDDIR;
		
   		ENTER();
		updateInterval[0] = GetStr(MSG_CO_UPDATE_FREQUENCY_NEVER);
  		updateInterval[1] = GetStr(MSG_CO_UPDATE_FREQUENCY_DAILY);
  		updateInterval[2] = GetStr(MSG_CO_UPDATE_FREQUENCY_WEEKLY);
  		updateInterval[3] = GetStr(MSG_CO_UPDATE_FREQUENCY_MONTHLY);
  		updateInterval[4] = NULL;
				
  		if((obj = DoSuperNew(cl, obj,
												Child, HGroup,
														  	Child, MakeImageObject("prefs_big", G->theme.itemImages[SI_PREFS_BIG]),
															Child, VGroup,
																		Child, TextObject,
																					MUIA_Text_PreParse, "\033b",
																					MUIA_Text_Contents, GetStr(MSG_CO_TITLE),
																					MUIA_Text_Copy,     FALSE,
																		End,
																		Child, TextObject,
																					MUIA_Text_PreParse, "\0332",
																					MUIA_Text_Contents, GetStr(MSG_CO_SUMMARY),
																					MUIA_Text_Copy,     FALSE,
																		End,
															End,
												End,
												Child, RectangleObject,
														  	MUIA_Rectangle_HBar, TRUE,
															MUIA_FixHeight,      4,
												End,
												Child, ScrollgroupObject,
        													MUIA_Scrollgroup_FreeHoriz, FALSE,
        													MUIA_Scrollgroup_AutoBars, TRUE,
        													MUIA_Scrollgroup_Contents, VIRT = VGroupV,
    														Child, HGroup,		
																		Child, VGroup, 
																					TextFrame, 
																					Child, HGroup,
																								Child, MakeImageObject("config_firststep_big", G->theme.configImages[CI_FIRSTSTEPBIG]),
																								Child, TextObject,
																										  	MUIA_Text_PreParse, "\033b",
																										  	MUIA_Text_Contents, GetStr(MSG_CO_GENERAL),
																										  	MUIA_Text_Copy,     FALSE,
																								End,
																					End,
																					Child, VGroup,
																								GroupFrameT(GetStr(MSG_CO_GENERAL)),
																								Child, HGroup, 
																												Child, HGroup, 
																															GroupFrameT(GetStr(MSG_CO_GENERAL_USERNAME)),
																															Child, HSpace(50),
																															Child, MakeImageObject("config_identities_big", G->theme.configImages[CI_IDENTITIESBIG]),
																															Child, HSpace(25),
        																													Child, ST_REALNAME = MakeString(SIZE_REALNAME, GetStr(MSG_CO_GENERAL_USERNAME)),
																															Child, HSpace(50),
																												End,
																												Child, HGroup,
																															GroupFrameT(GetStr(MSG_CO_GENERAL_CLIENT_PORT)),
																															Child, HSpace(50),
																															Child, MakeImageObject("config_network_big", G->theme.configImages[CI_NETWORKBIG]),
																															Child, HSpace(25),
																															Child, PORT = MakeInteger(5, "Port"),
																															Child, HSpace(50),
																												End,
																								End,
																					End,
																		End,
															End,
															Child, HGroup,
																		Child, VGroup, 
																					TextFrame, 
																					Child, HGroup,
																								Child, MakeImageObject("config_transfert", G->theme.configImages[CI_TRANSFERT]),
																								Child, TextObject,
																										  	MUIA_Text_PreParse, "\033b",
																										  	MUIA_Text_Contents, GetStr(MSG_CO_DIRECTORIES_TITLE),
																										  	MUIA_Text_Copy,     FALSE,
																								End,
																					End,
																					Child, ColGroup(2), GroupFrameT(GetStr(MSG_CO_DIRECTORIES_PATHS)),
																											Child, Label2(GetStr(MSG_CO_DIRECTORIES_TEMPORARY_PATHS)),
																											Child, PopaslObject,
																														MUIA_Popasl_Type,      ASL_FileRequest,
																														MUIA_Popstring_String, ST_TEMPDIR = MakeString(SIZE_PATH, GetStr(MSG_CO_DIRECTORIES_TEMPORARY_PATHS)),
																														MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
																														ASLFR_DrawersOnly, TRUE,
																											End,
																											Child, Label2(GetStr(MSG_CO_DIRECTORIES_INCOMING_PATHS)),
																											Child, PopaslObject,
																													  MUIA_Popasl_Type,      ASL_FileRequest,
																													  MUIA_Popstring_String, ST_DOWNLOADDIR = MakeString(SIZE_PATH, GetStr(MSG_CO_DIRECTORIES_INCOMING_PATHS)),
																													  MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
																													  ASLFR_DrawersOnly, TRUE,
																											End,
																											Child, Label2(GetStr(MSG_CO_DIRECTORIES_SHARED_PATHS)),
																											Child, PopaslObject,
																													  MUIA_Popasl_Type,      ASL_FileRequest,
																													  MUIA_Popstring_String, ST_SHAREDDIR = MakeString(SIZE_PATH, GetStr(MSG_CO_DIRECTORIES_SHARED_PATHS)),
																													  MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
																													  ASLFR_DrawersOnly, TRUE,
																											End,
																							  End,
																		End,
															End,
															Child, HGroup,
																		Child, VGroup, 
																					TextFrame, 
																					MUIA_VertWeight, 100,
																					Child, HGroup,
																								Child, MakeImageObject("config_lookfeel_big", G->theme.configImages[CI_LOOKFEELBIG]),
																								Child, TextObject,
																										  	MUIA_Text_PreParse, "\033b",
																										  	MUIA_Text_Contents, GetStr(MSG_CO_THEMES_TITLE),
																										  	MUIA_Text_Copy,     FALSE,
																								End,
																					End,
																					Child, VGroup,
																								GroupFrameT(GetStr(MSG_CO_THEMES_APPEARANCE)),
																								Child, HGroup, 
																												Child, HGroup, 
																															Child, ScrollgroupObject,
																																		MUIA_Scrollgroup_FreeHoriz, FALSE,
																																		MUIA_Scrollgroup_AutoBars, TRUE,
																																		MUIA_Scrollgroup_Contents, VGroupV,
																																														Child, THEMES = ThemeListGroupObject, End,
																																													  End,
																															End,
																												End,
																								End,
																					End,
																		End,
															End,
															Child, HGroup,
																		Child, VGroup, 
																					TextFrame, 
																					Child, HGroup,
																								Child, MakeImageObject("config_updates_big", G->theme.configImages[CI_UPDATEBIG]),
																								Child, TextObject,
																											MUIA_Text_PreParse, "\033b",
																											MUIA_Text_Contents, GetStr(MSG_CO_UPDATE_TITLE),
																											MUIA_Text_Copy,     FALSE,
																								End,
																					End,
																					Child, VGroup, 
																								GroupFrameT(GetStr(MSG_CO_UPDATE_ALL)),
        																						Child, HGroup,
																											Child, HSpace(50),
																								  			Child, LLabel1(GetStr(MSG_CO_UPDATE_FREQUENCY)),
																											Child, HSpace(25),
																								  			Child, CY_UPDATEINTERVAL = MakeCycle(updateInterval, GetStr(MSG_CO_UPDATE_FREQUENCY)),
																											Child, HSpace(100),
																								End,
																								Child, HGroup,
																											Child, HSpace(100),
																											Child, BT_UPDATENOW = MakeButton(GetStr(MSG_CO_UPDATE_SEARCH)),
																											Child, HSpace(50),
																								End,
																								Child, VSpace(1),
																								Child, ColGroup(2),
																											TextFrame,
																								  			Child, LLabel1("Dernière recherche"),
																								  			Child, TX_UPDATESTATUS = TextObject,
																																							MUIA_Text_Contents, "last status",
																																							MUIA_Text_Copy, FALSE,
																								  			End,
																								  			Child, HSpace(1),
																								  			Child, TX_UPDATEDATE = TextObject,
																								  			End,
																								End,
																					End,
																		End,
															End,
															Child, VSpace(2),
															Child, RectangleObject,
																		MUIA_Rectangle_HBar, TRUE,
																		MUIA_FixHeight,      4,
															End,
															Child, HGroup,
																		MUIA_Group_SameWidth, TRUE,
																		Child, MakeButton(GetStr(MSG_CO_SAVE)),
																		Child, MakeButton(GetStr(MSG_CO_USE)),
																		Child, MakeButton(GetStr(MSG_CO_ABORT)),
															End,
												End, 
												End,
  												TAG_MORE, inittags(msg))) != NULL)
  			{
				GETDATA;
				
				data->ST_REALNAME = ST_REALNAME;
				data->PORT = PORT;
    			data->THEMES =  THEMES;
				data->ST_TEMPDIR = ST_TEMPDIR;
				data->ST_DOWNLOADDIR = ST_DOWNLOADDIR;
				data->ST_SHAREDDIR = ST_SHAREDDIR;
				setstring(data->ST_REALNAME, C->UserName);
				setstring(data->ST_TEMPDIR, C->TempDir);
				setstring(data->ST_DOWNLOADDIR, C->DownloadDir);
				setstring(data->ST_SHAREDDIR, C->ShareDir);
				set(data->PORT, MUIA_String_Integer, C->ClientPort);
				DoMethod(data->THEMES, MUIM_ThemeListGroup_Update);

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
  		switch(((struct opGet *)msg)->opg_AttrID)
  			{
    			case ATTR(ConfigUpdate): 
					*store = TRUE; 
					result = TRUE; 
					break;
  			}
  		if(result == FALSE) result = DoSuperMethodA(cl, obj, msg);
  		RETURN(result);
  		return result;
	}

///
/// DECLARE(ConfigToGUI)
DECLARE(ConfigToGUI)
	{
  		ENTER();
  		E(DBF_GUI, "derived class did not overload MUIM_ConfigPage_ConfigToGUI method");
  		RETURN(0);
  		return 0;
	}

///
/// DECLARE(GUIToConfig)
DECLARE(GUIToConfig)
	{
  		ENTER();
  		E(DBF_GUI, "derived class did not overload MUIM_ConfigPage_GUIToConfig method");
 	 	RETURN(0);
  		return 0;
	}

///
/// DECLARE(ConfigUpdate)
DECLARE(ConfigUpdate) // ULONG sourcePage
	{
  		ENTER();
  		// this method is optional to be overloaded
  		RETURN(0);
  		return 0;
	}

///
