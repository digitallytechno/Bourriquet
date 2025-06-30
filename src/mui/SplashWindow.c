/*****************************************************
Bourriquet MorphOS
Copyright (C) 2022 Digitally

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
 
Superclass:  MUIC_Window
*****************************************************/
#include "SplashWindow_cl.h"

#include <string.h>
#include <proto/dos.h>
#include <proto/muimaster.h>
#include <mui/Guigfx_mcc.h>
#include <mui/BetterString_mcc.h>

#include "Bourriquet.h"
#include "Global.h"
#include "Stringsizes.h"

#include "Busy.h"
#include "Locale.h"
#include "MUIObjects.h"

#include "mui/ImageArea.h"
#include "mui/BourriquetApplication.h"
#include "Debug.h"

/* CLASSDATA
struct Data
{
  Object *windowGroup;
  Object *statusGauge;
  Object *progressGauge;
  BOOL progressGaugeActive;
  struct TimeVal last_gaugemove;
  APTR lastBusy;
  char statusInfoText[80];
  char progressInfoText[80];
};
*/

/* INCLUDE
#include "timeval.h"
*/

OVERLOAD(OM_NEW)
{
    Object *progressGauge;

    if((progressGauge = GaugeObject,
                        GaugeFrame,
                        MUIA_Gauge_InfoText, "",
                        MUIA_Gauge_Horiz,    TRUE,
                        End) != NULL)
      {
		char logopath[SIZE_PATHFILE];
		char *compileInfo;
        Object *windowGroup;
        Object *infoGroup;
        Object *statusGauge;

		compileInfo = (char *)xget(G->App, MUIA_BourriquetApplication_CompileInfo);
	    AddPath(logopath, G->ThemesDir, "default/logo", sizeof(logopath));
		//D(DBF_STARTUP, "%s", logopath);
        if((obj = DoSuperNew(cl, obj,
                              MUIA_Window_DragBar,     FALSE,
                              MUIA_Window_CloseGadget, FALSE,
                              MUIA_Window_DepthGadget, FALSE,
                              MUIA_Window_SizeGadget,  FALSE,
                              MUIA_Window_LeftEdge,    MUIV_Window_LeftEdge_Centered,
                              MUIA_Window_TopEdge,     MUIV_Window_TopEdge_Centered,
                              MUIA_Window_Activate,    TRUE,
                              WindowContents, windowGroup = VGroup,
                                                              MUIA_Background, MUII_GroupBack,
                                                              Child, HGroup,
                                                                MUIA_Group_Spacing, 0,
                                                                  Child, HSpace(0),
																  Child, MakeImageObject("logo", logopath),
                                                                  Child, HSpace(0),
                                                                    End,
                                                                  Child, HCenter((infoGroup = VGroup,
																							  Child, CLabel(GetStr(MSG_COPYRIGHT1)), 
																							  Child, TextObject,
                                                                                                MUIA_Text_PreParse, "\033c\033u\0335",
                                                                                                MUIA_Text_Contents, bourriqueturl,
                                                                                                MUIA_Text_Copy,     FALSE,
																								End,
																							  Child, RectangleObject,
                                                                                                MUIA_Rectangle_HBar, TRUE,
                                                                                                MUIA_FixHeight, 8,
																								End,
                                                                                              Child, ColGroup(2),
                                                                                                MUIA_Font, MUIV_Font_Tiny,
                                                                                                MUIA_Group_HorizSpacing, 8,
                                                                                                MUIA_Group_VertSpacing, 2,
																								  Child, Label(GetStr(MSG_VERSION)), 
                                                                                                  Child, LLabel(bourriquetversionver),
																								  Child, Label(GetStr(MSG_COMPILATION_DATE)), 
																								  Child, LLabel((char *)compileInfo),
                                                                                                  End,
                                                                                                End)),
                                                                                              Child, statusGauge = GaugeObject,
                                                                                                                    GaugeFrame,
                                                                                                                    MUIA_Gauge_InfoText, "",
                                                                                                                    MUIA_Gauge_Horiz,    TRUE,
                                                                                                  End,
                                                                                                End,
                                                                                                TAG_MORE, inittags(msg))) != NULL)
			{
				GETDATA;
				DoMethod(G->App, OM_ADDMEMBER, obj);
				data->windowGroup = windowGroup;
				data->statusGauge = statusGauge;
				data->progressGauge = progressGauge;
				data->progressGaugeActive = FALSE;
			}
		else MUI_DisposeObject(progressGauge);
      }
    return (IPTR)obj;
}

///
/// OVERLOAD(OM_DISPOSE)
OVERLOAD(OM_DISPOSE)
{
    GETDATA;

	if(data->progressGaugeActive == FALSE) MUI_DisposeObject(data->progressGauge);
    return DoSuperMethodA(cl, obj, msg);
}

DECLARE(StatusChange) // char *txt, LONG percent
{
    GETDATA;

    ENTER();
	strlcpy(data->statusInfoText, msg->txt, sizeof(data->statusInfoText));
	xset(data->statusGauge, MUIA_Gauge_InfoText, msg->txt, MUIA_Gauge_Current,  msg->percent);
	DoMethod(_app(obj), MUIM_Application_InputBuffered);
    RETURN(0);
    return 0;
}

///
/// DECLARE(ProgressChange)
DECLARE(ProgressChange) // struct BusyNode *busy
{
	GETDATA;

    ENTER();
	if(msg->busy != data->lastBusy || TimeHasElapsed(&data->last_gaugemove, 250000) == TRUE)
      {
		D(DBF_STARTUP, "splashwindow...ProgressChange");
		if(msg->busy != NULL)
          {
            if(msg->busy->progressMax > 0 && msg->busy->progressCurrent <= msg->busy->progressMax)
              {
				D(DBF_STARTUP, "splashwindow...progressMax > 0 && progressCurrent <= progressMax");
                set(data->statusGauge, MUIA_Gauge_InfoText, msg->busy->infoText);
                if(msg->busy != data->lastBusy) memset(&data->last_gaugemove, 0, sizeof(data->last_gaugemove));
                if(msg->busy->progressCurrent < msg->busy->progressMax)
                  {
					D(DBF_STARTUP, "splashwindow...progressMax > 0 && progressCurrent < progressMax");
                    snprintf(data->progressInfoText, sizeof(data->progressInfoText), "%%ld/%ld", msg->busy->progressMax);
                    xset(data->progressGauge,
                          MUIA_Gauge_InfoText, data->progressInfoText,
                          MUIA_Gauge_Max, msg->busy->progressMax,
                          MUIA_Gauge_Current, msg->busy->progressCurrent);
                    if(data->progressGaugeActive == FALSE && DoMethod(data->windowGroup, MUIM_Group_InitChange))
                      {
						D(DBF_STARTUP, "splashwindow...progressGaugeActive == 0");
                        DoMethod(data->windowGroup, OM_ADDMEMBER, data->progressGauge);
                        data->progressGaugeActive = TRUE;
                        DoMethod(data->windowGroup, MUIM_Group_ExitChange);
                      }
                  }
              }
          }
        if(msg->busy == NULL || msg->busy->progressCurrent >= msg->busy->progressMax)
          {
            if(data->progressGaugeActive == TRUE && DoMethod(data->windowGroup, MUIM_Group_InitChange))
              {
				D(DBF_STARTUP, "splashwindow...progressGaugeActive == 1");
                DoMethod(data->windowGroup, OM_REMMEMBER, data->progressGauge);
                data->progressGaugeActive = FALSE;
                set(data->statusGauge, MUIA_Gauge_InfoText, data->statusInfoText);
                DoMethod(data->windowGroup, MUIM_Group_ExitChange);
              }
		  }
       data->lastBusy = msg->busy;
      }
	DoMethod(_app(obj), MUIM_Application_InputBuffered);
    RETURN(0);
    return 0;
}

///
/// DECLARE(SelectUser)
DECLARE(SelectUser)
{
    //GETDATA;
    LONG user = -1;

    ENTER();
    RETURN(user);
    return (ULONG)user;
}

