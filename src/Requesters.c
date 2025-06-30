/***************************************************************************
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
 
***************************************************************************/
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

#include <clib/alib_protos.h>
#include <libraries/iffparse.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "Extrasrc.h"
#include "SDI_hook.h"

#include "Bourriquet.h"

#include "mui/ClassesExtra.h"
#include "mui/BourriquetApplication.h"
#include "mui/GenericRequestWindow.h"

#include "Locale.h"
#include "MethodStack.h"
#include "MUIObjects.h"
#include "Requesters.h"
#include "Threads.h"

#include "Debug.h"

#define REQUESTER_RETURNID          1000

LONG BOURRIQUETMUIRequest(const Object *app, const Object *parent, LONG flags, const char *tit, const char *gad, const char *format, ...)
	{
  		LONG result = -1;
  		va_list args;
  		char *reqtxt = NULL;

  		ENTER();
  		va_start(args, format);
  		if(vasprintf(&reqtxt, format, args) != -1) 	result = BOURRIQUETMUIRequestA(app, parent, flags, tit, gad, reqtxt);
 		va_end(args);
  		free(reqtxt);
  		RETURN(result);
  		return result;
	}

LONG BOURRIQUETMUIRequestA(const Object *app, const Object *parent, LONG flags, const char *title, const char *gadgets, const char *reqtxt)
	{
  		LONG result = -1;

  		ENTER();
  		if(IsMainThread() == FALSE)
  			{
    			result = PushMethodOnStackWait(G->App, 7, MUIM_BourriquetApplication_MUIRequestA, app, parent, flags, title, gadgets, reqtxt);
    			RETURN(result);
    			return result;
  			}
  		if(app == NULL)
  			{
    			if(IntuitionBase != NULL)
    				{
      					struct EasyStruct ErrReq;
      					char *stripped_gadgets;
      					char *p;

            			stripped_gadgets = strdup(gadgets);
      					while((p = strchr(stripped_gadgets, '_')) != NULL) memmove(p, p+1, strlen(p)+1);
      					ErrReq.es_StructSize   = sizeof(struct EasyStruct);
      					ErrReq.es_Flags        = 0;
      					ErrReq.es_Title        = (STRPTR)title;
      					ErrReq.es_TextFormat   = (STRPTR)reqtxt;
      					ErrReq.es_GadgetFormat = (STRPTR)stripped_gadgets;
      					result = EasyRequestArgs(NULL, &ErrReq, NULL, NULL);
      					free(stripped_gadgets);
    				}
  			}
  		else
  			{
    			Object *win;

    			win = GenericRequestWindowObject,
      						MUIA_Window_Title,  title != NULL ? title : GetStr(MSG_CONFIRM_REQ),
      						MUIA_Window_RefWindow,  parent,
      						MUIA_GenericRequestWindow_Body,       reqtxt,
      						MUIA_GenericRequestWindow_Buttons,    gadgets,
      						MUIA_GenericRequestWindow_Floattext,  isFlagSet(flags, MUIF_REQ_FLOATTEXT),
    			End;
    			if(win != NULL)
    				{
      					DoMethod(win, MUIM_Notify, MUIA_GenericRequestWindow_Result, MUIV_EveryTime, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, REQUESTER_RETURNID);
      					set(G->App, MUIA_Application_Sleep, TRUE);
      					if(SafeOpenWindow(win) == TRUE)
      						{
        						ULONG signals = 0;

        						do
        							{
          								if(DoMethod(G->App, MUIM_Application_NewInput, &signals) == REQUESTER_RETURNID)
          									{
            									result = xget(win, MUIA_GenericRequestWindow_Result);
            									break;
          									}
          								if(signals != 0)  signals = Wait(signals | SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F);
          								if(isFlagSet(signals, SIGBREAKF_CTRL_C)) break;
          								//if(isFlagSet(signals, SIGBREAKF_CTRL_F)) PopUp();
        							}
        						while(TRUE);
      						}
      					DoMethod(G->App, OM_REMMEMBER, win);
      					MUI_DisposeObject(win);
      					set(G->App, MUIA_Application_Sleep, FALSE);
    				}
  				}
  		RETURN(result);
  		return result;
	}