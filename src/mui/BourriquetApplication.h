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

#ifndef MUI_BOURRIQUETAPPLICATION_H
#define MUI_BOURRIQUETAPPLICATION_H 1

#ifndef SDI_COMPILER_H
  #include "SDI_compiler.h"
#endif

Object * Bourriquet_NewObject(CONST_STRPTR className, ...);

#define MUIC_BourriquetApplication "Bourriquet_BourriquetApplication"

#define BourriquetApplicationObject Bourriquet_NewObject(MUIC_BourriquetApplication

#define MUIM_BourriquetApplication_ShowError          0x80009d00
#define MUIM_BourriquetApplication_ShowWarning        0x80009d01
#define MUIM_BourriquetApplication_DisposeWindow      0x80009d02
#define MUIM_BourriquetApplication_GotoURL            0x80009d03
#define MUIM_BourriquetApplication_PopUp              0x80009d04
#define MUIM_BourriquetApplication_OpenAboutWindow    0x80009d05
#define MUIM_BourriquetApplication_CloseAboutWindow   0x80009d06
#define MUIM_BourriquetApplication_MUIRequestA        0x80009d07
#define MUIA_BourriquetApplication_Hidden             0x80009d08
#define MUIA_BourriquetApplication_CompileInfo        0x80009d09

struct MUIP_BourriquetApplication_ShowError
{
  ULONG methodID;
  char *message;
};

struct MUIP_BourriquetApplication_ShowWarning
{
  ULONG methodID;
  char *message;
};

struct MUIP_BourriquetApplication_DisposeWindow
{
  ULONG methodID;
  Object *window;
};

struct MUIP_BourriquetApplication_GotoURL
{
  ULONG methodID;
  char *url;
   ULONG newWindow;
};

struct MUIP_BourriquetApplication_PopUp
{
  ULONG methodID;
};

struct MUIP_BourriquetApplication_OpenAboutWindow
{
  ULONG methodID;
};

struct MUIP_BourriquetApplication_CloseAboutWindow
{
  ULONG methodID;
};

struct MUIP_BourriquetApplication_MUIRequestA
{
  ULONG methodID;
  Object *app;
   Object *win;
   LONG flags;
   const char *title;
   const char *gadgets;
   const char *reqtxt;
};

#endif /* MUI_BOURRIQUETAPPLICATION_H */
