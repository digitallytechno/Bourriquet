/*****************************************************
Bourriquet MorphOS
Copyright(C) 2022 Digitally

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
 
Superclass:  MUIC_Application 
Description: Sous-classe Application  qui gère tous les éléments "globaux". 
*****************************************************/
#include "BourriquetApplication_cl.h"

#include <string.h>

#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/muimaster.h>
#include <proto/rexxsyslib.h>
#include <proto/timer.h>
#include <mui/NList_mcc.h>
#include <mui/NListtree_mcc.h>
#include <workbench/icon.h>

#include "Bourriquet.h"
#include "Global.h"
#include "Stringsizes.h"
#include "Locale.h"
#include "MUIObjects.h"
#include "Threads.h"
#include "gitrev.h"
#include "mui/AboutWindow.h"
#include "Debug.h"
#include "Error.h"

/* CLASSDATA
struct Data
{
 	Object *aboutWindow;
  	Object *connection;
	Object *connectionWindow;
  	char compileInfo[SIZE_DEFAULT];
  	BOOL iconified;
};
*/

/* Overloaded Methods */
/// OVERLOAD(OM_NEW)
OVERLOAD(OM_NEW)
	{
		char filebuf[SIZE_PATHFILE];
		char verbuf[CBD_TITLELEN];
		static const char *const Classes[] = { "NBalance.mcc",
    	                                     "NList.mcc",
    	                                     "TheBar.mcc",
    	                                     NULL
    	                                   };

  		// now we load the standard icons like (check.info, new.info etc)
  		// but we also try to take care of different icon.library versions.
		AddPath(filebuf, G->ProgDir, G->ProgName, sizeof(filebuf));
		if(LIB_VERSION_IS_AT_LEAST(IconBase, 44, 0) == TRUE) G->HideIcon = GetIconTags(filebuf, ICONGETA_FailIfUnavailable, FALSE, TAG_DONE);
		else G->HideIcon = GetDiskObjectNew(filebuf);
		snprintf(verbuf, sizeof(verbuf), "%sVER: Bourriquet %s (%s)", "$", bourriquetver, bourriquetversiondate);
		if((obj = (Object *)DoSuperNew(cl, obj,
																MUIA_Application_Author,         "Digitally",
																MUIA_Application_Base,           "Bourriquet",
																MUIA_Application_Title,          "Bourriquet",
																MUIA_Application_Version,        verbuf,
																MUIA_Application_Copyright,      bourriquetcopyright,
																MUIA_Application_Description,    GetStr(MSG_APPLICATION_DESCRIPTION),
																MUIA_Application_UseRexx,        FALSE,
																MUIA_Application_UsedClasses,    Classes,
    															MUIA_Application_HelpFile,       " ",
																MUIA_Application_DiskObject,     G->HideIcon,
																TAG_MORE, inittags(msg))) != NULL)
			{
				GETDATA;
				struct DateTime dt;

				dt.dat_Stamp.ds_Days   = bourriquetversiondays;
				dt.dat_Stamp.ds_Minute = 0;
				dt.dat_Stamp.ds_Tick   = 0;
				dt.dat_Format          = FORMAT_DEF;
				dt.dat_Flags           = 0L;
				dt.dat_StrDay          = NULL;
				dt.dat_StrDate         = data->compileInfo;
				dt.dat_StrTime         = NULL;
				DateToStr(&dt);
				data->compileInfo[31] = '\0';  
				snprintf(data->compileInfo, sizeof(data->compileInfo), "%s (%s, "GIT_REVSTR")", data->compileInfo, bourriquetcompiler);
				if(GetTagData(ATTR(Hidden), FALSE, inittags(msg)) != FALSE) set(obj, MUIA_Application_Iconified, TRUE);
				//DoMethod(obj, MUIM_Notify, MUIA_Application_DoubleStart, TRUE, MUIV_Notify_Self, 1, METHOD(PopUp));
			}
  		return (IPTR)obj;
	}

///
/// OVERLOAD(OM_DISPOSE)
OVERLOAD(OM_DISPOSE)
	{
  		return DoSuperMethodA(cl, obj, msg);
	}

///
/// OVERLOAD(OM_GET)
OVERLOAD(OM_GET)
	{
   		GETDATA;
   		IPTR *store = ((struct opGet *)msg)->opg_Storage;

   		switch(((struct opGet *)msg)->opg_AttrID)
     		{
        		case ATTR(CompileInfo) : 
					{
						*store = (IPTR)data->compileInfo; 
						return TRUE;
					}
     		}
  		return DoSuperMethodA(cl, obj, msg);
	}

///
/// OVERLOAD(OM_SET)
OVERLOAD(OM_SET)
	{
  		GETDATA;
  		struct TagItem *tags = inittags(msg), *tag;

  		while((tag = NextTagItem((APTR)&tags)) != NULL)
  			{
    			switch(tag->ti_Tag)
    				{
      					case MUIA_Application_Iconified:
      						{
         						D(DBF_STARTUP, "l'application a été %s", tag->ti_Data ? "iconifiée" : "désiconifiée");
								data->iconified = tag->ti_Data;
      						}
      						break;
      					case MUIA_Application_Active:
      						{
        						D(DBF_STARTUP, "l'application est %s", tag->ti_Data ? "active" : "désactivée");
        						set(obj, MUIA_Application_Sleep, !tag->ti_Data);
      						}
      						break;
    				}
  			}
  		return DoSuperMethodA(cl, obj, msg);
	}

///
/// OVERLOAD(MUIM_Application_PushMethod)
OVERLOAD(MUIM_Application_PushMethod)
	{
  		struct MUIP_Application_PushMethod *pm = (struct MUIP_Application_PushMethod *)msg;
  		IPTR rc;

  		ENTER();  
  		if(LIB_VERSION_IS_AT_LEAST(MUIMasterBase, 20, 0) == FALSE) pm->count &= 0x0000000f;
  		rc = DoSuperMethodA(cl, obj, msg);
  		RETURN(rc);
  		return rc;
	}

///
/// DECLARE(ShowError)
// show an error message and free() the pointer
// NOTE: the message must have been allocated by malloc() or similar!
DECLARE(ShowError) // char *message
{
	ENTER();
	ER_NewError(msg->message);
	free(msg->message);
	RETURN(0);
	return 0;
}

///
/// DECLARE(ShowWarning)
// show a warning message and free() the pointer
// NOTE: the message must have been allocated by malloc() or similar!
DECLARE(ShowWarning) // char *message
{
	ENTER();
	D(DBF_NET, "message %s ", msg->message);
	ER_NewWarning(msg->message);
	free(msg->message);
	RETURN(0);
	return 0;
}

///
/// DECLARE(DisposeWindow)
DECLARE(DisposeWindow) // Object *window
	{
  		ENTER();
  		if(msg->window != NULL)
  			{
    			set(msg->window, MUIA_Window_Open, FALSE);
    			DoMethod(obj, OM_REMMEMBER, msg->window);
    			MUI_DisposeObject(msg->window);
  			}
  		RETURN(0);
  		return 0;
	}

///
/// DECLARE(GotoURL)
// invoke the GotoURL() function with the given parameters
// NOTE: the error message must have been allocated by malloc() or similar!
DECLARE(GotoURL) // char *url, ULONG newWindow
	{
  		ENTER();
  		//GotoURL(msg->url, (BOOL)msg->newWindow);
  		//free(msg->url);
	  	RETURN(0);
	  	return 0;
	}

///
/// DECLARE(PopUp)
DECLARE(PopUp)
	{
  		ENTER();
  		//if(G->MA != NULL && G->MA->GUI.WI != NULL) PopUp();
  		RETURN(0);
  		return 0;
	}


///
/// DECLARE(OpenAboutWindow)
// show the about window
DECLARE(OpenAboutWindow)
	{
  		GETDATA;

  		ENTER();
  		if(data->aboutWindow == NULL)
  			{
    			data->aboutWindow = AboutWindowObject, End;
    			if(data->aboutWindow != NULL) 
					{
						DoMethod(data->aboutWindow, MUIM_Notify, MUIA_Window_Open, FALSE, obj, 4, MUIM_Application_PushMethod,obj, 1, MUIM_BourriquetApplication_CloseAboutWindow);
  					}
			}
  		if(data->aboutWindow != NULL) SafeOpenWindow(data->aboutWindow);
    	RETURN(0);
  		return 0;
	}

///
/// DECLARE(CloseAboutWindow)
// close the about window
DECLARE(CloseAboutWindow)
	{
  		GETDATA;

  		ENTER();
  		DoMethod(obj, MUIM_BourriquetApplication_DisposeWindow, data->aboutWindow);
  		data->aboutWindow = NULL;
  		RETURN(0);
  		return 0;
	}

	///
/// DECLARE(MUIRequestA)
// call MUI requester
DECLARE(MUIRequestA) // Object *app, Object *win, LONG flags, const char *title, const char *gadgets, const char *reqtxt
{
 	LONG result;
  		
	ENTER();
  	result = BOURRIQUETMUIRequestA(msg->app, msg->win, msg->flags, msg->title, msg->gadgets,  msg->reqtxt);
  	RETURN(result);
  	return(result);
}
