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
Description: Classe NList pour afficher toutes les infos venant du serveur
***************************************************************************/

#include "InfosServeur_cl.h"
#include <mui/NList_mcc.h>

#include "Bourriquet.h"
#include "DashBoard.h"
//#include "Utilities.h"
#include "Debug.h"

/* INCLUDE
#include "Stringsizes.h"
*/

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
    									MUIA_NList_Title, FALSE,
										MUIA_NList_AutoVisible, FALSE,
    									TAG_MORE, inittags(msg));
	RETURN((IPTR)obj);
  	return(IPTR)obj;
}

///
/// OVERLOAD(MUIM_NList_Display)
OVERLOAD(MUIM_NList_Display)
{
	//struct MUIP_NList_Display *ndm = (struct MUIP_NList_Display *)msg;

	ENTER();
	RETURN(0);
	return(0);
}
///
/* Private Functions */

/* Public Methods */

