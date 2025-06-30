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
#ifndef ERROR_H
#define ERROR_H

#include <intuition/classusr.h>
#include "Stringsizes.h"

struct ER_GUIData
{
 	Object *WI;
  	Object *LV_ERROR;
  	Object *BT_NEXT;
  	Object *NB_ERROR;
  	Object *BT_PREV;
};
struct ER_ClassData  /* fenêtre d'erreur*/
{
 	struct ER_GUIData GUI;
  	char SliderLabel[SIZE_SMALL];
  	char ScreenTitle[SIZE_DEFAULT];
};

void ER_NewError(const char *message, ...);
void ER_NewWarning(const char *message, ...);

#endif /* ERROR_H */
