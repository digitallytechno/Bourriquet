/***************************************************************************
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
 
Superclass:  MUIC_Window
Description: Fenêtre "au sujet" de l'application
***************************************************************************/
#include "AboutWindow_cl.h"

#include <stdlib.h>
#include <proto/muimaster.h>

#include "Bourriquet.h"
#include "Global.h"
#include "Stringsizes.h"

#include "Locale.h"
#include "MUIObjects.h"

#include "mui/ImageArea.h"
#include "mui/BourriquetApplication.h"

#include "Debug.h"

/*CLASSDATA
struct Data
{
  char *aboutText1;
  char *aboutText2;
  char screenTitle[SIZE_DEFAULT];
};
*/

/* Fonctions privées */

/* Méthodes */
/// OVERLOAD(OM_NEW)
OVERLOAD(OM_NEW)
	{
  		char logopath[SIZE_PATHFILE];
  		char *compileInfo;
  		Object *bt_okay;
  		
  		ENTER();
  		compileInfo = (char *)xget(G->App, MUIA_BourriquetApplication_CompileInfo);
  		AddPath(logopath, G->ThemesDir, "default/logo", sizeof(logopath));
    	if((obj = DoSuperNew(cl, obj,
    											MUIA_Window_CloseGadget,  FALSE,
    											MUIA_Window_SizeGadget,   FALSE,
    											MUIA_Window_LeftEdge,     MUIV_Window_LeftEdge_Centered,
    											MUIA_Window_TopEdge,      MUIV_Window_TopEdge_Centered,
    											MUIA_Window_Width,        MUIV_Window_Width_MinMax(0),
    											WindowContents, VGroup,
      													MUIA_Background, MUII_GroupBack,
      													Child, HGroup,
        															MUIA_Group_Spacing, 0,
        																Child, HSpace(0),
																		Child, MakeImageObject("logo", logopath),
        																Child, HSpace(0),
      																End,
														Child, RectangleObject,
																	  MUIA_Rectangle_HBar, TRUE,
																	  MUIA_FixHeight, 8,
														End,
      													Child, VGroup,
															Child, Label(GetStr(MSG_COPYRIGHT1)),
															Child, TextObject,
																					MUIA_Text_Contents, "\033c\0338 digitallytechno@proton.me",
																					MUIA_InputMode, MUIV_InputMode_RelVerify,
															End,
															Child, TextObject,
																				  MUIA_Text_Contents, "\033c\0338http://",
																				  MUIA_InputMode, MUIV_InputMode_RelVerify,
															End,
															Child, RectangleObject,
																	  MUIA_Rectangle_HBar, TRUE,
																	  MUIA_FixHeight, 8,
															End,
															Child, ColGroup(2),
																	  MUIA_Group_HorizSpacing, 8,
																	  MUIA_Group_VertSpacing, 2,
																	  Child, Label(GetStr(MSG_VERSION)),
																	  Child, LLabel(bourriquetversionver),
																	  Child, Label(GetStr(MSG_COMPILATION_DATE)),
																	  Child, LLabel((char *)compileInfo),
															End,
															Child, RectangleObject,
																	  MUIA_Rectangle_HBar, TRUE,
																	  MUIA_FixHeight, 8,
															End,
											  			End,
														Child, ColGroup(2),
																	  MUIA_Group_HorizSpacing, 10,
																	  MUIA_Group_VertSpacing, 2,
																	  Child, Label("MUI 5"),
																	  Child, LLabel("v22.3 (Jacek Piszczek & Michel Wozniak)"),
																	  Child, Label("NList.mcc"),
																	  Child, LLabel("v20.147 (NList Open Source Team)"),
																	  Child, Label("NListview.mcc"),
																	  Child, LLabel("v19.102 (NList Open Source Team)"),
																	  Child, Label("TheBar.mcc"),
																	  Child, LLabel("v26.22 (TheBar Open Source Team)"),
														End,
														Child, RectangleObject, 
																	MUIA_Rectangle_HBar, TRUE,
																	MUIA_FixHeight, 8,
														End,
														Child, ColGroup(2),
																	  MUIA_Group_HorizSpacing, 10,
																	  MUIA_Group_VertSpacing, 2,
																	  Child, Label("Code Bourriquet"),
																	  Child, LLabel("SixK <dasixk@free.fr>"),
																	  Child, Label("Code GUI"),
																	  Child, LLabel("Digitally"),
											  			End,
														Child, RectangleObject, 
																	MUIA_Rectangle_HBar, TRUE,
																	MUIA_FixHeight, 8,
														End,													
      													Child, HGroup,
        														Child, RectangleObject, End,
        														Child, bt_okay = MakeButton((STRPTR)GetStr(MSG_OK)),
        														Child, RectangleObject, End,
      													End, 
													End,
		TAG_MORE, inittags(msg))) != NULL)
  			{
    			GETDATA;
        		DoMethod(G->App, OM_ADDMEMBER, obj);
    		    DoMethod(obj,       MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Self, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    			DoMethod(bt_okay,   MUIM_Notify, MUIA_Pressed, FALSE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    			xset(obj, MUIA_Window_Activate, TRUE,
              						MUIA_Window_DefaultObject, bt_okay,
              						MUIA_Window_Title, (STRPTR)GetStr(MSG_ABOUTBOURRIQUET),
              						MUIA_Window_ScreenTitle, CreateScreenTitle(data->screenTitle, sizeof(data->screenTitle), (STRPTR)GetStr(MSG_ABOUTBOURRIQUET)));
  			}
  		RETURN((IPTR)obj);
  		return (IPTR)obj;
	}

///
/// OVERLOAD(OM_DISPOSE)
OVERLOAD(OM_DISPOSE)
	{
  		//GETDATA;
  		IPTR result;

  		ENTER();
	  	result = DoSuperMethodA(cl, obj, msg);
  		RETURN(result);
  		return result;
	}
