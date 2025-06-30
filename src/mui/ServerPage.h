/***************************************************************************
Bourriquet MorphOS
Copyright (C) 2024-2025 digitally

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

#ifndef MUI_SERVERPAGE_H
#define MUI_SERVERPAGE_H 1

#ifndef SDI_COMPILER_H
  #include "SDI_compiler.h"
#endif

Object * Bourriquet_NewObject(CONST_STRPTR className, ...);

#define MUIC_ServerPage "Bourriquet_ServerPage"

#define ServerPageObject Bourriquet_NewObject(MUIC_ServerPage

#define MUIM_ServerPage_Update                        0x8000fb00
#define MUIM_ServerPage_SingleConnection              0x8000fb01
#define MUIM_ServerPage_Entry                         0x8000fb02
#define MUIM_ServerPage_AddServer                     0x8000fb03

struct MUIP_ServerPage_Update
{
  ULONG methodID;
  ULONG nbservers;
};

struct MUIP_ServerPage_SingleConnection
{
  ULONG methodID;
};

struct MUIP_ServerPage_Entry
{
  ULONG methodID;
};

struct MUIP_ServerPage_AddServer
{
  ULONG methodID;
};

#endif /* MUI_SERVERPAGE_H */
