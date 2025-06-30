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

#ifndef MUI_CONFIGPAGE_H
#define MUI_CONFIGPAGE_H 1

#ifndef SDI_COMPILER_H
  #include "SDI_compiler.h"
#endif

/* Exported includes */

#include "Locale.h"
#include "MUIObjects.h"
#include "Themes.h"

Object * Bourriquet_NewObject(CONST_STRPTR className, ...);

#define MUIC_ConfigPage "Bourriquet_ConfigPage"

#define ConfigPageObject Bourriquet_NewObject(MUIC_ConfigPage

#define MUIM_ConfigPage_ConfigToGUI                   0x80009c00
#define MUIM_ConfigPage_GUIToConfig                   0x80009c01
#define MUIM_ConfigPage_ConfigUpdate                  0x80009c02
#define MUIA_ConfigPage_ConfigUpdate                  0x80009c03

struct MUIP_ConfigPage_ConfigToGUI
{
  ULONG methodID;
};

struct MUIP_ConfigPage_GUIToConfig
{
  ULONG methodID;
};

struct MUIP_ConfigPage_ConfigUpdate
{
  ULONG methodID;
  ULONG sourcePage;
};

#endif /* MUI_CONFIGPAGE_H */
