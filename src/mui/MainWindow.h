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

#ifndef MUI_MAINWINDOW_H
#define MUI_MAINWINDOW_H 1

#ifndef SDI_COMPILER_H
  #include "SDI_compiler.h"
#endif

/* Exported includes */

//#include "YAM_find.h"

Object * Bourriquet_NewObject(CONST_STRPTR className, ...);

#define MUIC_MainWindow "Bourriquet_MainWindow"

#define MainWindowObject Bourriquet_NewObject(MUIC_MainWindow

#define MUIM_MainWindow_ShowErrors                    0x80005700
#define MUIM_MainWindow_Relayout                      0x80005701

struct MUIP_MainWindow_ShowErrors
{
  ULONG methodID;
};

struct MUIP_MainWindow_Relayout
{
  ULONG methodID;
};

#endif /* MUI_MAINWINDOW_H */
