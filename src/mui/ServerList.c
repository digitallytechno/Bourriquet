/***************************************************************************
 Bourriquet MorphOS
 Copyright (C) 2025 digitally

 Superclass:  MUIC_NList
 Description: Classe NList pour l'affichage de l'arborescence des serveurs 
***************************************************************************/
#include "ServerList_cl.h"

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
#include "Server.h"
#include "Debug.h"

/* INCLUDE
#include "Themes.h"
#include "Stringsizes.h"
#include "ServerList.h"
*/

/* CLASSDATA
struct Data
{
	Object *ServerImage;
  	char name[SIZE_ADDRESS];
  	char description[SIZE_LARGE];
  	char ip[SIZE_LARGE];
  	char port[SIZE_LARGE];
	char ping[SIZE_DEFAULT];
  	char maxusers[SIZE_DEFAULT];
  	char version[SIZE_DEFAULT];
  	char prefs[SIZE_DEFAULT];
	char failed[SIZE_DEFAULT];
	char statik[SIZE_DEFAULT];
  	char limitebassefiles[SIZE_DEFAULT];
  	char limitehautefiles[SIZE_DEFAULT];
  	char lowidusers[SIZE_DEFAULT];
  	char users[SIZE_DEFAULT];
  	char files[SIZE_DEFAULT];
    Object *context_menu;
	char ctxt_menutitle[SIZE_ADDRESS];
	char ctxt_connect[SIZE_COMMAND];
	char ctxt_priority[SIZE_DEFAULT];
	char ctxt_prioritylow[SIZE_DEFAULT];
	char ctxt_priorityhigh[SIZE_DEFAULT];
	char ctxt_prioritynormal[SIZE_DEFAULT];
	char ctxt_listadd[SIZE_COMMAND];
	char ctxt_listremove[SIZE_COMMAND];
	char ctxt_remove[SIZE_DEFAULT];
};
*/

/* Fonctions privées */
/// ServerCompare
//  Compare 2 serveurs
static int ServerCompare(struct Server *entry1, struct Server *entry2, LONG column)
	{
  		switch (column)
  			{
				case 0:
					{
						return stricmp(entry1->Name, entry2->Name);
					}
				case 1: 
					{
						return stricmp(entry1->IP, entry2->IP);
					}
				case 2: 
					{
						return stricmp(entry1->Description, entry2->Description);
					}
    			case 3:
    				{
      					return entry1->Ping - entry2->Ping; 
    				}
				case 4:
					{
						return entry1->Users - entry2->Users;
					}
				case 5:
					{
						return entry1->maxusers - entry2->maxusers;
					}
				case 6:
					{
						return entry1->Files - entry2->Files;
					}
				case 10:
					{
						return entry1->softfiles - entry2->softfiles;
					}
				case 11:
					{
						return entry1->hardfiles - entry2->hardfiles;
					}
				case 13:
					{
						return entry1->lowidusers - entry2->lowidusers;
					}
    			break;
			}
  		return 0;
	}

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
												MUIA_NList_TitleClick,           TRUE,
												MUIA_NList_TitleClick2,          TRUE,
												MUIA_NList_MultiSelect,          FALSE, 
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
				data->ServerImage	= MakeImageObject("status",  G->theme.itemImages[SI_STATUS]);	
				DoMethod(obj, MUIM_NList_UseImage, data->ServerImage, SI_STATUS, MUIF_NONE);						
				DoMethod(obj, METHOD(MakeFormat));	
				
				DoMethod(obj, MUIM_Notify, MUIA_NList_TitleClick,   MUIV_EveryTime, MUIV_Notify_Self, 4, MUIM_NList_Sort3, MUIV_TriggerValue,     MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_Both);
    			DoMethod(obj, MUIM_Notify, MUIA_NList_TitleClick2,  MUIV_EveryTime, MUIV_Notify_Self, 4, MUIM_NList_Sort3, MUIV_TriggerValue,     MUIV_NList_SortTypeAdd_2Values, MUIV_NList_Sort3_SortType_2);
    			DoMethod(obj, MUIM_Notify, MUIA_NList_SortType,     MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_Set,         MUIA_NList_TitleMark,  MUIV_TriggerValue);
    			DoMethod(obj, MUIM_Notify, MUIA_NList_SortType2,    MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_Set,         MUIA_NList_TitleMark2, MUIV_TriggerValue);				
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
  		if(data->context_menu != NULL) MUI_DisposeObject(data->context_menu);
  		DoMethod(obj, MUIM_NList_UseImage, NULL, MUIF_NONE);
 		if(data->ServerImage != NULL)
    		{
      			MUI_DisposeObject(data->ServerImage);
      			data->ServerImage = NULL;
			}
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
    			case ATTR(ImageArray): *store = (ULONG)data->ServerImage; return TRUE;
				case ATTR(SortOrderReversed):
    				{
      					LONG sortOrder[2];

      					sortOrder[0] = xget(obj, MUIA_NList_SortType);
      					sortOrder[1] = xget(obj, MUIA_NList_SortType2);
      					*store = (sortOrder[0] < 0 || sortOrder[1] < 0) ? TRUE : FALSE;
      					return TRUE;
    				}
  			}
  		return DoSuperMethodA(cl, obj, msg);
	}

///	
///
/// OVERLOAD(MUIM_ContextMenuBuild)
OVERLOAD(MUIM_ContextMenuBuild)
	{
  		GETDATA;
  		struct Server *sv;
		
  		// enleve l'ancien menu contextuel s'il existe toujours
  		if(data->context_menu != NULL)
  			{
    			MUI_DisposeObject(data->context_menu);
    			data->context_menu = NULL;
  			}
  		 if((sv = (struct Server *)DoMethod(obj, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, NULL)) != NULL)
  			{
				snprintf(data->ctxt_menutitle, sizeof(data->ctxt_menutitle), "\033I[5:PROGDIR:resources/themes/default/images/ServerList] %s",  sv->Name);
				snprintf(data->ctxt_connect, sizeof(data->ctxt_connect), "\033I[5:PROGDIR:resources/themes/default/images/ctxt_Connect] %s", (STRPTR)GetStr(MSG_SE_CTXT_CONNECT));
				snprintf(data->ctxt_priority, sizeof(data->ctxt_priority), "\033I[5:PROGDIR:resources/themes/default/images/ctxt_Priority] %s", (STRPTR)GetStr(MSG_SE_CTXT_PRIORITY));
				snprintf(data->ctxt_priorityhigh, sizeof(data->ctxt_priorityhigh), "\033I[5:PROGDIR:resources/themes/default/images/ctxt_PriorityHigh] %s", (STRPTR)GetStr(MSG_SE_HIGHPRI));
				snprintf(data->ctxt_prioritynormal, sizeof(data->ctxt_prioritynormal), "\033I[5:PROGDIR:resources/themes/default/images/ctxt_PriorityNormal] %s", (STRPTR)GetStr(MSG_SE_NORMALPRI));
				snprintf(data->ctxt_prioritylow, sizeof(data->ctxt_prioritylow), "\033I[5:PROGDIR:resources/themes/default/images/ctxt_PriorityLow] %s", (STRPTR)GetStr(MSG_SE_LOWPRI));
				snprintf(data->ctxt_listadd, sizeof(data->ctxt_listadd), "\033I[5:PROGDIR:resources/themes/default/images/ctxt_ListAdd] %s", (STRPTR)GetStr(MSG_SE_CTXT_LISTADD));
				snprintf(data->ctxt_listremove, sizeof(data->ctxt_listremove), "\033I[5:PROGDIR:resources/themes/default/images/ctxt_ListRemove] %s", (STRPTR)GetStr(MSG_SE_CTXT_LISTREMOVE));
				snprintf(data->ctxt_remove, sizeof(data->ctxt_remove), "\033I[5:PROGDIR:resources/themes/default/images/ctxt_Cancel] %s", (STRPTR)GetStr(MSG_SE_CTXT_ERASE));
				
        		data->context_menu = MenustripObject,
          											MenuChild, MenuObjectT(data->ctxt_menutitle),
            										MenuChild, Menuitem(data->ctxt_connect, NULL, TRUE, FALSE, 0),
            										MenuChild, MenuitemObject,
            															MUIA_Menuitem_Title, data->ctxt_priority,
            															MUIA_Menuitem_CopyStrings, FALSE,
            															MenuChild, MenuitemObject,
                                        													//MUIA_Menuitem_Checkit,     TRUE,
                                        													MUIA_Menuitem_Title,    data->ctxt_priorityhigh,
                                        													MUIA_Menuitem_CopyStrings,  FALSE,
																							MUIA_Menuitem_Enabled,    TRUE,
                                      														End,
																		MenuChild,  MenuitemObject,
                                        													//MUIA_Menuitem_Checkit,     TRUE,
                                        													MUIA_Menuitem_Title,    data->ctxt_prioritynormal,
                                        													MUIA_Menuitem_CopyStrings,  FALSE,
																							MUIA_Menuitem_Enabled,    TRUE,
                                      														End,
																		MenuChild, MenuitemObject,
                                        													//MUIA_Menuitem_Checkit,     TRUE,
                                        													MUIA_Menuitem_Title,    data->ctxt_prioritylow,
                                        													MUIA_Menuitem_CopyStrings,  FALSE,
																							MUIA_Menuitem_Enabled,    TRUE,
                                      														End, 
																		End,
            										MenuChild, Menuitem(data->ctxt_listadd, NULL, FALSE, FALSE, 2),
            										MenuChild, Menuitem(data->ctxt_listremove, NULL, FALSE, FALSE, 3),
            										MenuChild, Menuitem(data->ctxt_remove, NULL, FALSE, FALSE, 4),
													End,
          		End;
      		}
   		RETURN(data->context_menu);
  		return (IPTR)data->context_menu;
	}

/// OVERLOAD(MUIM_NList_Compare)
//  Message listview compare method
OVERLOAD(MUIM_NList_Compare)
	{
  		struct MUIP_NList_Compare *ncm = (struct MUIP_NList_Compare *)msg;
  		struct Server *entry1 = (struct Server *)ncm->entry1;
  		struct Server *entry2 = (struct Server *)ncm->entry2;
		LONG col1 = ncm->sort_type1 & MUIV_NList_TitleMark_ColMask;
  		LONG col2 = ncm->sort_type2 & MUIV_NList_TitleMark2_ColMask;
		LONG cmp;
    	
		if(ncm->sort_type1 == (LONG)MUIV_NList_SortType_None)
  			{
    			RETURN(0);
    			return 0;
  			}
  		if(ncm->sort_type1 & MUIV_NList_TitleMark_TypeMask) cmp = ServerCompare(entry2, entry1, col1);
  		else cmp = ServerCompare(entry1, entry2, col1);
  		if(cmp != 0 || col1 == col2)
  			{
    			RETURN(cmp);
    			return cmp;
  			}
  		if(ncm->sort_type2 & MUIV_NList_TitleMark2_TypeMask) cmp = ServerCompare(entry2, entry1, col2);
  		else cmp = ServerCompare(entry1, entry2, col2);
  		RETURN(cmp);
  		return cmp;
	}	
	
///
/// OVERLOAD(MUIM_NList_Display)
OVERLOAD(MUIM_NList_Display)
	{
  		struct MUIP_NList_Display *ndm = (struct MUIP_NList_Display *)msg;
		//struct Server *sv = (struct Server *)ndm->entry;
		struct Server *sv = (struct Server *)ndm->entry;
		
  		ENTER();
  		if(sv != NULL)
  			{
    			GETDATA;
				snprintf(data->name, sizeof(data->name), "\033o[%d] %s", SI_STATUS, sv->Name);
				ndm->strings[0] = data->name;
				snprintf(data->ip, sizeof(data->ip), "%s:%d", sv->IP, sv->Port);
				ndm->strings[1] = data->ip;
    			ndm->strings[2] = sv->Description;
				snprintf(data->ping, sizeof(data->ping), " %d",  sv->Ping);
				ndm->strings[3] = data->ping;
				snprintf(data->users, sizeof(data->users), "%ld", sv->Users);
				ndm->strings[4] = data->users;
				FormatSize(sv->maxusers, data->maxusers, sizeof(data->maxusers), SF_MIXED);
				ndm->strings[5] = data->maxusers;
				FormatSize(sv->Files, data->files, sizeof(data->files), SF_MIXED);
				ndm->strings[6] = data->files;
				if(sv->Prefs == 1) snprintf(data->prefs, sizeof(data->prefs), "%s", (STRPTR)GetStr(MSG_SE_HIGHPRI));
				else if(sv->Prefs == 2) snprintf(data->prefs, sizeof(data->prefs), "%s", (STRPTR)GetStr(MSG_SE_LOWPRI));
				else snprintf(data->prefs, sizeof(data->prefs), "%s", (STRPTR)GetStr(MSG_SE_NORMALPRI));
    			ndm->strings[7] = data->prefs;
				FormatSize(sv->softfiles, data->limitebassefiles, sizeof(data->limitebassefiles), SF_MIXED);
				ndm->strings[8] = data->limitebassefiles;
				FormatSize(sv->hardfiles, data->limitehautefiles, sizeof(data->limitehautefiles), SF_MIXED);
				ndm->strings[9] = data->limitehautefiles;
    			ndm->strings[10] = sv->Version;
				snprintf(data->lowidusers, sizeof(data->lowidusers), "%ld", sv->lowidusers);
				ndm->strings[11] = data->lowidusers;
  			}
  		else
  			{
    			ndm->strings[0] = (STRPTR)GetStr(MSG_SE_NAME);
    			ndm->strings[1] = (STRPTR)GetStr(MSG_SE_IP);
    			ndm->strings[2] = (STRPTR)GetStr(MSG_SE_DESCRIPTION);
				ndm->strings[3] = (STRPTR)GetStr(MSG_SE_PING); 
				ndm->strings[4] = (STRPTR)GetStr(MSG_SE_USERS);
				ndm->strings[5] = (STRPTR)GetStr(MSG_SE_MAXUSERS);
    			ndm->strings[6] = (STRPTR)GetStr(MSG_SE_FILES);
    			ndm->strings[7] = (STRPTR)GetStr(MSG_SE_PREFS);
				ndm->strings[8] = (STRPTR)GetStr(MSG_SE_SOFTFILES);
    			ndm->strings[9] = (STRPTR)GetStr(MSG_SE_HARDFILES);
    			ndm->strings[10] = (STRPTR)GetStr(MSG_SE_VERSION);
				ndm->strings[11] = (STRPTR)GetStr(MSG_SE_LOWIDUSERS);
  			}
  		RETURN(0);
  		return 0;
	}

///

/* Public Methods */
/// DECLARE(MakeFormat)
// création de la définition du format pour la listview des serveurs
DECLARE(MakeFormat)
{
 	static const int defwidth[11] = { -1,-1,-1,-1, -1,-1,-1,-1,-1, -1,-1};
 	char format[SIZE_LARGE];
 	BOOL first = TRUE;
 	int i;

 	ENTER();
 	*format = '\0';
 	for(i = 0; i < 11; i++)
  		{
      		int p;

      		if(first) first = FALSE;
      		else strlcat(format, " TBAR,", sizeof(format));
      		p = strlen(format);
      		snprintf(&format[p], sizeof(format)-p, "COL=%d W=%d", i, defwidth[i]);
      		if(i ==3  || i == 4 || i == 5 || i == 6 || i == 8 || i == 10 || i == 11 || i == 13) strlcat(format, " P=\033r", sizeof(format));
			else strlcat(format, " P=\033l", sizeof(format));
    	}
  	set(obj, MUIA_NList_Format, format);
  	RETURN(0);
  	return(0);
}

