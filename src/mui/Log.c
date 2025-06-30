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
 
Superclass:  MUIC_NList
Description: Classe NList pour afficher toutes les infos du journal
***************************************************************************/

#include "Log_cl.h"
#include <mui/NList_mcc.h>

#include "Utilities.h"
#include "Debug.h"

/* CLASSDATA
struct Data
{
  char displayBuffer[SIZE_DEFAULT];
};
*/

/* Overloaded Methods */
/// OVERLOAD(OM_NEW)
OVERLOAD(OM_NEW)
	{
  		ENTER();
  		obj = DoSuperNew(cl, obj,
    									InputListFrame,
    									//MUIA_NList_Title,        FALSE,
    									//MUIA_NList_DragType,     MUIV_NList_DragType_Immediate,
    									//MUIA_NList_DragSortable, TRUE,
    									TAG_MORE, inittags(msg));
  		RETURN((IPTR)obj);
  		return (IPTR)obj;
	}

///
/// OVERLOAD(MUIM_NList_Display)
OVERLOAD(MUIM_NList_Display)
	{
  		//struct MUIP_NList_Display *ndm = (struct MUIP_NList_Display *)msg;
  		//struct UserIdentityNode *uin = (struct UserIdentityNode *)ndm->entry;

  		ENTER();
  		/*if(uin != NULL)
  			{
    			GETDATA;
    			snprintf(data->displayBuffer, sizeof(data->displayBuffer), "%d) %s%s" MUIX_N, (int)ndm->strings[-1]+1,
    						                                                                              uin->active == FALSE ? MUIX_I : "",                                                                uin->description);

    			dm->strings[0] = data->displayBuffer;
  			}*/
  		RETURN(0);
  		return 0;
}
///
/* Private Functions */

/* Public Methods */

