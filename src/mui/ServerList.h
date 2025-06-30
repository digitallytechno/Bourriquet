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
/* Ce fichier est automatiquement généré avec GenClasses - NE PAS EDITER!!! */

#ifndef MUI_SERVERLIST_H
#define MUI_SERVERLIST_H 1

#ifndef SDI_COMPILER_H
  #include "SDI_compiler.h"
#endif

/* Exported includes */

#include "Themes.h"
#include "Stringsizes.h"

Object * Bourriquet_NewObject(CONST_STRPTR className, ...);

#define MUIC_ServerList "Bourriquet_ServerList"

#define ServerListObject Bourriquet_NewObject(MUIC_ServerList

#define MUIM_ServerList_MakeFormat                    0x8000a200
#define MUIA_ServerList_ImageArray                    0x8000a201
#define MUIA_ServerList_SortOrderReversed             0x8000a202

struct MUIP_ServerList_MakeFormat
{
  	ULONG methodID;
};

#endif /* MUI_SERVERLIST_H */
