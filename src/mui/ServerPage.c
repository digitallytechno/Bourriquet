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
#include "ServerPage_cl.h"

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
#include "MethodStack.h"
#include "Locale.h"
#include "Server.h"
#include "mui/ServerList.h"
#include "ServerList.h"
#include "ServerMet.h"
#include "Threads.h"
#include "Error.h"
#include "DashBoard.h"

/* CLASSDATA
struct Data
{
	Object *NBSERVERS;
	Object *LV_SERVER;
	Object *IP;
	Object *PORT;
	Object *INFOS;
	Object *LOG;
	Object *NAME;
	Object *ADDSERVER;
	Object *UPDATE;
	Object *UPDATE_URL;
};
*/

/* Overloaded Methods */
/// OVERLOAD(OM_NEW)
OVERLOAD(OM_NEW)
{
	Object *NBSERVERS;
	Object *LV_SERVER;
	Object *IP;
	Object *PORT;
	Object *INFOS;
	Object *LOG;
	Object *NAME;
	Object *ADDSERVER;
	Object *UPDATE;
	Object *UPDATE_URL;
		
	static char *spages[3];
	static char disp[SIZE_COMMAND], disp1[SIZE_COMMAND];
			
	sprintf(spages[0] = disp, "\033l\033I[5:PROGDIR:resources/themes/default/images/ServerInfo] %s", GetStr(MSG_SE_INFOS));
	sprintf(spages[1] = disp1, "\033l\033I[5:PROGDIR:resources/themes/default/images/Log] %s", GetStr(MSG_SE_LOG));
	spages[2] =  NULL;	
		
	ENTER();
	if((obj = DoSuperNew(cl, obj,
												Child, HGroup,
															Child, MakeImageObject("server_big", G->theme.itemImages[SI_SERVER_BIG]),
															Child, VGroup,
																		Child, TextObject,
																					MUIA_Text_PreParse, "\033b",
																					MUIA_Text_Contents, GetStr(MSG_SE_TITLE),
																					MUIA_Text_Copy,     FALSE,
																		End,
																		Child, TextObject,
																					MUIA_Text_Contents, GetStr(MSG_SE_SUMMARY),
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
																		Child, MakeImageObject("serverlist",  G->theme.itemImages[SI_SERVERLIST]), 
																		Child, NBSERVERS = TextObject,
																											MUIA_Text_Contents, GetStr(MSG_SE_NB_SERVERLIST),
																		End,
																		Child, RectangleObject, End,
															End,
															Child, HGroup, 
																		Child, VGroup,
																					MUIA_HorizWeight, 350,
																					Child,  NListviewObject,
																								MUIA_VertWeight, 250,
																								MUIA_CycleChain, TRUE,
																								MUIA_NListview_NList,   LV_SERVER = ServerListObject,  End,
																					End,
																					Child, VSpace(3),
																					Child, NBalanceObject,
																									MUIA_Balance_Quiet, FALSE,
																					End,
																					Child, VSpace(3),
																					Child, HGroup,
																								Child, RegisterGroup(spages),
																											TextFrame,
																											Child, NListviewObject,
																														MUIA_VertWeight, 250,
																														//MUIA_CycleChain, TRUE,
																														MUIA_NListview_NList,   INFOS = InfosServeurObject, End,
																											End,
																											Child, NListviewObject,
																														MUIA_VertWeight, 250,
																														//MUIA_CycleChain, TRUE,
																														MUIA_NListview_NList,   LOG = LogObject, End,
																											End,
																								End, 
																					End,
																		End,
																		Child, NBalanceObject,
																					MUIA_Balance_Quiet, FALSE,
																		End,
																		Child, VGroup,
																					Child, VGroup,
																								TextFrame,
																								Child, HGroup,
																											Child, MakeImageObject("newserver",  G->theme.itemImages[SI_NEWSERVER]), 
																											Child, CLabel(GetStr(MSG_SE_NEWSERVER)),
																											Child, RectangleObject,
																														 MUIA_FixHeight, 2,
																														 MUIA_Rectangle_HBar, TRUE,
																											End,
																								End,
																								Child, HGroup,
																											Child, Label2(GetStr(MSG_SE_NEWSERVER_IP)),
																											Child, IP= MakeString(SIZE_HOST, GetStr(MSG_SE_NEWSERVER_IP)),
																											Child, Label2(GetStr(MSG_SE_NEWSERVER_PORT)),
																											Child, PORT = MakeInteger(5, GetStr(MSG_SE_NEWSERVER_PORT)),
																								End,
																								Child, HGroup,
																											Child, Label2(GetStr(MSG_SE_NEWSERVER_NAME)),
																											Child, NAME = MakeString(SIZE_NAME, GetStr(MSG_SE_NEWSERVER_NAME)),
																								End,
																								Child, HGroup,
																											//MUIA_Disabled, TRUE,
																											Child, HSpace(0),
																											Child, ADDSERVER = MakeButton(GetStr(MSG_SE_NEWSERVER_ADD)),
																								End,
																					End,
																					Child, VSpace(5),
																					Child, VGroup,
																								TextFrame,
																								Child, HGroup,
																											Child, MakeImageObject("serverupdate",  G->theme.itemImages[SI_SERVERSUPDATE]), 
																											Child, CLabel(GetStr(MSG_SE_UPDATE_MET)),
																											 Child, RectangleObject,
																														MUIA_FixHeight, 2,
																														MUIA_Rectangle_HBar, TRUE,
																											End,
																								End,
																								Child, HGroup,
																											Child, UPDATE_URL = MakeString(SIZE_DEFAULT, GetStr(MSG_SE_UPDATE_MET)),
																								End,
																								Child, HGroup,
																											MUIA_Disabled, FALSE,
																											Child, HSpace(0),
																											Child, UPDATE = MakeButton(GetStr(MSG_SE_UPDATE)),
																								End,
																					End,
																					Child, VGroup,
																								StringFrame,
																								Child, HGroup,
																											Child, MakeImageObject("info",  G->theme.itemImages[SI_INFO]), 
																											Child, CLabel(GetStr(MSG_SE_MYINFOS)),
																											Child, RectangleObject, End,
																								End,
																								Child, VSpace(0),
																					End,
																		End, 
															End,
													End, 
  											TAG_MORE, inittags(msg))) != NULL)
  		{
			GETDATA;
			data->NBSERVERS = NBSERVERS;
			data->LV_SERVER = LV_SERVER;	
			data->IP = IP;
			data->PORT = PORT;
			data->NAME = NAME;
			data->ADDSERVER = ADDSERVER;
			data->UPDATE_URL = UPDATE_URL;
			data->UPDATE = UPDATE;
			data->LOG = LOG;
			data->INFOS = INFOS;
			SetHelp(data->IP, GetStr(MSG_SE_HELP_IP));
			SetHelp(data->PORT, GetStr(MSG_SE_HELP_PORT));
			SetHelp(data->NAME, GetStr(MSG_SE_HELP_NAME));
			SetHelp(data->ADDSERVER, GetStr(MSG_SE_HELP_ADDSERVER));
			SetHelp(data->UPDATE, GetStr(MSG_SE_HELP_UPDATE));
			SetHelp(data->UPDATE_URL, GetStr(MSG_SE_HELP_UPDATE_MET));
			DoMethod(data->ADDSERVER, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, METHOD(AddServer));	
			set(data->INFOS, MUIA_NList_Quiet, MUIV_NList_Quiet_Full);	
			set(data->LOG, MUIA_NList_Quiet, MUIV_NList_Quiet_Full);			
  		}
  	RETURN((IPTR)obj);
  	return((IPTR)obj);
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

/* Public Methods */
/// DECLARE(Update)
DECLARE(Update) // ULONG nbservers
{
  	BOOL result = FALSE;
	char txt[SIZE_LINE];
	
	ENTER();
	GETDATA;
	snprintf(txt, sizeof(txt), (STRPTR)GetStr(MSG_SE_NB_SERVERLIST), msg->nbservers);
	set(data->NBSERVERS, MUIA_Text_Contents, txt);
	RETURN(result);
  	return(result);
}
	
/// DECLARE(SingleConnection)
DECLARE(SingleConnection)
{
	LONG result = RETURN_FAIL;
	struct Server *server;
	LONG pos = MUIV_NList_GetPos_Start;
	
	ENTER();
	GETDATA;
	DoMethod(data->LV_SERVER, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &server);
	if(server != NULL)
		{
			D(DBF_NET, "tentative de connexion sur le serveur %s, %s, %d ", server->Name, server->IP, server->Port);
			DoMethod(data->LV_SERVER, MUIM_NList_GetPos, server, &pos);	
			set(data->LV_SERVER, MUIA_NList_Active, MUIV_NList_Active_Off);
			DoMethod(data->LV_SERVER, MUIM_NList_Redraw, MUIV_NList_Redraw_Active); 
			DoAction(NULL, TA_StartSingleConnection, TT_Connection_OneServer, server, TAG_DONE);	
			result = RETURN_OK;				
		}
  	RETURN(result);
  	return(result);
}

/// DECLARE(Entry)
DECLARE(Entry)
{
	BOOL result = FALSE;
	struct Server *server;
	//LONG pos = MUIV_NList_GetPos_Start;
		
	ENTER();
	GETDATA;
	DoMethod(data->LV_SERVER, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &server);
	if(server != NULL) 
		{
			//DoMethod(data->LV_SERVER, MUIM_NList_GetPos, server, &pos);	
			//set(data->LV_SERVER, MUIA_NList_Active, MUIV_NList_Active_Off);
			//DoMethod(data->LV_SERVER, MUIM_NList_Redraw, MUIV_NList_Redraw_Active); 
			result = TRUE;	
		}		
  	RETURN(result);
  	return(result); 
}
	
/// DECLARE(AddServer)
DECLARE(AddServer)
{
	BOOL result = FALSE;
	char str[SIZE_SMALL];
	
	GETDATA;
	ENTER();
	D(DBF_STARTUP, "addserver"); 
	GetMUIString(str, data->IP, sizeof(str));
	D(DBF_STARTUP, "str %s", str); 
   	//get(data->IP, MUIA_String_Contents, &str);
   	if(!*str)
    	{
       		ER_NewError("Vous devez entrer une IP pour identifier cette entrée");
       		return(0);
    	}
   	/*get(data->PORT, MUIA_String_Contents, &str);
   	if(!*str)
    	{
       		ER_NewError("Vous devez entrer un Port client pour identifier cette entrée");
       		return;
    	}
   	get(data->NAME, MUIA_String_Contents, &str);
   	if(!*str)
    	{
       		ER_NewError("Vous devez entrer un nom pour identifier cette entrée");
       		return;
    	}*/
	RETURN(result);
  	return(result);
}
