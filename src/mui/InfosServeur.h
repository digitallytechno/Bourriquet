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
/* Ce fichier est automatiquement g�n�r� avec GenClasses - NE PAS EDITER!!! */

#ifndef MUI_INFOSSERVEUR_H
#define MUI_INFOSSERVEUR_H 1

#ifndef SDI_COMPILER_H
  #include "SDI_compiler.h"
#endif

/* Exported includes */

#include "Stringsizes.h"

Object * Bourriquet_NewObject(CONST_STRPTR className, ...);

#define MUIC_InfosServeur "Bourriquet_InfosServeur"

#define InfosServeurObject Bourriquet_NewObject(MUIC_InfosServeur


#endif /* MUI_INFOSSERVEUR_H */
