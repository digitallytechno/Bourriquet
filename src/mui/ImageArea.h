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

#ifndef MUI_IMAGEAREA_H
#define MUI_IMAGEAREA_H 1

#ifndef SDI_COMPILER_H
  #include "SDI_compiler.h"
#endif

/* Exported includes */

#include "ImageCache.h"

Object * Bourriquet_NewObject(CONST_STRPTR className, ...);

#define MUIC_ImageArea "Bourriquet_ImageArea"

#define ImageAreaObject Bourriquet_NewObject(MUIC_ImageArea

#define MUIA_ImageArea_ID                             0x80004d00
#define MUIA_ImageArea_Filename                       0x80004d01
#define MUIA_ImageArea_AltText                        0x80004d02
#define MUIA_ImageArea_Label                          0x80004d03
#define MUIA_ImageArea_FreeVert                       0x80004d04
#define MUIA_ImageArea_FreeHoriz                      0x80004d05
#define MUIA_ImageArea_ShowLabel                      0x80004d06
#define MUIA_ImageArea_MaxHeight                      0x80004d07
#define MUIA_ImageArea_MaxWidth                       0x80004d08
#define MUIA_ImageArea_NoMinHeight                    0x80004d09
#define MUIA_ImageArea_RawWidth                       0x80004d0a
#define MUIA_ImageArea_RawHeight                      0x80004d0b

/* Exported text */

#define MakeImageObject(id, file) ImageAreaObject,\
                                    MUIA_ImageArea_ID, (id),\
                                    MUIA_ImageArea_Filename, (file),\
                                  End
#define MakeImageAltObject(id, file, alt) ImageAreaObject,\
                                            MUIA_ImageArea_ID, (id),\
                                            MUIA_ImageArea_Filename, (file),\
                                            MUIA_ImageArea_AltText, (alt),\
                                          End

#endif /* MUI_IMAGEAREA_H */
