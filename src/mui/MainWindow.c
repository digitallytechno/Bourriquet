/***************************************************************************
Bourriquet MorphOS 
Copyright (C) 2022 digitally 

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
 Description: Classe Mainwindow carrying all main GUI elements of YAM
***************************************************************************/

#include "MainWindow_cl.h"

#include <proto/muimaster.h>
#include <libraries/iffparse.h>
#include <mui/NBalance_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>

#include "Bourriquet.h"

//#include "mui/MainMailListGroup.h"
//#include "mui/QuickSearchBar.h"
//#include "mui/ReadMailGroup.h"

#include "Busy.h"
#include "Locale.h"
#include "MUIObjects.h"
#include "Requesters.h"
#include "DashBoard.h"

#include "Debug.h"

/* INCLUDE
//#include "YAM_find.h"
*/

/* Overloaded Methods */
/// OVERLOAD(OM_SET)
OVERLOAD(OM_SET)
	{
  		IPTR result = 0;
  		struct TagItem *tags = inittags(msg), *tag;
		int i;
		
  		ENTER();
  		while((tag = NextTagItem((APTR)&tags)) != NULL)
  			{
  		 	 	switch(tag->ti_Tag)
    				{
      					case MUIA_Window_DefaultObject:
      						{
                				//if((Object *)tag->ti_Data == NULL) tag->ti_Data = xget(G->MA->GUI.PG_MAILLIST, MUIA_MainMailListGroup_ActiveListviewObject);		
      						}
      						break;
    				}
  			}
  		result = DoSuperMethodA(cl, obj, msg);
  		RETURN(result);
  		return result;
	}

///
/// OVERLOAD(MUIM_Window_Snapshot)
OVERLOAD(MUIM_Window_Snapshot)
	{
  		struct MUIP_Window_Snapshot *snap = (struct MUIP_Window_Snapshot *)msg;
  		IPTR result;

  		ENTER();
  		// remember the weights for snapshot operations, but not for unsnapshot operations
  		/*if(snap->flags != 0)
  			{
    			// get the weights according to their GUI elements
    			G->Weights[0] = xget(G->MA->GUI.LV_FOLDERS,  MUIA_HorizWeight);
    			G->Weights[1] = xget(G->MA->GUI.GR_MAILVIEW, MUIA_HorizWeight);
    			// if the embedded read pane objects are currently active we save their weight values
    			if(C->EmbeddedReadPane == TRUE)
    				{
      					G->Weights[6] = xget(G->MA->GUI.GR_MAILLIST, MUIA_VertWeight);
      					G->Weights[7] = xget(G->MA->GUI.MN_EMBEDDEDREADPANE, MUIA_VertWeight);
      					G->Weights[8] = xget(G->MA->GUI.MN_EMBEDDEDREADPANE, MUIA_ReadMailGroup_HGVertWeight);
      					G->Weights[9] = xget(G->MA->GUI.MN_EMBEDDEDREADPANE, MUIA_ReadMailGroup_TGVertWeight);
    				}
    			// save the FolderListtree order
    			FO_SaveTree();
   		 		// make sure the layout is saved
    			SaveLayout(TRUE);
  			}*/
  		result = DoSuperMethodA(cl, obj, msg);
  		RETURN(result);
  		return result;
	}

///

/* Fonctions privées */
	
/* Methodes publiques */
/// DECLARE(ShowErrors)
// Montre la fenêtre d'erreur
DECLARE(ShowErrors)
	{
  		ENTER();
  		//ER_NewError(NULL);
  		RETURN(0);
  		return 0;
	}

///
/// DECLARE(Relayout)
// relayout the main window after a configuration change
DECLARE(Relayout)
	{
  		ENTER();
  		/*if(DoMethod(G->MA->GUI.GR_MAIN, MUIM_Group_InitChange))
  			{
    			BOOL showbar = TRUE;
    			
				switch(C->InfoBarPos)
    				{
      					case IB_POS_TOP:
      						{
        						DoMethod(G->MA->GUI.GR_MAIN, MUIM_Group_Sort, G->MA->GUI.IB_INFOBAR,
        																	                                              G->MA->GUI.GR_TOP,
        																	                                              G->MA->GUI.GR_HIDDEN,
       																		                                               G->MA->GUI.GR_BOTTOM, NULL);
      						}
      						break;
      					case IB_POS_CENTER:
      						{
        						DoMethod(G->MA->GUI.GR_MAIN, MUIM_Group_Sort, G->MA->GUI.GR_TOP,
        																	                                              G->MA->GUI.GR_HIDDEN,
        																	                                              G->MA->GUI.IB_INFOBAR,
        																	                                              G->MA->GUI.GR_BOTTOM, NULL);
      						}
      						break;
      					case IB_POS_BOTTOM:
      						{
        						DoMethod(G->MA->GUI.GR_MAIN, MUIM_Group_Sort, G->MA->GUI.GR_TOP,
                                                      																		G->MA->GUI.GR_HIDDEN,
                                                      																		G->MA->GUI.GR_BOTTOM,
                                                      																		G->MA->GUI.IB_INFOBAR, NULL);
      						}
      						break;
      					default:
      						{
        						showbar = FALSE;
      						}
    				}
    			// if the InfoBar is enabled by the user we make sure we show it
    			set(G->MA->GUI.IB_INFOBAR, MUIA_ShowMe, showbar);
    			DoMethod(G->MA->GUI.GR_MAIN, MUIM_Group_ExitChange);
  			}
  		if(DoMethod(G->MA->GUI.GR_MAILLIST, MUIM_Group_InitChange))
  			{
    			BOOL showbar = TRUE;

    			switch(C->QuickSearchBarPos)
    				{
      					case QSB_POS_TOP:
      						{
        						DoMethod(G->MA->GUI.GR_MAILLIST, MUIM_Group_Sort, G->MA->GUI.GR_QUICKSEARCHBAR,
                                                          																		G->MA->GUI.PG_MAILLIST, NULL);
      						}
      						break;
      					case QSB_POS_BOTTOM:
      						{
        						DoMethod(G->MA->GUI.GR_MAILLIST, MUIM_Group_Sort, G->MA->GUI.PG_MAILLIST,
                                                          																		G->MA->GUI.GR_QUICKSEARCHBAR, NULL);
      						}
      						break;
      					default:
      						{
        						showbar = FALSE;
      						}
      					break;
    				}
    			// if the QuickSearchBar is enabled by the user we make sure we show it
    			set(G->MA->GUI.GR_QUICKSEARCHBAR, MUIA_ShowMe, showbar);
    			DoMethod(G->MA->GUI.GR_MAILLIST, MUIM_Group_ExitChange);
  			}

  // check whether the embedded read pane object is already embeeded in our main
  // window so that we know what to do now
  if(G->MA->GUI.MN_EMBEDDEDREADPANE != NULL && C->EmbeddedReadPane == FALSE)
  {
    // the user want to have the embedded read pane removed from the main
    // window, so lets do it now
    if(DoMethod(G->MA->GUI.GR_MAILVIEW, MUIM_Group_InitChange))
    {
      DoMethod(G->MA->GUI.GR_MAILVIEW, OM_REMMEMBER, G->MA->GUI.MN_EMBEDDEDREADPANE);
      DoMethod(G->MA->GUI.GR_MAILVIEW, OM_REMMEMBER, G->MA->GUI.BL_MAILVIEW);

      // clear all contents of the currenly displayed mail
      DoMethod(G->MA->GUI.MN_EMBEDDEDREADPANE, MUIM_ReadMailGroup_Clear, MUIF_NONE);

      // dispose the objects now that we don't need them anymore
      MUI_DisposeObject(G->MA->GUI.MN_EMBEDDEDREADPANE);
      MUI_DisposeObject(G->MA->GUI.BL_MAILVIEW);

      // and nullify it to make it readdable again
      G->MA->GUI.MN_EMBEDDEDREADPANE = NULL;
      G->MA->GUI.BL_MAILVIEW = NULL;

      DoMethod(G->MA->GUI.GR_MAILVIEW, MUIM_Group_ExitChange);
    }
  }
  else if(G->MA->GUI.MN_EMBEDDEDREADPANE == NULL && C->EmbeddedReadPane == TRUE)
  {
    // the user want to have the embedded read pane added to the main
    // window, so lets do it now and create the object
    if((G->MA->GUI.BL_MAILVIEW = NBalanceObject,
      MUIA_ObjectID, MAKE_ID('B','0','0','2'),
      MUIA_Balance_Quiet, TRUE,
    End) != NULL)
    {
      if((G->MA->GUI.MN_EMBEDDEDREADPANE = ReadMailGroupObject,
        MUIA_ContextMenu, TRUE,
        MUIA_VertWeight, G->Weights[7],
        MUIA_ReadMailGroup_HGVertWeight, G->Weights[8],
        MUIA_ReadMailGroup_TGVertWeight, G->Weights[9],
      End) != NULL)
      {
        if(DoMethod(G->MA->GUI.GR_MAILVIEW, MUIM_Group_InitChange))
        {
          set(G->MA->GUI.GR_MAILLIST, MUIA_VertWeight, G->Weights[6]);

          DoMethod(G->MA->GUI.GR_MAILVIEW, OM_ADDMEMBER, G->MA->GUI.BL_MAILVIEW);
          DoMethod(G->MA->GUI.GR_MAILVIEW, OM_ADDMEMBER, G->MA->GUI.MN_EMBEDDEDREADPANE);

          DoMethod(G->MA->GUI.GR_MAILVIEW, MUIM_Group_ExitChange);
        }
        else
        {
          MUI_DisposeObject(G->MA->GUI.MN_EMBEDDEDREADPANE);
          G->MA->GUI.MN_EMBEDDEDREADPANE = NULL;
          MUI_DisposeObject(G->MA->GUI.BL_MAILVIEW);
          G->MA->GUI.BL_MAILVIEW = NULL;
        }
      }
      else
      {
        MUI_DisposeObject(G->MA->GUI.BL_MAILVIEW);
        G->MA->GUI.BL_MAILVIEW = NULL;
      }
    }
  }*/

  RETURN(0);
  return 0;
}
