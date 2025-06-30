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

#ifndef MUI_SPLASHWINDOW_H
#define MUI_SPLASHWINDOW_H 1

#ifndef SDI_COMPILER_H
  #include "SDI_compiler.h"
#endif

/* Exported includes */

#include "timeval.h"

Object * Bourriquet_NewObject(CONST_STRPTR className, ...);

#define MUIC_SplashWindow "Bourriquet_SplashWindow"

#define SplashWindowObject Bourriquet_NewObject(MUIC_SplashWindow

#define MUIM_SplashWindow_StatusChange                0x80004f00
#define MUIM_SplashWindow_ProgressChange              0x80004f01
#define MUIM_SplashWindow_SelectUser                  0x80004f02

struct MUIP_SplashWindow_StatusChange
{
  ULONG methodID;
  char *txt;
   LONG percent;
};

struct MUIP_SplashWindow_ProgressChange
{
  ULONG methodID;
  struct BusyNode *busy;
};

struct MUIP_SplashWindow_SelectUser
{
  ULONG methodID;
};

#endif /* MUI_SPLASHWINDOW_H */
